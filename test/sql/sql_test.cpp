// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "common/conf/ini.h"
#include "common/lang/string.h"
#include "common/os/process_param.h"
#include "common/seda/init.h"
#include "init.h"
#include "net/server.h"
#include <chrono>
#include <cstdlib>
#include <event.h>
#include <ghc/filesystem.hpp>
#include <gtest/gtest.h>
#include <pthread.h>
#include <sys/un.h>
#include <thread>
#include <vector>
#include <tuple>
#include <sstream>

#define MAX_MEM_BUFFER_SIZE 8192
#define SERVER_START_STOP_TIMEOUT 1s

namespace fs = ghc::filesystem;
using namespace std::string_literals;
using namespace std::chrono_literals;
std::string conf_path = "../etc/observer.ini";
std::string observer_binary = "./bin/observer";

// Generate random string
// Author: https://stackoverflow.com/a/12468109
std::string random_string(size_t length)
{
  auto randchar = []() -> char {
    const char charset[] = "0123456789"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

int init_unix_sock(const char *unix_sock_path)
{
  int sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "failed to create unix socket. %s", strerror(errno));
    return -1;
  }

  struct sockaddr_un sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sun_family = PF_UNIX;
  snprintf(sockaddr.sun_path, sizeof(sockaddr.sun_path), "%s", unix_sock_path);

  if (connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    fprintf(stderr, "failed to connect to server. unix socket path '%s'. error %s", sockaddr.sun_path, strerror(errno));
    close(sockfd);
    return -1;
  }
  return sockfd;
}

Server *init_server(std::string socket_path, std::string storage_dir)
{
  common::ProcessParam *process_param = common::the_process_param();
  process_param->set_unix_socket_path(socket_path.c_str());
  process_param->set_conf(conf_path);
  if (common::get_properties() == nullptr)
    common::get_properties() = new common::Ini();
  common::get_properties()->put("BaseDir", storage_dir, "DefaultStorageStage");
  init(process_param);

  std::map<std::string, std::string> net_section = common::get_properties()->get("NET");

  long listen_addr = INADDR_ANY;
  long max_connection_num = MAX_CONNECTION_NUM_DEFAULT;
  int port = PORT_DEFAULT;

  std::map<std::string, std::string>::iterator it = net_section.find(CLIENT_ADDRESS);
  if (it != net_section.end()) {
    std::string str = it->second;
    common::str_to_val(str, listen_addr);
  }

  it = net_section.find(MAX_CONNECTION_NUM);
  if (it != net_section.end()) {
    std::string str = it->second;
    common::str_to_val(str, max_connection_num);
  }

  if (process_param->get_server_port() > 0) {
    port = process_param->get_server_port();
    LOG_INFO("Use port config in command line: %d", port);
  } else {
    it = net_section.find(PORT);
    if (it != net_section.end()) {
      std::string str = it->second;
      common::str_to_val(str, port);
    }
  }

  ServerParam server_param;
  server_param.listen_addr = listen_addr;
  server_param.max_connection_num = max_connection_num;
  server_param.port = port;

  if (process_param->get_unix_socket_path().size() > 0) {
    server_param.use_unix_socket = true;
    server_param.unix_socket_path = process_param->get_unix_socket_path();
  }

  Server *server = new Server(server_param);
  Server::init();
  return server;
}

// TestServer is observer for test
class TestServer {
public:
  virtual ~TestServer(){};
  virtual void start(std::string data_dir, std::string socket_path){};
  virtual void stop(){};
};

class ThreadTestServer : public TestServer {
public:
  ~ThreadTestServer(){};
  static void *start_server_func(void *server)
  {
    ((Server *)server)->serve();
    return nullptr;
  }
  void start(std::string data_dir, std::string socket_path)
  {
    server = init_server(socket_path, data_dir);
    pthread_create(&pid, nullptr, ThreadTestServer::start_server_func, server);
    std::this_thread::sleep_for(50ms);
  }
  void stop()
  {
    server->shutdown();
    pthread_join(pid, nullptr);
    cleanup();
    common::cleanup_seda();
    delete server;
  }

private:
  Server *server;
  pthread_t pid;
};

class ForkTestServer : public TestServer {
public:
  ~ForkTestServer(){};
  void start(std::string data_dir, std::string socket_path)
  {
    pid = fork();
    if (pid == 0) {
      fs::current_path(data_dir);
      Server *server = init_server(socket_path, data_dir);
      server->serve();
      exit(0);
    }
    std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
  }
  void stop()
  {
    kill(pid, SIGTERM);
    waitpid(pid, nullptr, 0);
  }

private:
  pid_t pid;
};

class ExecTestServer : public TestServer {
public:
  ~ExecTestServer(){};
  void start(std::string data_dir, std::string socket_path)
  {
    pid = fork();
    if (pid == 0) {
      fs::current_path(data_dir);
      if (execl(observer_binary.c_str(), "observer", "-s", socket_path.c_str(), "-f", conf_path.c_str(), NULL) != 0) {
        std::cerr << "Failed to start observer" << std::endl;
      }
      exit(0);
    }
    std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
  }
  void stop()
  {
    kill(pid, SIGTERM);
    std::this_thread::sleep_for(.5s);
    kill(pid, SIGTERM);
    waitpid(pid, nullptr, 0);
  }

private:
  pid_t pid;
};

using DefaultTestServer = ThreadTestServer;

class SQLTest : public ::testing::Test {
public:
  SQLTest()
  {
    const char *test_server_workaround = std::getenv("SQL_TEST_SERVER_WORKAROUND");
    if (test_server_workaround == nullptr) {
      server = new DefaultTestServer();
    } else if (strcmp(test_server_workaround, "fork") == 0) {
      server = new ForkTestServer();
    } else if (strcmp(test_server_workaround, "exec") == 0) {
      server = new ExecTestServer();
    } else if (strcmp(test_server_workaround, "thread") == 0) {
      server = new ThreadTestServer();
    } else {
      server = new DefaultTestServer();
    }
  }

  ~SQLTest()
  {
    delete server;
  }

  void SetUp() override
  {
    data_dir = fs::temp_directory_path() / ("miniob-sql-test-" + random_string(6));
    socket_path = data_dir / ".sock";

    fs::create_directory(data_dir);
    fs::current_path(data_dir);
    server->start(data_dir, socket_path);

    sockfd[0] = init_unix_sock(socket_path.c_str());
    sockfd[1] = init_unix_sock(socket_path.c_str());
  }

  void TearDown() override
  {
    close(sockfd[0]);
    close(sockfd[1]);
    server->stop();
    // FIXME: Randomly abort
    fs::remove_all(data_dir);
  }

  void restart()
  {
    close(sockfd[0]);
    close(sockfd[1]);
    server->stop();

    server->start(data_dir, socket_path);

    sockfd[0] = init_unix_sock(socket_path.c_str());
    sockfd[1] = init_unix_sock(socket_path.c_str());
  }

  std::string exec_sql(std::string sql, int client = 0)
  {
    char send_bytes;
    if ((send_bytes = write(sockfd[client], sql.c_str(), sql.length() + 1)) == -1) {
      fprintf(stderr, "send error: %d:%s \n", errno, strerror(errno));
      return "FAILURE";
    }

    memset(recv_buf, 0, MAX_MEM_BUFFER_SIZE);
    int len = 0;
    std::string resp;
    while ((len = recv(sockfd[client], recv_buf, MAX_MEM_BUFFER_SIZE, 0)) > 0) {
      bool msg_end = false;
      int last_char;
      for (last_char = 0; last_char < len; last_char++) {
        if (0 == recv_buf[last_char]) {
          msg_end = true;
          break;
        }
      }

      resp.append(recv_buf, recv_buf + last_char);
      if (msg_end) {
        break;
      }
      memset(recv_buf, 0, MAX_MEM_BUFFER_SIZE);
    }

    return resp;
  }

private:
  fs::path data_dir;
  fs::path socket_path;
  TestServer *server;

  int sockfd[2];
  char recv_buf[MAX_MEM_BUFFER_SIZE];
};

// ########     ###     ######  ####  ######
// ##     ##   ## ##   ##    ##  ##  ##    ##
// ##     ##  ##   ##  ##        ##  ##
// ########  ##     ##  ######   ##  ##
// ##     ## #########       ##  ##  ##
// ##     ## ##     ## ##    ##  ##  ##    ##
// ########  ##     ##  ######  ####  ######

TEST_F(SQLTest, BasicCreateTableShouldWork)
{
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
}

TEST_F(SQLTest, BasicInsertShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
}

TEST_F(SQLTest, BasicInsertWithWrongValueShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  // typecast is enabled
  // ASSERT_EQ(exec_sql("insert into t values (1, \"A\");"), "FAILURE\n");
  ASSERT_EQ(exec_sql("insert into t values (1);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n");
}

TEST_F(SQLTest, BasicSelectShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select t.a from t;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select a, b from t;"), "a | b\n1 | 2\n");
}

// TODO: Fix issues that select after sync not work
TEST_F(SQLTest, DISABLED_BasicSelectAfterSyncShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("sync;"), "SUCCESS");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select t.a from t;"), "a\n1\n");
}

TEST_F(SQLTest, BasicSelectWithConditionShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  // ASSERT_EQ(exec_sql("select * from t where 1 = 1;"), "a | b\n1 | 2\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where a = 1;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select * from t where t.a = 1;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t where a = 1;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select * from t where a < 2 and a > 1;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where 1 = a;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select * from t where 1 = t.a;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t where 1 = a;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select * from t where 2 > a and 1 < a;"), "a | b\n");
}

TEST_F(SQLTest, BasicSelectWithIndex)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a > 1;"), "a | b\n2 | 2\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where a = 2;"), "a | b\n2 | 2\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where a < 2;"), "a | b\n1 | 2\n");
}

TEST_F(SQLTest, BasicSelectWithIndexEqualToMinValue)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a = -1;"), "a | b\n");
}

TEST_F(SQLTest, BasicInvalidIndex)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t2 on t(b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t3 on t(c);"), "FAILURE\n");
}

TEST_F(SQLTest, BasicExtFloatFormat)
{
  ASSERT_EQ(exec_sql("create table t(a float);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1.0);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1.2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1.23);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1.257);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (10.0);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t;"), "a\n1\n1.2\n1.23\n1.26\n10\n");
}

TEST_F(SQLTest, BasicExtConditionBetweenDifferentType)
{
  ASSERT_EQ(exec_sql("create table t(a int, b float);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1.0);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1.2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3, 1.23);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a > b;"), "a | b\n3 | 1.23\n");
  ASSERT_EQ(exec_sql("select * from t where a > 1.2;"), "a | b\n3 | 1.23\n");
  ASSERT_EQ(exec_sql("select * from t where 1.2 < a;"), "a | b\n3 | 1.23\n");
}

TEST_F(SQLTest, BasicIndexShouldBeValidAfterRestart)
{
  ASSERT_EQ(exec_sql("create table t_basic(id int, age int, name char, score float);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_id on t_basic(id);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t_basic values(1, 1, 'a', 1.0);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t_basic values(2, 2, 'b', 2.0);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t_basic values(4, 4, 'c', 3.0);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t_basic values(3, 3, 'd', 4.0);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t_basic values(5, 5, 'e', 5.5);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t_basic values(6, 6, 'f', 6.6);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t_basic values(7, 7, 'g', 7.7);"), "SUCCESS\n");
  restart();
  ASSERT_EQ(exec_sql("select id from t_basic where id > 5;"),
      "id\n"
      "6\n7\n");
  ASSERT_EQ(exec_sql("delete from t_basic where id = 1;"), "SUCCESS\n");
}

//  ######  ######## ##       ########  ######  ########
// ##    ## ##       ##       ##       ##    ##    ##
// ##       ##       ##       ##       ##          ##
//  ######  ######   ##       ######   ##          ##
//       ## ##       ##       ##       ##          ##
// ##    ## ##       ##       ##       ##    ##    ##
//  ######  ######## ######## ########  ######     ##

// ##     ## ######## ########    ###
// ###   ### ##          ##      ## ##
// #### #### ##          ##     ##   ##
// ## ### ## ######      ##    ##     ##
// ##     ## ##          ##    #########
// ##     ## ##          ##    ##     ##
// ##     ## ########    ##    ##     ##

TEST_F(SQLTest, SelectMetaShouldResponseHeadWhenNoData)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a\n");
}

TEST_F(SQLTest, SelectMetaInvalidTableShouldFailure)
{
  ASSERT_EQ(exec_sql("select * from t2;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaInvalidTableShouldFailureInMultiTables)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t2, t;"), "FAILURE\n");
}

TEST_F(SQLTest, DISABLED_SelectMetaSameTableShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  // TODO: Undefined behavior
  ASSERT_EQ(exec_sql("select * from t, t;"), "t.a\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidColumnShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t3(a int,b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select b from t;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select t2.a from t;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select b,* from t3;"), "b | a | b\n");
  ASSERT_EQ(exec_sql("select a,b, * from t3;"), "a | b | a | b\n");
  ASSERT_EQ(exec_sql("select a,b from t3;"), "a | b\n");
  ASSERT_EQ(exec_sql("select *,b from t3;"), "a | b | b\n");
  ASSERT_EQ(exec_sql("select * from t3 where t3.a > 0;"), "a | b\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidColumnInMultiTablesShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select t.b from t, t2;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select *,a from t, t2;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectIndeterminableColumnInMultiTablesShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select a from t, t2;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidConditionShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t where a > 0;"), "a\n");

  ASSERT_EQ(exec_sql("select * from t where b > a;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where b > 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where 1 > b;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where b = 1;"), "FAILURE\n");

  ASSERT_EQ(exec_sql("select * from t where b < c;"), "FAILURE\n");

  ASSERT_EQ(exec_sql("select * from t where t.b > 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where t2.b > 1;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectValidConditionInMultiTablesShouldSuccess)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");
  // ASSERT_NE(exec_sql("select * from t, t2 where a > b;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select * from t, t2 where t.a > t2.b;"), "FAILURE\n");
  // ASSERT_NE(exec_sql("select * from t, t2 where a > 1 and b > 1 and t.a > t2.b;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select t.*, t2.b from t,t2; "), "FAILURE\n");
  ASSERT_NE(exec_sql("select * from t,t2 where t2.b > 10;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select t.*,t2.* from t,t2;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidConditionInMultiTablesShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where a > c;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where t.a > t3.b;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where a > 1 and b > 1 and t.a > t3.b;"), "FAILURE\n");

  ASSERT_EQ(exec_sql("create table t3(c char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t, t3 where a > c;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectIndeterminableConditionInMultiTablesShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where a > 0;"), "FAILURE\n");
}

// ########  ########   #######  ########
// ##     ## ##     ## ##     ## ##     ##
// ##     ## ##     ## ##     ## ##     ##
// ##     ## ########  ##     ## ########
// ##     ## ##   ##   ##     ## ##
// ##     ## ##    ##  ##     ## ##
// ########  ##     ##  #######  ##

// ########    ###    ########  ##       ########
//    ##      ## ##   ##     ## ##       ##
//    ##     ##   ##  ##     ## ##       ##
//    ##    ##     ## ########  ##       ######
//    ##    ######### ##     ## ##       ##
//    ##    ##     ## ##     ## ##       ##
//    ##    ##     ## ########  ######## ########

TEST_F(SQLTest, DropTableShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n");
  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
  ASSERT_EQ(exec_sql("select * from t;"), "FAILURE\n");
}

TEST_F(SQLTest, DropTableWithIndexShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a_idx on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n");
  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
}

TEST_F(SQLTest, DropTableWithDataShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a\n1\n");
  exec_sql("sync;");

  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a\n");
}

TEST_F(SQLTest, DropTableFailureIfNotExist)
{
  ASSERT_EQ(exec_sql("drop table t2;"), "FAILURE\n");
}

TEST_F(SQLTest, DropTableCanCreateAgain)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n");
  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n");
}

TEST_F(SQLTest, DropTableWithIndexCreateAgain)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a_idx on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n");

  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");

  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a_idx on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n");
}

TEST_F(SQLTest, DropTableRealExample)
{
  ASSERT_EQ(exec_sql("CREATE table Drop_table_6(ID int ,NAME char,AGE int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Drop_table_6 VALUES (1,'OB',12);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Drop_table_6 VALUES (2,'ODC',12);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("DELETE FROM Drop_table_6 WHERE ID = 2;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("SELECT * FROM Drop_table_6;"),
      "ID | NAME | AGE\n"
      "1 | OB | 12\n");
  ASSERT_EQ(exec_sql("DROP TABLE Drop_table_6;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("SELECT * FROM Drop_table_6;"), "FAILURE\n");
}

// ##     ## ########  ########     ###    ######## ########
// ##     ## ##     ## ##     ##   ## ##      ##    ##
// ##     ## ##     ## ##     ##  ##   ##     ##    ##
// ##     ## ########  ##     ## ##     ##    ##    ######
// ##     ## ##        ##     ## #########    ##    ##
// ##     ## ##        ##     ## ##     ##    ##    ##
//  #######  ##        ########  ##     ##    ##    ########

TEST_F(SQLTest, UpdateShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 100;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "100 | 1\n"
      "100 | 10\n"
      "100 | 3\n"
      "100 | 5\n");
}

TEST_F(SQLTest, UpdateWithConditionsShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 100 where a = 1;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "100 | 1\n"
      "100 | 10\n"
      "2 | 3\n"
      "2 | 5\n");
}

TEST_F(SQLTest, UpdateWithIndexShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a = 1;"),
      "a | b\n"
      "1 | 1\n"
      "1 | 10\n");

  ASSERT_EQ(exec_sql("update t set a = 100 where a = 1;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "100 | 1\n"
      "100 | 10\n"
      "2 | 3\n"
      "2 | 5\n");
  ASSERT_EQ(exec_sql("select * from t where a = 1;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where a = 100;"),
      "a | b\n"
      "100 | 1\n"
      "100 | 10\n");
}

TEST_F(SQLTest, UpdateWithInvalidColumnShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set c = 100 where a = 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 1\n"
      "1 | 10\n"
      "2 | 3\n"
      "2 | 5\n");
}

TEST_F(SQLTest, UpdateWithInvalidConditionShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 100 where c = 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 1\n"
      "1 | 10\n"
      "2 | 3\n"
      "2 | 5\n");
}

TEST_F(SQLTest, UpdateMultiValueShouldSuccess)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 100, b = 100 where a = 1;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "100 | 100\n"
      "100 | 100\n"
      "2 | 3\n"
      "2 | 5\n");
}

// all value is valid if typecast is enabled
TEST_F(SQLTest, DISABLED_UpdateWithInvalidValueShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 1.0 where a = 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 1\n"
      "1 | 10\n"
      "2 | 3\n"
      "2 | 5\n");
}

// ########     ###    ######## ########
// ##     ##   ## ##      ##    ##
// ##     ##  ##   ##     ##    ##
// ##     ## ##     ##    ##    ######
// ##     ## #########    ##    ##
// ##     ## ##     ##    ##    ##
// ########  ##     ##    ##    ########

TEST_F(SQLTest, DateCanCreateTable)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
      "t(\n"
      "\tfield name=__trx, type=ints, len=4, visible=no, nullable=no\n"
      "\tfield name=__null, type=ints, len=4, visible=no, nullable=no\n"
      "\tfield name=a, type=ints, len=4, visible=yes, nullable=no\n"
      "\tfield name=d, type=date, len=4, visible=yes, nullable=no\n"
      ")\n");
}

TEST_F(SQLTest, DateInsertShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2100-2-29');"), "FAILURE\n");
}

TEST_F(SQLTest, DateSelectShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '20-1-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '4000-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | d\n1 | 0020-01-10\n1 | 2020-10-10\n1 | 2020-01-01\n1 | "
      "4000-01-01\n");
}

TEST_F(SQLTest, DateSelectWhereShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select d from t where d > '2020-12-1';"), "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2021-1-1';"), "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2020-1-21';"), "d\n");
  ASSERT_EQ(exec_sql("select d from t where d < '2020-12-1';"), "d\n2020-10-10\n");
}

TEST_F(SQLTest, DateSelectWithIndexShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_d on t(d);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select d from t where d > '2020-12-1';"), "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2021-1-1';"), "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2020-1-21';"), "d\n");
  ASSERT_EQ(exec_sql("select d from t where d < '2020-12-1';"), "d\n2020-10-10\n");
}

TEST_F(SQLTest, DateCharsNotBeAffected)
{
  ASSERT_EQ(exec_sql("create table t(a int, b char(20), d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-1-1', '2020-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b | d\n"
      "1 | 2020-1-1 | 2020-01-01\n");
}

TEST_F(SQLTest, DateUpdateShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set d='2022-2-22' where d < '2020-12-31';"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | d\n"
      "1 | 2022-02-22\n"
      "1 | 2021-01-01\n");
}

TEST_F(SQLTest, DateUpdateWithIndexShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_d on t(d);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set d='2022-2-22' where d < '2020-12-31';"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t where d='2022-2-22';"),
      "a | d\n"
      "1 | 2022-02-22\n");
}

TEST_F(SQLTest, DateUpdateInvalidDateShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set d='2022-2-30' where d < '2020-12-31';"), "FAILURE\n");
}

TEST_F(SQLTest, DateSelectWhereInvalidDateShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select d from t where d > '2020-13-1';"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2017-2-29';"), "FAILURE\n");
}

// ######## ##    ## ########  ########  ######     ###     ######  ########
//    ##     ##  ##  ##     ## ##       ##    ##   ## ##   ##    ##    ##
//    ##      ####   ##     ## ##       ##        ##   ##  ##          ##
//    ##       ##    ########  ######   ##       ##     ##  ######     ##
//    ##       ##    ##        ##       ##       #########       ##    ##
//    ##       ##    ##        ##       ##    ## ##     ## ##    ##    ##
//    ##       ##    ##        ########  ######  ##     ##  ######     ##

TEST_F(SQLTest, TypeCastInsertShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(i int, f float, d date, s char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('1', '1.0', '2020-10-10', 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('2a', '1.6', '2021-1-1', 1.1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "i | f | d | s\n"
      "1 | 1 | 2020-10-10 | 1\n"
      "2 | 1.6 | 2021-01-01 | 1.1\n");
}

TEST_F(SQLTest, TypeCastInsertFloatShouldBeRound)
{
  ASSERT_EQ(exec_sql("create table t(i int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(0.4);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(0.5);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(0.6);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "i\n"
      "0\n"
      "1\n"
      "1\n");
}

TEST_F(SQLTest, TypeCastConditionShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(i int, f float, d date, s char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, 1.0, '2020-10-10', '1a');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(2, 1.6, '2021-1-1', '1.1');"), "SUCCESS\n");

  // Both CHAR
  ASSERT_EQ(exec_sql("select s from t where s >= '1a';"), "s\n1a\n");

  // Both INT
  ASSERT_EQ(exec_sql("select i from t where i > 1;"), "i\n2\n");

  // INT OP CHAR
  ASSERT_EQ(exec_sql("select i from t where i > '1';"), "i\n2\n");
  ASSERT_EQ(exec_sql("select i from t where i > '1.5';"), "i\n2\n");
  ASSERT_EQ(exec_sql("select i from t where i > '1a';"), "i\n2\n");
  ASSERT_EQ(exec_sql("select i from t where i > 'aa';"), "i\n1\n2\n");  // 'aa' cast as 0

  // FLOAT OP CHAR
  ASSERT_EQ(exec_sql("select f from t where f > '1';"), "f\n1.6\n");
  ASSERT_EQ(exec_sql("select f from t where f > '1.5';"), "f\n1.6\n");
  ASSERT_EQ(exec_sql("select f from t where f > '1a';"), "f\n1.6\n");
  ASSERT_EQ(exec_sql("select f from t where f > 'aa';"), "f\n1\n1.6\n");  // 'aa' cast as 0
}

TEST_F(SQLTest, TypeCastWithConditionIndexShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(i int, f float, d date, s char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_i on t(i);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_f on t(f);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_d on t(d);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_s on t(s);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values(1, 1.0, '2020-10-10', '1a');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(2, 1.6, '2021-1-1', '1.1');"), "SUCCESS\n");

  // Both CHAR
  ASSERT_EQ(exec_sql("select s from t where s >= '1a';"), "s\n1a\n");

  // Both INT
  ASSERT_EQ(exec_sql("select i from t where i > 1;"), "i\n2\n");

  // INT OP CHAR
  ASSERT_EQ(exec_sql("select i from t where i > '1';"), "i\n2\n");
  ASSERT_EQ(exec_sql("select i from t where i > '1.5';"), "i\n2\n");
  ASSERT_EQ(exec_sql("select i from t where i > '1a';"), "i\n2\n");
  ASSERT_EQ(exec_sql("select i from t where i > 'aa';"), "i\n1\n2\n");  // 'aa' cast as 0

  // FLOAT OP CHAR
  ASSERT_EQ(exec_sql("select f from t where f > '1';"), "f\n1.6\n");
  ASSERT_EQ(exec_sql("select f from t where f > '1.5';"), "f\n1.6\n");
  ASSERT_EQ(exec_sql("select f from t where f > '1a';"), "f\n1.6\n");
  ASSERT_EQ(exec_sql("select f from t where f > 'aa';"), "f\n1\n1.6\n");  // 'aa' cast as 0
}

TEST_F(SQLTest, TypeCastUpdateShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(i int, f float, c char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, 1.0, 'a');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set i = '2.0';"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select i from t;"), "i\n2\n");

  ASSERT_EQ(exec_sql("update t set i = 3.0;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select i from t;"), "i\n3\n");

  ASSERT_EQ(exec_sql("update t set f = 4;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select f from t;"), "f\n4\n");

  ASSERT_EQ(exec_sql("update t set f = '5.5';"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select f from t;"), "f\n5.5\n");

  ASSERT_EQ(exec_sql("update t set c = 1;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select c from t;"), "c\n1\n");

  ASSERT_EQ(exec_sql("update t set c = 1.5;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select c from t;"), "c\n1.5\n");
}

// ##       #### ##    ## ########
// ##        ##  ##   ##  ##
// ##        ##  ##  ##   ##
// ##        ##  #####    ######
// ##        ##  ##  ##   ##
// ##        ##  ##   ##  ##
// ######## #### ##    ## ########

TEST_F(SQLTest, LikeShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(s char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('adc');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('eagh');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('dddd');"), "SUCCESS\n");

  std::vector<std::tuple<std::string, std::string>> cases = {
      {"___", "adc\n"},
      {"____", "eagh\ndddd\n"},
      {"%d%", "adc\ndddd\n"},
      {"%a__", "adc\neagh\n"},
  };

  for (auto &test_case : cases) {
    auto &like_expr = std::get<0>(test_case);
    auto &res = std::get<1>(test_case);

    ASSERT_EQ(exec_sql("select s from t where s like '"s + like_expr + "';"), "s\n"s + res);
  }
}

TEST_F(SQLTest, NotLikeShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(s char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('adc');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('eagh');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('dddd');"), "SUCCESS\n");

  std::vector<std::tuple<std::string, std::string>> cases = {
      {"___", "eagh\ndddd\n"},
      {"____", "adc\n"},
      {"%d%", "eagh\n"},
      {"%a__", "dddd\n"},
  };

  for (auto &test_case : cases) {
    auto &like_expr = std::get<0>(test_case);
    auto &res = std::get<1>(test_case);

    ASSERT_EQ(exec_sql("select s from t where s not like '"s + like_expr + "';"), "s\n"s + res);
  }
}

TEST_F(SQLTest, MultiLikeShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(s char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('adc');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('ccc');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select s from t where s not like 'a%' and s not like 'c%';"), "s\n");
}

TEST_F(SQLTest, LikeShouldBeCaseInsensitivity)
{
  ASSERT_EQ(exec_sql("create table t(s char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('aaa');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values('AAA');"), "SUCCESS\n");

  std::vector<std::tuple<std::string, std::string>> cases = {
      {"%a%", "aaa\nAAA\n"},
      {"%A%", "aaa\nAAA\n"},
  };

  for (auto &test_case : cases) {
    auto &like_expr = std::get<0>(test_case);
    auto &res = std::get<1>(test_case);

    ASSERT_EQ(exec_sql("select s from t where s like '"s + like_expr + "';"), "s\n"s + res);
  }
}

// ##     ## ##    ## ####  #######  ##     ## ########
// ##     ## ###   ##  ##  ##     ## ##     ## ##
// ##     ## ####  ##  ##  ##     ## ##     ## ##
// ##     ## ## ## ##  ##  ##     ## ##     ## ######
// ##     ## ##  ####  ##  ##  ## ## ##     ## ##
// ##     ## ##   ###  ##  ##    ##  ##     ## ##
//  #######  ##    ## ####  ##### ##  #######  ########

TEST_F(SQLTest, UniqueIndexMetaShouldWork)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_b on t(b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
      "t(\n"
      "\tfield name=__trx, type=ints, len=4, visible=no, nullable=no\n"
      "\tfield name=__null, type=ints, len=4, visible=no, nullable=no\n"
      "\tfield name=a, type=ints, len=4, visible=yes, nullable=no\n"
      "\tfield name=b, type=ints, len=4, visible=yes, nullable=no\n"
      "\tindex name=t_a, field=a, unique=yes\n"
      "\tindex name=t_b, field=b, unique=no\n"
      ")\n");
}

TEST_F(SQLTest, UniqueIndexInsertConflictRecordShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 2\n"
      "2 | 2\n");
}

TEST_F(SQLTest, UniqueIndexOnExistsData)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 2\n"
      "2 | 2\n");
}

TEST_F(SQLTest, UniqueIndexUpdateConflictRecordShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 1 where b = 3;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 2\n"
      "2 | 3\n");
  // Trigger index scanner
  ASSERT_EQ(exec_sql("select * from t where a > 0;"),
      "a | b\n"
      "1 | 2\n"
      "2 | 3\n");
}

TEST_F(SQLTest, UniqueMultiIndexInsertConflictRecordShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a, b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 2\n"
      "2 | 2\n"
      "1 | 3\n");
}

TEST_F(SQLTest, UniqueMultiIndexUpdateConflictRecordShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a, b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set a = 1 where a = 2;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 2\n"
      "2 | 2\n"
      "1 | 3\n");

  ASSERT_EQ(exec_sql("create table t2 (a int, b int, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t2_a on t2(b, c);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (1, 1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (2, 1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (3, 2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t2 set b = 2, c = 2 where a = 1;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t2 set b = 1, c = 2 where a = 3;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t2;"),
      "a | b | c\n"
      "1 | 2 | 2\n"
      "2 | 1 | 2\n"
      "3 | 2 | 3\n");
}

TEST_F(SQLTest, UniqueIndexMultiIndexOnExistsData)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a, b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 2\n"
      "2 | 2\n"
      "1 | 3\n");
}

TEST_F(SQLTest, UniqueIndexManyIndexsInsert)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int, c int, d int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index i1 on t(b, c);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index i2 on t(c, d);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1, 2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2, 3, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (3, 2, 2, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("insert into t values (3, 2, 2, 4);"), "SUCCESS\n");
}

TEST_F(SQLTest, UniqueIndexManyIndexsUpdate)
{
  ASSERT_EQ(exec_sql("create table t (a int, b int, c int, d int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index i1 on t(b, c);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index i2 on t(c, d);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1, 2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2, 3, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set c = 2 where a = 2;"), "FAILURE\n");

  ASSERT_EQ(exec_sql("insert into t values (3, 2, 2, 4);"), "SUCCESS\n");
}

// ######## ######## ##     ## ########
//    ##    ##        ##   ##     ##
//    ##    ##         ## ##      ##
//    ##    ######      ###       ##
//    ##    ##         ## ##      ##
//    ##    ##        ##   ##     ##
//    ##    ######## ##     ##    ##

TEST_F(SQLTest, TextCreateTableShouldWork)
{
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
      "t(\n"
      "\tfield name=__trx, type=ints, len=4, visible=no, nullable=no\n"
      "\tfield name=__null, type=ints, len=4, visible=no, nullable=no\n"
      "\tfield name=a, type=text, len=4, visible=yes, nullable=no\n"
      ")\n");
}

TEST_F(SQLTest, TextInsertSelectShouldWork)
{
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values ('aa');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a\n"
      "aa\n");
}

TEST_F(SQLTest, TextUpdateShouldWork)
{
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values ('aa');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a\n"
      "aa\n");
  ASSERT_EQ(exec_sql("update t set a = 'bb';"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a\n"
      "bb\n");
}

TEST_F(SQLTest, TextInsertSelectVeryLongTextShouldWork)
{
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  std::string long_text = random_string(4096);
  ASSERT_EQ(exec_sql(std::string() + "insert into t values ('" + long_text + "');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), std::string() + "a\n" + long_text + "\n");
}

TEST_F(SQLTest, TextInsertSelectOverTextShouldWork)
{
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  std::string long_text = random_string(4097);
  ASSERT_EQ(exec_sql(std::string() + "insert into t values ('" + long_text + "');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), std::string() + "a\n" + long_text.substr(0, 4096) + "\n");
}

// #### ##    ##  ######  ######## ########  ########
//  ##  ###   ## ##    ## ##       ##     ##    ##
//  ##  ####  ## ##       ##       ##     ##    ##
//  ##  ## ## ##  ######  ######   ########     ##
//  ##  ##  ####       ## ##       ##   ##      ##
//  ##  ##   ### ##    ## ##       ##    ##     ##
// #### ##    ##  ######  ######## ##     ##    ##

TEST_F(SQLTest, InsertMultiTupleShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1), (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | 1\n"
      "2 | 3\n");
}

TEST_F(SQLTest, InsertInvalidTupleShouldInsertNothing)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1), (1), (2, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n");
  ASSERT_EQ(exec_sql("insert into t values (1), (1), (2, 3), (4);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n");
}

TEST_F(SQLTest, InsertMultiTupleShouldBeAtomic)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null), (null, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n");
}

//  ######  ##        #######   ######
// ##    ## ##       ##     ## ##    ##
// ##       ##       ##     ## ##
// ##       ##       ##     ## ##   ####
// ##       ##       ##     ## ##    ##
// ##    ## ##       ##     ## ##    ##
//  ######  ########  #######   ######

TEST_F(SQLTest, CLogShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(id int);"), "SUCCESS\n");

  // client 0
  ASSERT_EQ(exec_sql("begin;", 0), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1);", 0), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(2);", 0), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set id = 5 where id = 2;", 0), "SUCCESS\n");

  // client 1
  ASSERT_EQ(exec_sql("begin;", 1), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(3);", 1), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set id = 4 where id = 5;", 1), "SUCCESS\n");

  // client 0
  ASSERT_EQ(exec_sql("select * from t;", 0),
      "id\n"
      "1\n4\n3\n");
  ASSERT_EQ(exec_sql("commit;", 0), "SUCCESS\n");

  restart();

  // client 0
  ASSERT_EQ(exec_sql("select * from t;", 0),
      "id\n"
      "1\n5\n");
}

// ##     ## ##     ## ##       ######## ####
// ###   ### ##     ## ##          ##     ##
// #### #### ##     ## ##          ##     ##
// ## ### ## ##     ## ##          ##     ##
// ##     ## ##     ## ##          ##     ##
// ##     ## ##     ## ##          ##     ##
// ##     ##  #######  ########    ##    ####
// #### ##    ## ########  ######## ##     ##
//  ##  ###   ## ##     ## ##        ##   ##
//  ##  ####  ## ##     ## ##         ## ##
//  ##  ## ## ## ##     ## ######      ###
//  ##  ##  #### ##     ## ##         ## ##
//  ##  ##   ### ##     ## ##        ##   ##
// #### ##    ## ########  ######## ##     ##

TEST_F(SQLTest, MultiIndexCanCreate)
{
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a, b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t2 on t(b, c);"), "SUCCESS\n");
}

TEST_F(SQLTest, MultiIndexInvalidAttributeShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(d);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a, d);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("create index t1 on t(d, a);"), "FAILURE\n");
}

TEST_F(SQLTest, MultiIndexDuplicateShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t0 on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("create index t2 on t(a, b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t3 on t(a, b);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("create index t4 on t(b, a);"), "SUCCESS\n");
}

//  ######  ##     ##  #######  ##      ##
// ##    ## ##     ## ##     ## ##  ##  ##
// ##       ##     ## ##     ## ##  ##  ##
//  ######  ######### ##     ## ##  ##  ##
//       ## ##     ## ##     ## ##  ##  ##
// ##    ## ##     ## ##     ## ##  ##  ##
//  ######  ##     ##  #######   ###  ###
// #### ##    ## ########  ######## ##     ##
//  ##  ###   ## ##     ## ##        ##   ##
//  ##  ####  ## ##     ## ##         ## ##
//  ##  ## ## ## ##     ## ######      ###
//  ##  ##  #### ##     ## ##         ## ##
//  ##  ##   ### ##     ## ##        ##   ##
// #### ##    ## ########  ######## ##     ##

TEST_F(SQLTest, ShowIndexNonIndex)
{
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show index from t;"), "TABLE | NON_UNIQUE | KEY_NAME | SEQ_IN_INDEX | COLUMN_NAME\n");
}

TEST_F(SQLTest, ShowIndexSingleIndex)
{
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t2 on t(b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show index from t;"),
      "TABLE | NON_UNIQUE | KEY_NAME | SEQ_IN_INDEX | COLUMN_NAME\n"
      "t | 1 | t1 | 1 | a\n"
      "t | 0 | t2 | 1 | b\n");
}

TEST_F(SQLTest, ShowIndexMultiIndex)
{
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a, b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t2 on t(b, a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show index from t;"),
      "TABLE | NON_UNIQUE | KEY_NAME | SEQ_IN_INDEX | COLUMN_NAME\n"
      "t | 1 | t1 | 1 | a\n"
      "t | 1 | t1 | 2 | b\n"
      "t | 0 | t2 | 1 | b\n"
      "t | 0 | t2 | 2 | a\n");
}

TEST_F(SQLTest, ShowIndexNonExists)
{
  ASSERT_EQ(exec_sql("show index from t;"), "FAILURE\n");
}

//  ######  ######## ##       ######## ########
// ##    ## ##       ##       ##          ##   
// ##       ##       ##       ##          ##   
//  ######  ######   ##       ######      ##   
//       ## ##       ##       ##          ##   
// ##    ## ##       ##       ##          ##   
//  ######  ######## ######## ########    ##   
// ########    ###    ########  ##       ########  ######
//    ##      ## ##   ##     ## ##       ##       ##    ##
//    ##     ##   ##  ##     ## ##       ##       ##
//    ##    ##     ## ########  ##       ######    ######
//    ##    ######### ##     ## ##       ##             ##
//    ##    ##     ## ##     ## ##       ##       ##    ##
//    ##    ##     ## ########  ######## ########  ######
TEST_F(SQLTest, SelectTablesOfficalExample)
{
  ASSERT_EQ(exec_sql("CREATE TABLE Select_tables_1(id int, age int, u_name char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("CREATE TABLE Select_tables_2(id int, age int, u_name char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("CREATE TABLE Select_tables_3(id int, res int, u_name char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("CREATE TABLE Select_tables_4(id int, age int, u_name char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("CREATE TABLE Select_tables_5(id int, res int, u_name char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_1 VALUES (1,18,'a');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_1 VALUES (2,15,'b');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_2 VALUES (1,20,'a');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_2 VALUES (2,21,'c');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_3 VALUES (1,35,'a');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_3 VALUES (2,37,'a');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_4 VALUES (1, 2, 'a');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_4 VALUES (1, 3, 'b');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_4 VALUES (2, 2, 'c');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_4 VALUES (2, 4, 'd');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_5 VALUES (1, 10, 'g');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_5 VALUES (1, 11, 'f');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("INSERT INTO Select_tables_5 VALUES (2, 12, 'c');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("SELECT * FROM Select_tables_1,Select_tables_2,Select_tables_3;"),
      "Select_tables_1.id | Select_tables_1.age | Select_tables_1.u_name | Select_tables_2.id | Select_tables_2.age | "
      "Select_tables_2.u_name | Select_tables_3.id | Select_tables_3.res | Select_tables_3.u_name\n"
      "1 | 18 | a | 1 | 20 | a | 1 | 35 | a\n"
      "2 | 15 | b | 1 | 20 | a | 1 | 35 | a\n"
      "1 | 18 | a | 2 | 21 | c | 1 | 35 | a\n"
      "2 | 15 | b | 2 | 21 | c | 1 | 35 | a\n"
      "1 | 18 | a | 1 | 20 | a | 2 | 37 | a\n"
      "2 | 15 | b | 1 | 20 | a | 2 | 37 | a\n"
      "1 | 18 | a | 2 | 21 | c | 2 | 37 | a\n"
      "2 | 15 | b | 2 | 21 | c | 2 | 37 | a\n");
  ASSERT_EQ(exec_sql("SELECT * FROM Select_tables_1,Select_tables_2,Select_tables_3 WHERE "
                     "Select_tables_1.id=Select_tables_2.id AND Select_tables_3.res=35;"),
      "Select_tables_1.id | Select_tables_1.age | Select_tables_1.u_name | Select_tables_2.id | Select_tables_2.age | "
      "Select_tables_2.u_name | Select_tables_3.id | Select_tables_3.res | Select_tables_3.u_name\n"
      "1 | 18 | a | 1 | 20 | a | 1 | 35 | a\n"
      "2 | 15 | b | 2 | 21 | c | 1 | 35 | a\n");
  ASSERT_EQ(exec_sql("Select Select_tables_1.res FROM Select_tables_1,Select_tables_2,Select_tables_3;"), "FAILURE\n");

  ASSERT_EQ(
      exec_sql("SELECT * FROM Select_tables_1,Select_tables_2,Select_tables_3 WHERE "
               "Select_tables_1.u_name=Select_tables_2.u_name AND Select_tables_2.u_name=Select_tables_3.u_name;"),
      "Select_tables_1.id | Select_tables_1.age | Select_tables_1.u_name | Select_tables_2.id | Select_tables_2.age | "
      "Select_tables_2.u_name | Select_tables_3.id | Select_tables_3.res | Select_tables_3.u_name\n"
      "1 | 18 | a | 1 | 20 | a | 1 | 35 | a\n"
      "1 | 18 | a | 1 | 20 | a | 2 | 37 | a\n");
  ASSERT_EQ(exec_sql("SELECT * FROM Select_tables_1,Select_tables_2,Select_tables_3 WHERE Select_tables_1.age<18 AND "
                     "Select_tables_2.u_name='c' AND Select_tables_3.res=35 AND Select_tables_1.id=Select_tables_2.id "
                     "AND Select_tables_2.id=Select_tables_3.id;"),
      "Select_tables_1.id | Select_tables_1.age | Select_tables_1.u_name | Select_tables_2.id | Select_tables_2.age | "
      "Select_tables_2.u_name | Select_tables_3.id | Select_tables_3.res | Select_tables_3.u_name\n");
  ASSERT_EQ(exec_sql("SELECT Select_tables_2.age FROM Select_tables_1,Select_tables_2 WHERE Select_tables_1.age<18 AND "
                     "Select_tables_2.u_name='c' AND Select_tables_1.id=Select_tables_2.id;"),
      "Select_tables_2.age\n"
      "21\n");
  ASSERT_EQ(exec_sql("SELECT * from Select_tables_4, Select_tables_5 where Select_tables_4.id=Select_tables_5.id;"),
      "Select_tables_4.id | Select_tables_4.age | Select_tables_4.u_name | Select_tables_5.id | Select_tables_5.res | "
      "Select_tables_5.u_name\n"
      "1 | 2 | a | 1 | 10 | g\n"
      "1 | 3 | b | 1 | 10 | g\n"
      "1 | 2 | a | 1 | 11 | f\n"
      "1 | 3 | b | 1 | 11 | f\n"
      "2 | 2 | c | 2 | 12 | c\n"
      "2 | 4 | d | 2 | 12 | c\n");
  ASSERT_EQ(exec_sql("select * from Select_tables_4, Select_tables_5 where Select_tables_4.id >= Select_tables_5.id;"),
      "Select_tables_4.id | Select_tables_4.age | Select_tables_4.u_name | Select_tables_5.id | Select_tables_5.res | "
      "Select_tables_5.u_name\n"
      "1 | 2 | a | 1 | 10 | g\n"
      "1 | 3 | b | 1 | 10 | g\n"
      "2 | 2 | c | 1 | 10 | g\n"
      "2 | 4 | d | 1 | 10 | g\n"
      "1 | 2 | a | 1 | 11 | f\n"
      "1 | 3 | b | 1 | 11 | f\n"
      "2 | 2 | c | 1 | 11 | f\n"
      "2 | 4 | d | 1 | 11 | f\n"
      "2 | 2 | c | 2 | 12 | c\n"
      "2 | 4 | d | 2 | 12 | c\n");
  ASSERT_EQ(exec_sql("CREATE TABLE Select_tables_6(id int, res int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("SELECT Select_tables_1.id,Select_tables_6.id from Select_tables_1, Select_tables_6 where "
                     "Select_tables_1.id=Select_tables_6.id;"),
      "Select_tables_1.id | Select_tables_6.id\n");
}

TEST_F(SQLTest, SelectTablesShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t, t2;"),
      "t.a | t.b | t2.b | t2.d\n"
      "1 | 1 | 100 | 200\n"
      "2 | 3 | 100 | 200\n"
      "1 | 1 | 300 | 500\n"
      "2 | 3 | 300 | 500\n");
  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (999, 888);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 666);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t, t2, t3;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "1 | 1 | 100 | 200 | 999 | 888\n"
      "2 | 3 | 100 | 200 | 999 | 888\n"
      "1 | 1 | 300 | 500 | 999 | 888\n"
      "2 | 3 | 300 | 500 | 999 | 888\n"
      "1 | 1 | 100 | 200 | 777 | 666\n"
      "2 | 3 | 100 | 200 | 777 | 666\n"
      "1 | 1 | 300 | 500 | 777 | 666\n"
      "2 | 3 | 300 | 500 | 777 | 666\n");
}

TEST_F(SQLTest, SelectTablesColumnsOrderShouldCorrect)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (999, 888);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 666);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select t3.o, t.a, t2.b from t, t2, t3;"),
      "t3.o | t.a | t2.b\n"
      "999 | 1 | 100\n"
      "999 | 2 | 100\n"
      "999 | 1 | 300\n"
      "999 | 2 | 300\n"
      "777 | 1 | 100\n"
      "777 | 2 | 100\n"
      "777 | 1 | 300\n"
      "777 | 2 | 300\n");
  ASSERT_EQ(exec_sql("select * from t2, t;"),
      "t2.b | t2.d | t.a | t.b\n"
      "100 | 200 | 1 | 1\n"
      "300 | 500 | 1 | 1\n"
      "100 | 200 | 2 | 3\n"
      "300 | 500 | 2 | 3\n");
}

TEST_F(SQLTest, SelectTablesSingleColumnShouldShowTableName)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select t.a from t, t2;"),
      "t.a\n"
      "1\n"
      "2\n"
      "1\n"
      "2\n");
}

TEST_F(SQLTest, SelectTablesBothStarAndColumnsShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select t.*, t2.b from t, t2;"),
      "t.a | t.b | t2.b\n"
      "1 | 1 | 100\n"
      "2 | 3 | 100\n"
      "1 | 1 | 300\n"
      "2 | 3 | 300\n");
}

TEST_F(SQLTest, SelectTablesWithConditionsShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (999, 888);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 666);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 0);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t, t2, t3 where t3.o <= 777 and t.a >= 2;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "2 | 3 | 100 | 200 | 777 | 666\n"
      "2 | 3 | 300 | 500 | 777 | 666\n"
      "2 | 3 | 100 | 200 | 777 | 0\n"
      "2 | 3 | 300 | 500 | 777 | 0\n");

  ASSERT_EQ(exec_sql("select * from t, t2, t3 where t3.o <= 777 and t.a >= 2 and "
                     "t.a < t3.a;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "2 | 3 | 100 | 200 | 777 | 666\n"
      "2 | 3 | 300 | 500 | 777 | 666\n");

  ASSERT_EQ(exec_sql("select * from t, t2, t3 where t.a < t2.b and t.a > t3.a;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "1 | 1 | 100 | 200 | 777 | 0\n"
      "2 | 3 | 100 | 200 | 777 | 0\n"
      "1 | 1 | 300 | 500 | 777 | 0\n"
      "2 | 3 | 300 | 500 | 777 | 0\n");
}

TEST_F(SQLTest, SelectTablesWithConditionsNotInProjectionShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (1, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (3, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select t.a, t.b from t, t2 where t.a < t2.b;"),
      "t.a | t.b\n"
      "1 | 1\n"
      "2 | 3\n");
}
TEST_F(SQLTest, SelectMutilTablesShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 11);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 22);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3, 33);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t2 values (10, 12);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (20, 22);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (100, 102);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (200, 202);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (300, 302);"), "SUCCESS\n");

  ASSERT_NE(exec_sql("select * from t2,t;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select  t2.*, t3.a from t2,t3;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select * from t2,t3 where t2.b>=t2.d;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select * from t2,t3 where t2.b > 10; "), "FAILURE\n");
  ASSERT_NE(exec_sql("select t2.b,t3.a from t3,t2; "), "FAILURE\n");
  ASSERT_EQ(exec_sql("select t2.b,t3.a from t3,t2 where t3.a != 100; "),
      "t2.b | t3.a\n"
      "10 | 102\n"
      "10 | 202\n"
      "10 | 302\n"
      "20 | 102\n"
      "20 | 202\n"
      "20 | 302\n");

  // ASSERT_NE(exec_sql("select *   from t2,t3 where a>b;"), "FAILURE\n");
  // ASSERT_NE(exec_sql("select *,a from t3,t2;"), "FAILURE\n");
}

//       ##  #######  #### ##    ##
//       ## ##     ##  ##  ###   ##
//       ## ##     ##  ##  ####  ##
//       ## ##     ##  ##  ## ## ##
// ##    ## ##     ##  ##  ##  ####
// ##    ## ##     ##  ##  ##   ###
//  ######   #######  #### ##    ##
// ########    ###    ########  ##       ########  ######
//    ##      ## ##   ##     ## ##       ##       ##    ##
//    ##     ##   ##  ##     ## ##       ##       ##
//    ##    ##     ## ########  ##       ######    ######
//    ##    ######### ##     ## ##       ##             ##
//    ##    ##     ## ##     ## ##       ##       ##    ##
//    ##    ##     ## ########  ######## ########  ######

TEST_F(SQLTest, JoinTablesShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (999, 888);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 666);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 0);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t, t2 inner join t3 on t3.o <= 777 and t.a >= 2;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "2 | 3 | 100 | 200 | 777 | 666\n"
      "2 | 3 | 300 | 500 | 777 | 666\n"
      "2 | 3 | 100 | 200 | 777 | 0\n"
      "2 | 3 | 300 | 500 | 777 | 0\n");

  ASSERT_EQ(exec_sql("select * from t "
                     "inner join t2 on 1 = 1 "
                     "inner join t3 on 1 = 1 "
                     "where t3.o <= 777 and t.a >= 2 and t.a < t3.a;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "2 | 3 | 100 | 200 | 777 | 666\n"
      "2 | 3 | 300 | 500 | 777 | 666\n");

  ASSERT_EQ(exec_sql("select * from t "
                     "inner join t2 on t.a < t2.b "
                     "inner join t3 on t.a > t3.a;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "1 | 1 | 100 | 200 | 777 | 0\n"
      "2 | 3 | 100 | 200 | 777 | 0\n"
      "1 | 1 | 300 | 500 | 777 | 0\n"
      "2 | 3 | 300 | 500 | 777 | 0\n");
}
TEST_F(SQLTest, JoinTablesShouldWork2)
{
  ASSERT_EQ(exec_sql("create table join_table_large_1 (id int,num1 int ); "), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_2 (id int,num2 int ); "), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_3 (id int,num3 int ); "), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_4 (id int,num4 int ); "), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_5 (id int,num5 int ); "), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_6 (id int,num6 int ); "), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_1 values(1,1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_2 values(1,1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_3 values(1,1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_4 values(1,1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_5 values(84,84),(85,85),(86,86);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_5 values(94,94);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_5 values(93,93);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_5 values(92,92);"), "SUCCESS\n");
  ASSERT_EQ(
      exec_sql("insert into join_table_large_5 values(95,95),(96,96),(97,97),(98,98),(99,99),(100,100);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_6 values(84,84),(85,85),(86,86);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_6 values(94,94);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_6 values(93,93);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into join_table_large_6 values(92,92);"), "SUCCESS\n");
  ASSERT_EQ(
      exec_sql("insert into join_table_large_6 values(95,95),(96,96),(97,97),(98,98),(99,99),(100,100);"), "SUCCESS\n");

  ASSERT_EQ(
      exec_sql("select * from join_table_large_1 inner join join_table_large_2 on "
               "join_table_large_1.id=join_table_large_2.id inner join join_table_large_3 on "
               "join_table_large_1.id=join_table_large_3.id inner join join_table_large_4 on "
               "join_table_large_3.id=join_table_large_4.id and join_table_large_4.num4 <= 5 inner join "
               "join_table_large_5 on 1=1 inner join join_table_large_6 on join_table_large_5.id=join_table_large_6.id "
               "where join_table_large_3.num3 <10 and join_table_large_5.num5>90;"),
      "join_table_large_1.id | join_table_large_1.num1 | join_table_large_2.id | join_table_large_2.num2 | "
      "join_table_large_3.id | join_table_large_3.num3 | join_table_large_4.id | join_table_large_4.num4 | "
      "join_table_large_5.id | join_table_large_5.num5 | join_table_large_6.id | join_table_large_6.num6\n1 | 1 | 1 | "
      "1 | 1 | 1 | 1 | 1 | 94 | 94 | 94 | 94\n1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 93 | 93 | 93 | 93\n1 | 1 | 1 | 1 | 1 | 1 "
      "| 1 | 1 | 92 | 92 | 92 | 92\n1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 95 | 95 | 95 | 95\n1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | "
      "96 | 96 | 96 | 96\n1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 97 | 97 | 97 | 97\n1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 98 | 98 | "
      "98 | 98\n1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 99 | 99 | 99 | 99\n1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 100 | 100 | 100 | "
      "100\n");
}
TEST_F(SQLTest, DISABLED_JoinTablesVeryLargeJoin)
{
  // create
  ASSERT_EQ(exec_sql("create table join_table_large_1(id int, num1 int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_2(id int, num2 int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_3(id int, num3 int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_4(id int, num4 int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_5(id int, num5 int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table join_table_large_6(id int, num6 int);"), "SUCCESS\n");
  // inserts
  std::string tables = "";
  for (size_t i = 1; i <= 100; i++) {
    std::string sql;
    std::string num = std::to_string(i);
    tables = tables + "(" + num + "," + num + "),";
    sql = "insert into join_table_large_1 values(" + num + "," + num + ");";
    // ASSERT_EQ(sql, "dd");
    ASSERT_EQ(exec_sql(sql), "SUCCESS\n");
    sql = "insert into join_table_large_2 values(" + num + "," + num + ");";
    ASSERT_EQ(exec_sql(sql), "SUCCESS\n");
    sql = "insert into join_table_large_3 values(" + num + "," + num + ");";
    ASSERT_EQ(exec_sql(sql), "SUCCESS\n");
    sql = "insert into join_table_large_4 values(" + num + "," + num + ");";
    ASSERT_EQ(exec_sql(sql), "SUCCESS\n");
    sql = "insert into join_table_large_5 values(" + num + "," + num + ");";
    ASSERT_EQ(exec_sql(sql), "SUCCESS\n");
    sql = "insert into join_table_large_6 values(" + num + "," + num + ");";
    ASSERT_EQ(exec_sql(sql), "SUCCESS\n");
  }
  ASSERT_EQ(tables, "ddd");
  ASSERT_EQ(exec_sql("select * from join_table_large_1 inner join join_table_large_2 on "
                     "join_table_large_1.id=join_table_large_2.id "
                     "inner join join_table_large_3 on join_table_large_1.id=join_table_large_3.id inner join "
                     "join_table_large_4 on "
                     "join_table_large_3.id=join_table_large_4.id and join_table_large_4.num4 <= 5 inner join "
                     "join_table_large_5 on "
                     "1=1 inner join join_table_large_6 on join_table_large_5.id=join_table_large_6.id where "
                     "join_table_large_3.num3 "
                     "<10 and join_table_large_5.num5>90;"),
      "SUCCESS\n");
}
// ##    ## ##     ## ##       ##
// ###   ## ##     ## ##       ##
// ####  ## ##     ## ##       ##
// ## ## ## ##     ## ##       ##
// ##  #### ##     ## ##       ##
// ##   ### ##     ## ##       ##
// ##    ##  #######  ######## ########

TEST_F(SQLTest, NullCreateTableShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a float not null, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
      "t(\n"
      "\tfield name=__trx, type=ints, len=4, visible=no, nullable=no\n"
      "\tfield name=__null, type=ints, len=4, visible=no, nullable=no\n"
      "\tfield name=a, type=floats, len=4, visible=yes, nullable=no\n"
      "\tfield name=b, type=ints, len=4, visible=yes, nullable=yes\n"
      ")\n");
}

TEST_F(SQLTest, NullInsertShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | NULL\n"
      "1 | 1\n");
}

TEST_F(SQLTest, NullInsertNullOnNotNullableShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null, null);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("insert into t values (null, 1);"), "FAILURE\n");
}

TEST_F(SQLTest, NullUpdateShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3, 1);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = null where a = 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("update t set a = 200 where b is null;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set b = null where a = 1;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set a = 300 where b is not null;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set a = 0 where b = null;"), "SUCCESS\n");  // no effect, b = null is always false

  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | NULL\n"
      "200 | NULL\n"
      "300 | 1\n");
}

TEST_F(SQLTest, NullInsertWithIndexShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_b on t(b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
      "a | b\n"
      "1 | NULL\n"
      "1 | 1\n");
}

TEST_F(SQLTest, NullCompareWithNullShouldAlwaysFalse)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t where a = null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where b = null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where b < null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where b > null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where null = null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where 1 = null;"), "a | b\n");
}

TEST_F(SQLTest, NullIsNullShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where b is null;"), "a | b\n1 | NULL\n");
  ASSERT_EQ(exec_sql("select * from t where b is not null;"), "a | b\n1 | 1\n");
  ASSERT_EQ(exec_sql("select * from t where 1 is null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where 1 is not null;"),
      "a | b\n"
      "1 | NULL\n"
      "1 | 1\n");
}

TEST_F(SQLTest, DISABLED_NullAggCountNullableShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select count(*) from t;"), "count(*)\n2\n");
  ASSERT_EQ(exec_sql("select count(a) from t;"), "count(a)\n1\n");
}

TEST_F(SQLTest, DISABLED_NullAggMaxOfNullableShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select max(a) from t;"), "max(a)\nNULL\n");

  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select max(a) from t;"), "max(a)\n2\n");
}

TEST_F(SQLTest, DISABLED_NullAggMinOfNullableShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select min(a) from t;"), "min(a)\nNULL\n");

  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select min(a) from t;"), "min(a)\n1\n");
}

TEST_F(SQLTest, DISABLED_NullAggAvgOfNullableShouldWork)
{
  ASSERT_EQ(exec_sql("create table t(a int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select avg(a) from t;"), "avg(a)\nNULL\n");

  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select avg(a) from t;"), "avg(a)\n1.5\n");
}

int main(int argc, char **argv)
{
  srand((unsigned)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  conf_path = fs::absolute(conf_path);
  observer_binary = fs::absolute(observer_binary);
  return RUN_ALL_TESTS();
}
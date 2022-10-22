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
    std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
  }
  void stop()
  {
    server->shutdown();
    std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
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
    fs::path socket_path = data_dir / ".sock";

    fs::create_directory(data_dir);
    fs::current_path(data_dir);
    server->start(data_dir, socket_path);

    sockfd = init_unix_sock(socket_path.c_str());
  }

  void TearDown() override
  {
    server->stop();
    fs::remove_all(data_dir);
  }

  std::string exec_sql(std::string sql)
  {
    char send_bytes;
    if ((send_bytes = write(sockfd, sql.c_str(), sql.length() + 1)) == -1) {
      fprintf(stderr, "send error: %d:%s \n", errno, strerror(errno));
      return "FAILURE";
    }

    memset(recv_buf, 0, MAX_MEM_BUFFER_SIZE);
    int len = 0;
    std::string resp;
    while ((len = recv(sockfd, recv_buf, MAX_MEM_BUFFER_SIZE, 0)) > 0) {
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
  TestServer *server;

  int sockfd;
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

TEST_F(SQLTest, DISABLED_SelectMetaSelectValidConditionInMultiTablesShouldSuccess)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");
  ASSERT_NE(exec_sql("select * from t, t2 where a > b;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select * from t, t2 where t.a > t2.b;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select * from t, t2 where a > 1 and b > 1 and t.a > t2.b;"), "FAILURE\n");
}

TEST_F(SQLTest, DISABLED_SelectMetaSelectInvalidConditionInMultiTablesShouldFailure)
{
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where a > c;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where t.a > t3.b;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where a > 1 and b > 1 and t.a > t3.b;"), "FAILURE\n");

  ASSERT_EQ(exec_sql("create table t3(c char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t, t3 where a > c;"), "FAILURE\n");
}

TEST_F(SQLTest, DISABLED_SelectMetaSelectIndeterminableConditionInMultiTablesShouldFailure)
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

TEST_F(SQLTest, DISABLED_DateCanCreateTable)
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

TEST_F(SQLTest, DISABLED_UniqueIndexMetaShouldWork)
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
      "\tindex name=t_a, fields=a, unique=yes\n"
      "\tindex name=t_b, fields=b, unique=no\n"
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

int main(int argc, char **argv)
{
  srand((unsigned)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  conf_path = fs::absolute(conf_path);
  observer_binary = fs::absolute(observer_binary);
  return RUN_ALL_TESTS();
}
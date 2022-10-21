#include "sql/operator/decarts_join_operator.h"
RC DecartsJoinOperator::open()
{
  for (auto child : children_) {
    child->open();
  }
  current_index = -1;
}

RC DecartsJoinOperator::next()
{

  for (auto child : children_) {
    RC rc = get_child_tuples();
  }
  hasjoined = true;
  current_index++;
  return rc;
}

RC DecartsJoinOperator::close()
{
  for (auto child : children_) {
    child->close();
  }
  return RC::SUCCESS;
}

Tuple *DecartsJoinOperator::current_tuple()
{
  return tuples[current_index];
}

RC DecartsJoinOperator::get_one_tuples()
{
  std::vector<Tuple *> child_tuples;
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = oper->current_tuple();
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get tuple from operator");
      break;
    }
    child_tuples.push_back(tuple);
  }
  // 如果当前为tuple.size()==0
  rc = RC::SUCCESS;
  decartes(tuples, child_tuples);
  children_tuples.push_back(child_tuples);  // for debug
  return rc;
}

void DecartsJoinOperator::decartes(std::vector<ComplexTuple> &current_tuples, std::vector<Tuple *> &new_tuples)
{
  if (new_tuples.size() == 0) {
    return;
  }
  if (current_tuples.size() == 0) {
    for (auto new_tuple : new_tuples) {
      RowTuple *new_row_tuple = static_cast<RowTuple &>(*new_tuple);
      ComplexTuple complex_tuple(new_row_tuple);
      current_tuples.push_back(complex_tuple);
    }
  }
  for (auto cur_tuple : current_tuples) {
    for (auto new_tuple : new_tuples) {
      // decarts 的child 是tablescan 返回的tuple 类型是rowTuple
      RowTuple *new_row_tuple = static_cast<RowTuple &>(*new_tuple);
      cur_tuple.add_row_tuple(new_row_tuple);
    }
  }
}

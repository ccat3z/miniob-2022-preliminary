#include "sql/operator/decarts_join_operator.h"
#include "common/log/log.h"

RC DecartsJoinOperator::open()
{
  for (auto child : children_) {
    child->open();
  }
  current_index = -1;
  return RC::SUCCESS;
}

RC DecartsJoinOperator::next()
{
  RC rc = RC::SUCCESS;
  if (!hasjoined) {
    for (auto child : children_) {
      rc = get_child_tuples(child);
    }
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
  if (current_index >= (int)current_tuples.size()) {
    return nullptr;
  }
  ComplexTuple *tuple = current_tuples[current_index];
  LOG_DEBUG("DecartsJoinOperator::current_tuple() tuple.cell_num():%d", tuple->cell_num());
  return tuple;
}

RC DecartsJoinOperator::get_child_tuples(Operator *child)
{
  RC rc = RC::SUCCESS;
  while (RC::SUCCESS == (rc = child->next())) {
    // 每次调用current_tuple 它返回的tuple的地址是同一个。
    RowTuple *tuple = dynamic_cast<RowTuple *>(child->current_tuple());
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get tuple from operator");
      break;
    }
    decartes_one(tuple);  //    计算tuple和current_tuples 的笛卡尔积结果，结果存放在tuples中
  }
  // 如果当前为tuple.size()==0
  rc = RC::SUCCESS;
  // 将tuples替换current_tuple,然后清空tuples
  for (unsigned i = 0; i < current_tuples.size(); i++) {
    delete current_tuples[i];
  }
  current_tuples.clear();
  LOG_DEBUG("DecartsJoinOperator::get_child_tuples: after decarts oper print tuples");
  for (unsigned i = 0; i < tuples.size(); i++) {
    tuples[i]->print();
    current_tuples.push_back(tuples[i]);
  }
  tuples.clear();

  return rc;
}
void DecartsJoinOperator::decartes_one(RowTuple *tuple)
{
  if (tuple == nullptr)
    return;
  if (current_tuples.size() == 0) {
    ComplexTuple *new_tuple = new ComplexTuple(tuple);
    tuples.push_back(new_tuple);
    return;
  }
  for (unsigned i = 0; i < current_tuples.size(); i++) {
    ComplexTuple *new_tuple = new ComplexTuple(current_tuples[i]);
    new_tuple->add_row_tuple(tuple);
    tuples.push_back(new_tuple);
  }
}
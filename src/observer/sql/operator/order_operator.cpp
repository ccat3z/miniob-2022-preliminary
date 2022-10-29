#include "sql/operator/order_operator.h"
#include "common/log/log.h"

RC OrderOperator::open()
{
  children_[0]->open();
  current_index = -1;
  return RC::SUCCESS;
}

RC OrderOperator::next()
{
  RC rc = RC::SUCCESS;
  if (!hasordered) {
    rc = order_child_tuples(children_[0]);
  }
  hasordered = true;
  current_index++;
  if (current_index >= (int)current_tuples_.size()) {
    return RC::RECORD_EOF;
  }
  return rc;
}

RC OrderOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple *OrderOperator::current_tuple()
{
  if (current_index >= (int)current_tuples_.size()) {
    return nullptr;
  }
  ComplexTuple *tuple = current_tuples_[current_index];
  LOG_DEBUG("OrderOperator::current_tuple() tuple.cell_num():%d", tuple->cell_num());
  return tuple;
}

RC OrderOperator::order_child_tuples(Operator *child)
{
  RC rc = RC::SUCCESS;
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get tuple from operator");
      break;
    }
    insert_one(tuple);
  }
  return RC::SUCCESS;
}
void OrderOperator::insert_one(Tuple *tuple)
{
  ComplexTuple *new_tuple = new ComplexTuple(tuple);
  for (size_t i = 0; i < current_tuples_.size(); i++) {
    ComplexTuple *tuple = current_tuples_[i];
    if (compare(tuple, new_tuple) >= 0) {  // <0 new_tuple在tuple前面，> 0 new_tuple在tuple后面
      current_tuples_.insert(current_tuples_.begin() + i, new_tuple);
      return;
    }
  }
  current_tuples_.push_back(new_tuple);
  new_tuple->print();
}
int OrderOperator::compare(ComplexTuple *tuple1, ComplexTuple *tuple2)
{
  //根据feilds  判断两个tuple之间的大小
  //先按照第一个排序，如果第一个一样，则按照第二个排序依次比较大小
  //
  for (size_t i = 0; i < order_stmts_.size(); i++) {
    std::string relation_name = "";
    if (order_stmts_[i].expr.value.attr.relation_name) {
      relation_name = order_stmts_[i].expr.value.attr.relation_name;
    }
    std::string field_name = order_stmts_[i].expr.value.attr.attribute_name;
    // 找到对应的field
    int index = 0;
    for (; index < tuple1->cell_num(); index++) {
      const TupleCellSpec *spec;
      tuple1->cell_spec_at(index, spec);
      FieldExpr *field_expr = (FieldExpr *)spec->expression();
      if (relation_name.compare(field_expr->table_name()) != 0 && relation_name != "") {
        continue;
      }
      if (field_name.compare(field_expr->field_name()) != 0) {
        continue;
      }
      break;
    }
    if (index == tuple1->cell_num())
      return 0;
    TupleCell tuple_cell1;
    TupleCell tuple_cell2;
    tuple1->cell_at(index, tuple_cell1);
    tuple2->cell_at(index, tuple_cell2);
    if (!tuple_cell1.compare(tuple_cell2))
      continue;
    bool bigger = tuple_cell1.compare(tuple_cell2) > 0;  // tuple2 比 tuple1 小
    if ((bigger && order_stmts_[i].asc) || (!bigger && !order_stmts_[i].asc))
      return 1;  // tuple1 放在 tuple2 后面。
    else
      return -1;
  }
  return 0;
}
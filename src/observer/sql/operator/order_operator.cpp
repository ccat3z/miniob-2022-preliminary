#include "sql/operator/order_operator.h"
#include "common/log/log.h"
#include <stdexcept>

RC OrderOperator::open()
{
  children_[0]->open();
  current_index = -1;

  if (order_exprs_.empty()) {
    RC rc = RC::SUCCESS;
    for (auto &stmt : order_stmts_) {
      order_exprs_.emplace_back();
      rc = create_expression(order_exprs_.back(), stmt.expr);

      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed create expr for order key");
        return rc;
      }
    }
  }

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
  for (size_t i = 0; i < order_stmts_.size(); i++) {
    TupleCell tuple_cell1;
    TupleCell tuple_cell2;

    if (RC::SUCCESS != order_exprs_[i]->get_value(*tuple1, tuple_cell1)) {
      throw std::invalid_argument("Failed to eval order key");
    }

    if (RC::SUCCESS != order_exprs_[i]->get_value(*tuple2, tuple_cell2)) {
      throw std::invalid_argument("Failed to eval order key");
    }

    bool bigger = false;  // tuple cell 1 is bigger then tuple cell 2
    if (tuple_cell1.is_null() && tuple_cell2.is_null()) {
      continue;
    } else if (tuple_cell1.is_null() && !tuple_cell2.is_null()) {
      bigger = false;
    } else if (!tuple_cell1.is_null() && tuple_cell2.is_null()) {
      bigger = true;
    } else {
      auto res = tuple_cell1.compare(tuple_cell2);
      if (res == 0)
        continue;
      bigger = tuple_cell1.compare(tuple_cell2) > 0;
    }

    if ((bigger && order_stmts_[i].asc) || (!bigger && !order_stmts_[i].asc))
      return 1;  // tuple1 放在 tuple2 后面。
    else
      return -1;
  }
  return 0;
}
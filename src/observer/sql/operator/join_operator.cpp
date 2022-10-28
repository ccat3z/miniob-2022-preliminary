#include "sql/operator/join_operator.h"
#include "common/log/log.h"

RC JoinOperator::open()
{
  left_->open();
  right_->open();
  return RC::SUCCESS;
}

RC JoinOperator::next()
{
  RC rc = RC::SUCCESS;
  if (begin_join == false) {
    rc = left_->next();
    begin_join = true;
  }
  while (rc == RC::SUCCESS) {  // 当 left->next() !=RC::SUCCESS 则应该返回RC::E
    left_tuple_ = left_->current_tuple();
    if (nullptr == left_tuple_) {  // left 错误的情况
      rc = RC::INTERNAL;
      LOG_WARN("RC JoinOperator::next():failed to get left tuple from operator");
      break;
    }
    if (RC::SUCCESS == (rc = right_->next())) {
      current_tuple_ = new ComplexTuple();
      current_tuple_.add_tuple(left_tuple_);
      LOG_DEBUG("join left_tule");
      current_tuple_.print();
      Tuple *right_tuple = right_->current_tuple();
      if (nullptr == right_tuple) {  // right 错误的情况
        rc = RC::INTERNAL;
        LOG_WARN("RC JoinOperator::next():failed to get right tuple from operator");
        break;
      }
      current_tuple_.add_tuple(right_tuple);
      LOG_DEBUG("join: add right tuple---");
      LOG_DEBUG("join:current_tuple():current_tuple->cell_num():%d", current_tuple_.cell_num());
      current_tuple_.print();
      return rc;  // 成功next
    }
    if (rc == RC::RECORD_EOF) {  // 说明right 的表遍历到底了,left进行下一个，right则重新开始
      right_->close();
      right_->open();
      rc = left_->next();
    }
  }
  return rc;
}

RC JoinOperator::close()
{
  right_->close();
  left_->close();
  return RC::SUCCESS;
}

Tuple *JoinOperator::current_tuple()
{
  LOG_DEBUG("Tuple *JoinOperator::current_tuple()");
  current_tuple_.print();
  return &current_tuple_;
}

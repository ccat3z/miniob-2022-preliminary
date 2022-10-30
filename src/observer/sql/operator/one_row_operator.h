
#include "sql/operator/operator.h"

class OneRowOperator : public Operator {
public:
  virtual RC open() override
  {
    return RC::SUCCESS;
  }

  virtual RC next() override
  {
    if (readed)
      return RC::RECORD_EOF;

    readed = true;
    return RC::SUCCESS;
  }

  virtual RC close() override
  {
    readed = false;
    return RC::SUCCESS;
  }

  Tuple *current_tuple() override
  {
    return &tuple;
  }

  MemoryTuple tuple;
  bool readed = false;
};
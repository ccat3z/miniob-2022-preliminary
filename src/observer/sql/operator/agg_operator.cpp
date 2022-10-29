#include "agg_operator.h"
#include "common/log/log.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/tuple_cell.h"
#include "sql/parser/parse_defs.h"
#include <cstring>
#include <exception>
#include <memory>
#include <strings.h>
#include <vector>

class CountAggregator : public Aggregator {
public:
  CountAggregator(std::shared_ptr<AggFuncExpr> expr) : Aggregator(expr){};
  RC get_cell(TupleCell &cell) override
  {
    cell.save(cnt);
    return RC::SUCCESS;
  }

  RC add_tuple(const Tuple &) override
  {
    cnt++;
    return RC::SUCCESS;
  };

private:
  int cnt = 0;
};

class MaxAggregator : public Aggregator {
public:
  MaxAggregator(std::shared_ptr<AggFuncExpr> expr) : Aggregator(expr)
  {
    cell.set_null(true);
  };

  RC get_cell(TupleCell &cell) override
  {
    cell = this->cell;
    return RC::SUCCESS;
  }

  RC add_tuple(const Tuple &tuple) override
  {
    RC rc = RC::SUCCESS;

    TupleCell number;
    rc = get_arg(tuple, number);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to get arg");
      return rc;
    }

    if (cell.is_null()) {
      cell = number;
    } else if (cell.compare(number) < 0) {
      cell = number;
    }

    return rc;
  };

private:
  TupleCell cell;
};

class MinAggregator : public Aggregator {
public:
  MinAggregator(std::shared_ptr<AggFuncExpr> expr) : Aggregator(expr)
  {
    cell.set_null(true);
  };

  RC get_cell(TupleCell &cell) override
  {
    cell = this->cell;
    return RC::SUCCESS;
  }

  RC add_tuple(const Tuple &tuple) override
  {
    RC rc = RC::SUCCESS;

    TupleCell number;
    rc = get_arg(tuple, number);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to get arg");
      return rc;
    }

    if (cell.is_null()) {
      cell = number;
    } else if (cell.compare(number) > 0) {
      cell = number;
    }

    return rc;
  };

private:
  TupleCell cell;
};

class AvgAggregator : public Aggregator {
public:
  AvgAggregator(std::shared_ptr<AggFuncExpr> expr) : Aggregator(expr){};

  RC get_cell(TupleCell &cell) override
  {
    cell.save(total / (float)size);
    return RC::SUCCESS;
  }

  RC add_tuple(const Tuple &tuple) override
  {
    RC rc = RC::SUCCESS;

    TupleCell number;
    rc = get_arg(tuple, number);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to get arg");
      return rc;
    }

    if (!number.is_null()) {
      size++;
      total += number.as_float();
    }

    return rc;
  };

private:
  float total = 0;
  int size = 0;
};

class SumAggregator : public Aggregator {
public:
  SumAggregator(std::shared_ptr<AggFuncExpr> expr) : Aggregator(expr){};

  RC get_cell(TupleCell &cell) override
  {
    cell.save((float)total);
    return RC::SUCCESS;
  }

  RC add_tuple(const Tuple &tuple) override
  {
    RC rc = RC::SUCCESS;

    TupleCell number;
    rc = get_arg(tuple, number);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to get arg");
      return rc;
    }

    if (!number.is_null()) {
      total += number.as_float();
    }

    return rc;
  };

private:
  float total = 0;
};

RC Aggregator::get_arg(const Tuple &tuple, TupleCell &cell)
{
  RC rc = RC::SUCCESS;
  if (expr->args.size() != 1) {
    LOG_ERROR("Aggregator %s requires one args", expr->toString(true).c_str());
    return RC::INVALID_ARGUMENT;
  }

  auto &arg = expr->args[0];

  rc = arg->get_value(tuple, cell);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Cannot get value from arg expr: %s", arg->toString(true).c_str());
    return rc;
  }

  return rc;
}

RC AggOperator::open()
{
  return children_[0]->open();
}

RC AggOperator::close()
{
  RC rc = RC::SUCCESS;
  rc = children_[0]->close();

  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to close children");
    return rc;
  }

  aggregators.clear();
  keys.clear();
  tuple.reset();
  reduced = false;
  idx = -1;

  return rc;
}

RC AggOperator::reduce()
{
  RC rc = RC::SUCCESS;
  while (RC::SUCCESS == (rc = children_[0]->next())) {
    auto tuple = children_[0]->current_tuple();

    // Build group key
    MemoryTuple key;
    for (auto &key_expr : key_exprs) {
      TupleCell cell;
      rc = key_expr->get_value(*tuple, cell);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to get key: %s", key_expr->toString(true).c_str());
        return rc;
      }

      key.append_cell(cell, TupleCellSpec(key_expr));
    }

    auto &aggs = aggregators[key];
    if (aggs.empty()) {
      for (auto &expr : exprs) {
        auto func = expr->agg_func().c_str();
        if (strcasecmp(func, "count") == 0) {
          aggs.emplace_back(std::make_unique<CountAggregator>(expr));
        } else if (strcasecmp(func, "max") == 0) {
          aggs.emplace_back(std::make_unique<MaxAggregator>(expr));
        } else if (strcasecmp(func, "min") == 0) {
          aggs.emplace_back(std::make_unique<MinAggregator>(expr));
        } else if (strcasecmp(func, "avg") == 0) {
          aggs.emplace_back(std::make_unique<AvgAggregator>(expr));
        } else if (strcasecmp(func, "sum") == 0) {
          aggs.emplace_back(std::make_unique<SumAggregator>(expr));
        } else {
          LOG_ERROR("Unsupport agg func: %s", func);
          return RC::GENERIC_ERROR;
        }
      }
      keys.emplace_back(key);
    }

    for (auto &agg : aggs) {
      rc = agg->add_tuple(*tuple);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to add value to aggreator");
        return rc;
      }
    }
  }

  if (rc == RC::RECORD_EOF) {
    return RC::SUCCESS;
  }

  return rc;
}

RC AggOperator::next()
{
  RC rc = RC::SUCCESS;
  if (!reduced) {
    rc = reduce();
    reduced = true;
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to reduce");
      return rc;
    }
  }

  if (++idx >= (int)keys.size()) {
    return RC::RECORD_EOF;
  }
  auto &key = keys[idx];

  tuple = std::make_shared<MemoryTuple>();
  rc = tuple->append_tuple(keys[idx]);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed append tuple into tuple");
    return rc;
  }

  auto &aggs = aggregators[key];
  for (auto &agg : aggs) {
    TupleCell cell;
    rc = agg->get_cell(cell);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to get cell from aggregator");
      return rc;
    }
    tuple->append_cell(cell, agg->get_spec());
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to append cell into tuple");
      return rc;
    }
  }

  return RC::SUCCESS;
}

RC AggOperator::add_agg_expr(const FuncExpr &expr)
{
  if (expr.arg_num != 1) {
    LOG_ERROR("Aggregator %s requires one args", expr.name);
    return RC::INVALID_ARGUMENT;
  }

  try {
    exprs.emplace_back(std::make_shared<AggFuncExpr>(expr));
  } catch (const std::exception &e) {
    LOG_ERROR("Failed to add agg expr to agg operator: %s", e.what());
    return RC::INTERNAL;
  }

  return RC::SUCCESS;
}

RC AggOperator::add_groups(const std::vector<UnionExpr> &exprs)
{
  RC rc = RC::SUCCESS;
  for (auto &expr : exprs) {
    std::unique_ptr<Expression> expression;
    rc = create_expression(expression, expr);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to create expression for group expr");
      return rc;
    }
    key_exprs.emplace_back(expression.release());
  }

  return rc;
}
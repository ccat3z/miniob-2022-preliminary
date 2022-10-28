#include "agg_operator.h"
#include "common/log/log.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/tuple_cell.h"
#include <cstring>
#include <exception>
#include <memory>
#include <strings.h>

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

    auto &aggs = aggregators[key];
    if (aggs.empty()) {
      for (auto &expr : exprs) {
        auto func = expr->agg_func().c_str();
        if (strcasecmp(func, "count") == 0) {
          aggs.emplace_back(std::make_unique<CountAggregator>(expr));
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
  if (reduced) {
    return RC::RECORD_EOF;
  }

  RC rc = RC::SUCCESS;
  rc = reduce();
  reduced = true;
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to reduce");
    return rc;
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
  try {
    exprs.emplace_back(std::make_shared<AggFuncExpr>(expr));
  } catch (const std::exception &e) {
    LOG_ERROR("Failed to add agg expr to agg operator: %s", e.what());
    return RC::INTERNAL;
  }

  return RC::SUCCESS;
}
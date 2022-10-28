#pragma once

#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/tuple_cell.h"
#include "sql/operator/operator.h"
#include "sql/parser/parse_defs.h"

#include <memory>
#include <unordered_map>
#include <vector>

class Aggregator {
public:
  Aggregator(std::shared_ptr<AggFuncExpr> expr) : expr(expr), spec(expr)
  {}
  virtual ~Aggregator() = default;

  virtual RC add_tuple(const Tuple &tuple) = 0;
  virtual RC get_cell(TupleCell &cell) = 0;
  const TupleCellSpec &get_spec() const
  {
    return spec;
  }

protected:
  std::shared_ptr<AggFuncExpr> expr;
  RC get_arg(const Tuple &tuple, TupleCell &cell);

private:
  TupleCellSpec spec;
};

class AggOperator : public Operator {
public:
  RC open() override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override
  {
    return tuple.get();
  }

  RC add_agg_expr(const FuncExpr &expr);

private:
  std::vector<std::shared_ptr<AggFuncExpr>> exprs;

  RC reduce();
  bool reduced = false;

  std::map<MemoryTuple, std::vector<std::unique_ptr<Aggregator>>> aggregators;
  std::vector<MemoryTuple> keys;
  std::shared_ptr<MemoryTuple> tuple;

  int idx = -1;
};
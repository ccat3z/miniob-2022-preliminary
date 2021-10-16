# 介绍

miniob 是 OceanBase 与华中科技大学联合开发的、面向"零"基础同学的数据库入门实践工具。
miniob 设计的目标是让同学们快速了解数据库并深入学习数据库内核，期望通过相关训练之后，能够对数据库内核各个模块的功能及其关联有所了解，并能够在
使用数据库时，设计出高效的 SQL 。miniob 面向的对象主要是在校学生，并且诸多模块都做了简化，比如不考虑并发操作。

(注意：此代码仅供学习使用，不考虑任何安全特性。)

[GitHub 首页](https://github.com/oceanbase/miniob)

## TODO

- [ ] drop table (drop-table)
- [ ] 实现 update 功能 (update)
- [ ] 隐式类型转换 (typecast)
- [ ] 查看索引 (show-index)
- [ ] 增加 date 字段 (date)
- [ ] 查询元数据校验 (select-meta)
- [ ] 字符串匹配 (like)
- [ ] 多表查询 (select-tables)
- [ ] 聚合运算 (aggregation-func)
- [ ] 多表 join 操作 (join-tables)
- [ ] 一次插入多条数据 (insert)
- [ ] 复杂更新 (update-select)
- [ ] 唯一索引 (unique)
- [ ] 支持 NULL 类型 (null)
- [ ] 简单子查询 (simple-sub-query)
- [ ] 别名 (alias)
- [ ] 函数 (function)
- [ ] 超长字段 (text)
- [ ] 查询条件支持表达式 (expression)
- [ ] 复杂子查询 (complex-sub-query)
- [ ] 排序 (order-by)
- [ ] 分组 (group-by)
- [ ] redo log (clog)

# 1. 题目说明

[miniob 题目描述](docs/miniob_topics.md)

# 2. 开发指南

## 搭建开发环境

1. [本地配置 gcc 环境](docs/how_to_build.md)。
2. [使用 Docker 开发](docs/how-to-dev-using-docker.md)。
3. [在 Windows 上使用 Docker](docs/how_to_dev_miniob_by_docker_on_windows.md)。

## 词法、语法解析

请参考 [miniob 词法语法解析开发与测试](docs/miniob-sql-parser.md)。

# 3. 提交测试

题目完成并通过自测后，大家可以在 [miniob 训练营](https://open.oceanbase.com/train?questionId=200001) 上提交代码进行测试。

客户端输出需要满足一定要求，如果你的测试结果不符合预期，请参考 [miniob 输出约定](docs/miniob-output-convention.md)。

# 4. 数据库管理系统实现基础讲义

由华中科技大学谢美意和左琼老师联合编撰的数据库管理系统实现教材：[《数据库管理系统实现基础讲义》](docs/lectures/index.md)

# 5. miniob 介绍

[miniob 源码解析视频](https://open.oceanbase.com/activities/4921877)

[miniob 源码解析文档](https://www.oceanbase.com/docs/community-developer-quickstart-10000000000627363)

（资料持续整理中，请大家自行查阅标题为“MiniOB...”的视频或文档）

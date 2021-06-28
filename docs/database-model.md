# 数据库模型说明

表数量：8 张

## 商品信息 (goods)

| 字段名 | 字段描述 | 字段类型 | 完整性限制/说明 |
|-------|--------|---------|-----------|
| gid | 商品 ID | int | 主键，自增 |
| gno | 商品编号 | char(20) | |
| cid | 商品分类 ID | int | 外键 |
| uid | 商品创建人 ID | int | 外键 |
| sku | 规格（最小库存单位）| nchar(100) | |
| title | 商品名称 | nchar(50) | |
| subtitle | 商品子名称 / 描述 | nchar(200) | |
| inventory | 商品库存量 | int | 默认值 0，使用触发器更新 |
| restock_value | 商品进价 | float | |
| selling_value | 商品售价 | float | |
| length | 商品长度 | float | |
| width | 商品宽度 | float | |
| height | 商品高度 | float | |
| weight | 商品重量 | float | |
| color | 商品颜色 | nchar(20) | |

## 商品分类 (categories)

| 字段名 | 字段描述 | 字段类型 | 完整性限制/说明 |
|-------|--------|---------|-----------|
| cid | 分类 ID | int | 主键，自增 |
| name | 分类名称 | nchar(100) | |
| number | 当前分类商品数量 | int | 使用触发器更新 |

## 商品图片 (pictures)

| 字段名 | 字段描述 | 字段类型 | 完整性限制/说明 |
|-------|--------|---------|-----------|
| pid | 图片 ID |  int | 主键，自增 |
| gid | 商品 ID | int | 外键 |
| picture | 图片内容 | text | Base64 编码后的图片，或者路径形式保存.. |
| description | 图片描述 | nchar(100) | |

## 用户信息 (users)

| 字段名 | 字段描述 | 字段类型 | 完整性限制/说明 |
|-------|--------|---------|---------------|
| uid | 用户 ID | int | 主键，自增 |
| username | 用户名 | nchar(50) | |
| password | 密码 | char(32) | 使用 MD5 形式保存 |
| role | 角色 | int | 0 = 系统管理员，1 = 经理 / 人事，2 = 普通用户... |
| phone | 电话号码 | char(20) | |
| email | 邮箱 | nchar(100) | |
| last_login | 上次登录时间 | datetime | |

## 商品卖出信息表 (goods_sellouts)

| 字段名 | 字段描述 | 字段类型 | 完整性限制/说明 |
|-------|--------|---------|---------------|
| sid | 售货编号 | int | 主键，自增 |
| gid | 商品 ID | int | 外键 |
| uid | 操作用户 ID | int | 外键 |
| number | 卖出数量 | float | |
| price | 卖出价格  | float | |
| customer_name | 顾客姓名 | nchar(20) | |
| customer_contact | 顾客联系方式 | nchar(20) | |
| sell_time | 卖出时间 | datetime | |


## 商品变动信息表 (goods_logs) 

保存商品的进货/上架/下架等信息。

| 字段名 | 字段描述 | 字段类型 | 完整性限制/说明 |
|-------|--------|---------|---------------|
| log_id | 日志 ID | int | 主键，自增 |
| gid | 商品 ID | int | 外键 |
| uid | 操作用户 ID | int | 外键 |
| type | 操作类型 | int | 1=进货，2=上架, 3=下架 |
| number | 操作数量 | float | |
| time | 操作日期 | datetime | |

## 用户权限表 (user_permissions)

| 字段名 | 字段描述 | 字段类型 | 完整性限制/说明 |
|-------|--------|---------|---------------|
| pid | 权限信息 ID  | int | 主键，自增 |
| uid | 用户 ID | int | 外键 |
| type | 权限类型 | char(50) | |
| domain | 权限域 | char(50) | |

## 商品折扣策略 (discounts)

| 字段名 | 字段描述 | 字段类型 | 完整性限制/说明 |
|-------|--------|---------|---------------|
| did | 折扣信息 ID | int | 主键，自增 |
| gid | 商品 ID | int | 外键 |
| uid | 用户 ID | int | 外键 |
| start_time | 开始日期 | datetime |  |
| end_time | 结束日期 | endtime | |
| discount | 折扣价格 | float | |
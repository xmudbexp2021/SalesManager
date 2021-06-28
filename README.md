# 商品销售管理系统

这是厦门大学 2018 级信息学院《数据库系统原理》课程期末项目，基于 C/S 模型编写的商品销售管理系统 (SalesManager).

## 技术栈

- 编程语言：C++ (c++14 标准)
- GUI 框架：Qt (5.14)
- 数据库：MySQL (8.0)

## 数据库初始化

将 [database.sql](https://github.com/xmudbexp2021/SalesManager/blob/main/docs/database.sql) 导入数据库即可。默认管理员用户名为 `admin`，密码为 `admin`。

另请参阅 [《数据库表结构和设计说明》](https://github.com/xmudbexp2021/SalesManager/blob/main/docs/database-model.md)。

## 开发环境搭建

请参考 [《项目部署说明 - 搭建开发环境》](https://s6npaj5yw4.feishu.cn/docs/doccnxjRj9k5WgWTQAzT5udhNRe)

## 打包程序

1. 在 Qt Creator 中，以 Release 模式编译一次程序。
2. 在 SalesManager.exe 所在目录下，执行 `windeployqt SalesManager.exe`.
3. 在 SalesManager.exe 所在目录下，创建 `plugins` 文件夹，并将除了 `plugins` 文件夹外的其它文件夹移动到 `plugins` 中。
4. 将 [《项目部署说明 - 搭建开发环境》](https://s6npaj5yw4.feishu.cn/docs/doccnxjRj9k5WgWTQAzT5udhNRe) 下载的 `libmariadb.dll` 和 `libmariadb.lib` 复制到与 `SalesManager.exe` 同级的目录下。

## 截图

登录界面：

![image.png](https://i.loli.net/2021/06/28/PuywOcMBCGUDSkx.png)

程序主界面：

![image.png](https://i.loli.net/2021/06/28/qtoJm3Lf7ObZiVe.png)

子页面：

![image.png](https://i.loli.net/2021/06/28/bhV6oXOGcrRCKa7.png)

![image.png](https://i.loli.net/2021/06/28/mXs6hfWj3blBCUN.png)

![image.png](https://i.loli.net/2021/06/28/6xgAZFjcWQVyGXM.png)

分类管理：

![image.png](https://i.loli.net/2021/06/28/4AYcmw8n196frFi.png)

用户管理：

![image.png](https://i.loli.net/2021/06/28/ScjT5X8eV1HsUqO.png)

权限系统：

![image.png](https://i.loli.net/2021/06/28/5QN9nMLCaowjFPf.png)

日志系统：

![image.png](https://i.loli.net/2021/06/28/nSgKcrj756ILWYx.png)

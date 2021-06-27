#ifndef APPLICATION_H
#define APPLICATION_H

/* Qt Headers - Qt 头文件 */
#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QJsonDocument>
#include <QtSql>

// Qt 程序实例 (main.cpp)
extern QApplication* app;

// SQL 实例 (SQL.cpp)
extern QSqlDatabase db;

// SQL 数据库连接信息 (SQL.cpp)
extern QString databaseHost;
extern QString databaseUser;
extern QString databasePassword;
extern QString databaseName;
extern int databasePort;

// 建立 SQL 数据库连接 (SQL.cpp)
bool connectDatabase();

// 用户信息
extern int loginUserID;
extern int loginUserRole;
extern QString loginUserName;

// 创建提示框
void createMessageBox(QString content);
#endif

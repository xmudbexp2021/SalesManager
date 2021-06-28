#include "application.h"

void createMessageBox(QString content)
{
    QMessageBox::information(NULL, "提示", content, QMessageBox::Yes, QMessageBox::Yes);
}

int hasPermission(int uid, QString domain, QString type)
{
    // 获取 UID 对应角色
    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE uid = ?");
    query.addBindValue(uid);
    if (!query.exec()) {
        qDebug() << query.lastError();
        return -1;
    }
    if (!query.next())
        return -2;

    int role = query.value(0).toInt();

    // 系统管理员默认具有所有权限域和所有类型的权限;
    if (role == 0) {
        return 1;
    }
    // 特殊角色具有权限
    else if (role == 1) {
        if (domain == "goods" || domain == "pictures"
                || domain == "goods_logs" || domain == "goods_sellouts"
                || domain == "discounts" || domain == "categories")
            return 1;
    }
    else if (role == 2) {
        if (domain == "users" || domain == "user_permissions")
            return 1;
    }

    // 查询 SQL 权限表中是否有对应项
    query.prepare("SELECT `grant` FROM user_permissions WHERE uid = ? AND type = ? AND domain = ?");
    query.addBindValue(uid);
    query.addBindValue(type);
    query.addBindValue(domain);
    if (!query.exec()) {
        qDebug() << query.lastError();
        return -1;
    }
    if (!query.next()) {
        if (type == "select")
            return 1;               // 默认具有读权限
        return 0;                   // 如果没有指定，则没有任何权限
    } else {
        return query.value(0).toInt();
    }
}

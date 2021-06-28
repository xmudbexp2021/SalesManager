#include "UserEdit.h"
#include "ui_UserEdit.h"

#include "application.h"

using std::vector;

UserEdit::UserEdit(QWidget *parent, int uid, QString action) :
    QMainWindow(parent),
    ui(new Ui::UserEdit)
{
    ui->setupUi(this);
    currentUid = uid;
    userAction = action;

    renderPermissionDomain();
    renderPermissionType();
    renderPermissionOperation();

    connect(ui->permissionDomain, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectionChange(int)));
    connect(ui->permissionType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectionChange(int)));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(onSaveButtonClick()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClick()));
    connect(ui->savePermissionButton, SIGNAL(clicked()), this, SLOT(onSavePermissionButtonClick()));

    if (action == "modify") {
        // 刷新信息
        refreshUserInfo();
        // 手动刷新一次权限状态
        onSelectionChange(0);
    } else if (action == "add") {
        setWindowTitle("新建用户");
        ui->titleLabel->setText("新建用户");
        ui->userPermissionBox->setVisible(0);
        ui->lastLoginLabel->setVisible(0);
        ui->lastLoginTitleTable->setVisible(0);
    }
}

UserEdit::~UserEdit()
{
    delete ui;
}

void UserEdit::renderPermissionDomain()
{
    // 数据表列表
    vector<QString> tables = {"goods", "users", "categories", "goods_logs", "goods_sellouts", "pictures", "user_permissions", "discounts"};

    for (auto table : tables) {
        domain.push_back(table);
        ui->permissionDomain->addItem(table);
    }
}

void UserEdit::renderPermissionType()
{
    // 每个表都具有四级权限：读取（查询）、插入（新增）、更新（编辑）、删除
    ui->permissionType->addItem("查询");
    ui->permissionType->addItem("新增");
    ui->permissionType->addItem("编辑");
    ui->permissionType->addItem("删除");
}

void UserEdit::renderPermissionOperation()
{
    ui->permisionOp->addItem("授予");
    ui->permisionOp->addItem("拒绝");
}

void UserEdit::onSelectionChange(int index)
{
    // 获取当前选择的权限域和权限类型
    int domainIndex = ui->permissionDomain->currentIndex();
    int typeIndex = ui->permissionType->currentIndex();

    QString domainName = domain[domainIndex];
    QString typeName =
            typeIndex == 0 ? "select" :
            typeIndex == 1 ? "insert" :
            typeIndex == 2 ? "update" :
            typeIndex == 3 ? "delete" : "unknown";

    int status = hasPermission(currentUid, domainName, typeName);
    if (status >= 0)
        ui->statusLabel->setText(status == 0 ? "已拒绝" : "已授予");
}

void UserEdit::onSavePermissionButtonClick()
{
    if (!hasPermission(loginUserID, "user_permissions", "insert")
            || !hasPermission(loginUserID, "user_permissions", "update")
            || !hasPermission(loginUserID, "user_permissions", "delete")) {
        createMessageBox("您无权编辑用户权限！");
        return;
    }

    if (currentUid == loginUserID) {
        createMessageBox("您无法编辑自己的权限！");
        return;
    }

    // 获取当前选择的权限域和权限类型
    int domainIndex = ui->permissionDomain->currentIndex();
    int typeIndex = ui->permissionType->currentIndex();
    int opIndex = ui->permisionOp->currentIndex();

    QString domainName = domain[domainIndex];
    QString typeName =
            typeIndex == 0 ? "select" :
            typeIndex == 1 ? "insert" :
            typeIndex == 2 ? "update" :
            typeIndex == 3 ? "delete" : "unknown";

    if (!hasPermission(loginUserID, domainName, typeName)) {
        createMessageBox("您自身没有访问该域的相关类型权限！");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE uid = ?");
    query.addBindValue(currentUid);
    if (!query.exec())
        qDebug() << query.lastError();
    if (!query.next())
        return;
    if ((loginUserRole != 0 && query.value(0).toInt() == 0) || currentUid == 0) {
        createMessageBox("无法对系统管理员的权限进行修改！");
        return;
    }

    // 删除已设置的权限记录
    query.prepare("DELETE FROM user_permissions WHERE uid = ? AND domain = ? AND type = ?");
    query.addBindValue(currentUid);
    query.addBindValue(domainName);
    query.addBindValue(typeName);
    if (!query.exec()) {
        qDebug() << query.lastError();
        return;
    }

    // 设置新权限
    query.prepare("INSERT INTO user_permissions (uid, domain, type, `grant`) VALUES(?, ?, ?, ?)");
    query.addBindValue(currentUid);
    query.addBindValue(domainName);
    query.addBindValue(typeName);
    query.addBindValue(!opIndex);
    qDebug() << domainName << typeName << opIndex;
    if (!query.exec()) {
        qDebug() << query.lastError();
        return;
    }

    // 刷新显示
    onSelectionChange(0);
}

void UserEdit::refreshUserInfo()
{
    QSqlQuery query;
    query.prepare("SELECT username, role, phone, email, last_login FROM users WHERE uid = ?");
    query.addBindValue(currentUid);
    if (!query.exec()) {
        qDebug() << query.lastError();
        return;
    }
    if (!query.next()) {
        qDebug() << "User does not exist.";
        return;
    }
    ui->uidLabel->setText(QString::number(currentUid));
    ui->usernameEdit->setText(query.value(0).toString());
    ui->phoneEdit->setText(query.value(2).toString());
    ui->emailEdit->setText(query.value(3).toString());
    ui->lastLoginLabel->setText(query.value(4).toString());

    int role = query.value(1).toInt();
    switch (role) {
    case 0:
        ui->admin->setChecked(true);
        break;
    case 1:
        ui->manager->setChecked(true);
        break;
    case 2:
        ui->hr->setChecked(true);
        break;
    default:
        ui->user->setChecked(true);
        break;
    }
}

void UserEdit::onSaveButtonClick()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString email = ui->emailEdit->text();
    QString phone = ui->phoneEdit->text();

    int targetRole;
    if (ui->admin->isChecked())
        targetRole = 0;
    else if (ui->manager->isChecked())
        targetRole = 1;
    else if (ui->hr->isChecked())
        targetRole = 2;
    else
        targetRole = 3;

    if (userAction == "modify") {
        if (!hasPermission(loginUserID, "users", "update")) {
            createMessageBox("您无权编辑用户！");
            return;
        }

        if (targetRole < loginUserRole) {
            createMessageBox("不允许创建比当前用户权限更高的用户！");
            return;
        }

        QSqlQuery query;
        query.prepare("UPDATE users SET username = ?, email = ?, phone = ? WHERE uid = ?");
        query.addBindValue(username);
        query.addBindValue(email);
        query.addBindValue(phone);
        query.addBindValue(currentUid);
        if (!query.exec()) {
            qDebug() << query.lastError();
        }

        // 修改权限
        int userRole = -1;
        query.prepare("SELECT role FROM users WHERE uid = ?");
        query.addBindValue(currentUid);
        if (!query.exec())
            qDebug() << query.lastError();
        if (query.next())
            userRole = query.value(0).toInt();

        if (targetRole == userRole) {}
        else if (currentUid == loginUserID || currentUid == 1 || (userRole == 0 && loginUserID != 1)) {
            createMessageBox("不能修改自己的权限或系统管理员的权限！");
        }
        else if (loginUserRole != 0 && targetRole == 0) {
            createMessageBox("您无权设置系统管理员！");
        }
        else {
            query.prepare("UPDATE users SET role = ? WHERE uid = ?");
            query.addBindValue(targetRole);
            query.addBindValue(currentUid);
            if (!query.exec())
                qDebug() << query.lastError();
        }

        // 修改密码
        if (password.length()) {
            QByteArray tmp, res;
            QCryptographicHash hash(QCryptographicHash::Md5);
            tmp.append(password);
            hash.addData(tmp);
            res = hash.result();
            QString inputMd5 = res.toHex();

            query.prepare("UPDATE users SET password = ? WHERE uid = ?");
            query.addBindValue(inputMd5);
            query.addBindValue(currentUid);
            if (!query.exec())
                qDebug() << query.lastError();
        }

        createMessageBox("用户信息已修改！");
        emit tableChanged();
        QWidget::close();
    } else if (userAction == "add") {
        if (!hasPermission(loginUserID, "users", "insert")) {
            createMessageBox("您无权新建用户！");
            return;
        }

        if (targetRole < loginUserRole) {
            createMessageBox("不允许创建比当前用户权限更高的用户！");
            return;
        }

        if (!password.length() || !username.length()) {
            createMessageBox("用户名和密码不能为空！");
            return;
        }

        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM users WHERE username = ?") ;
        query.addBindValue(username);
        if (!query.exec()) {
            qDebug() << query.lastError();
            return;
        }
        if (!query.next()) {
            qDebug() << query.lastError();
            return;
        }
        if (query.value(0).toInt() > 0) {
            createMessageBox("用户名为 " + username + " 的用户已存在！");
            return;
        }

        QByteArray tmp, res;
        QCryptographicHash hash(QCryptographicHash::Md5);
        tmp.append(password);
        hash.addData(tmp);
        res = hash.result();
        QString inputMd5 = res.toHex();

        query.prepare("INSERT INTO users(username, password, role, phone, email) VALUES(?, ?, ?, ?, ?)");
        query.addBindValue(username);
        query.addBindValue(inputMd5);
        query.addBindValue(targetRole);
        query.addBindValue(phone);
        query.addBindValue(email);
        if (!query.exec())
            qDebug() << query.lastError();
        if (targetRole == 3) {
            query.prepare("INSERT INTO user_permissions(uid, type, domain, `grant`) "
                          "VALUES (?, 'select', 'goods_sellout', 1), (?, 'insert', 'goods_sellout', 1)");
            query.addBindValue(query.lastInsertId());
            query.addBindValue(query.lastInsertId());
            if (!query.exec())
                qDebug() << query.lastError();
        }

        createMessageBox("新建用户成功！");
        emit tableChanged();
        QWidget::close();
    }
}

void UserEdit::onCancelButtonClick()
{
    QWidget::close();
}

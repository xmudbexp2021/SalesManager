#include "loginwindow.h"
#include "ui_loginwindow.h"

#include "MainWindow.h"

#include "application.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    // 绑定按钮事件
    connect(ui->exitButton, SIGNAL(clicked()), this, SLOT(onExitButtonClick()));
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(onLoginButtonClick()));
    connect(ui->settingButton, SIGNAL(clicked()), this, SLOT(onSettingButtonClick()));

    // 数据库连接完成事件
    connect(&sqlThread, SIGNAL(connectDone(bool)), this, SLOT(onConnectDone(bool)));

    // 连接数据库事件
    sqlThread.execute();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}


/**
 * @brief 退出按钮点击响应事件
 */
void LoginWindow::onExitButtonClick()
{
    app->exit(0);
}

/**
 * @brief 登录按钮点击响应事件
 */
void LoginWindow::onLoginButtonClick()
{
    ui->dbAddrLabel->setText("正在鉴权并登录...");

    // 获取用户名密码输入
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.length() == 0 || password.length() == 0) {
        createMessageBox("用户名和密码不能为空！");
        return;
    }

    // 查找是否存在用户
    QSqlQuery query;
    query.prepare("SELECT password, uid, username, role FROM users WHERE username = ?");
    query.addBindValue(username);
    if (!query.exec()) {
        createMessageBox("数据库连接失败：" + query.lastError().text());
    }

    // 用户存在，则验证密码
    if (query.next()) {
        int uid = query.value(1).toInt();

        // 计算密码的 MD5
        QString passwordMd5 = query.value(0).toString(),
                inputMd5;

        QByteArray tmp, res;
        QCryptographicHash hash(QCryptographicHash::Md5);
        tmp.append(password);
        hash.addData(tmp);
        res = hash.result();
        inputMd5 = res.toHex();

        if (passwordMd5 == inputMd5) {
            // 更新上次登录时间
            QDateTime currentTime = QDateTime::currentDateTime();
            QString currentTimeString = currentTime.toString("yyyy-MM-dd hh:mm:ss");
            QSqlQuery lastLogin;
            lastLogin.prepare("UPDATE users SET last_login = ? WHERE uid = ?");
            lastLogin.addBindValue(currentTimeString);
            lastLogin.addBindValue(uid);
            lastLogin.exec();

            // 设置用户信息
            loginUserID = uid;
            loginUserName = query.value(2).toString();
            loginUserRole = query.value(3).toUInt();

            // 跳转到新页面，同时关闭自己
            MainWindow* mainWindow = new MainWindow();
            mainWindow->show();
            this->close();

        } else {
            createMessageBox("密码错误，拒绝登录！");
        }
    }
    // 用户不存在
    else {
        createMessageBox("用户 " + username + " 不存在！");
    }

}

/**
 * @brief 服务器设置按钮点击响应事件
 */
void LoginWindow::onSettingButtonClick()
{
    serverSetting = new ServerSetting(this);
    serverSetting->show();


    // 数据库信息变更
    connect(serverSetting, SIGNAL(databaseInfoChanged()), this, SLOT(onDatabaseInfoChanged()));
}

/**
 * @brief 数据库连接完成事件
 * @param result 连接结果
 */
void LoginWindow::onConnectDone(bool result)
{
    if (result) {
        ui->dbAddrLabel->setText("已连接到 MySQL 服务器 " + databaseHost + ":" + QString::number(databasePort));
    } else {
        ui->dbAddrLabel->setText("数据库连接失败 :(");
    }
}

void LoginWindow::onDatabaseInfoChanged()
{
    ui->dbAddrLabel->setText("正在重新连接数据库...");
    if (sqlThread.isRunning()) {
        sqlThread.exit();
    }
    sqlThread.execute();
}

#include "loginwindow.h"
#include "ui_loginwindow.h"

#include "application.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    // 绑定按钮事件
    connect(ui->exitButton, SIGNAL(clicked()), this, SLOT(onExitButtonClick()));
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

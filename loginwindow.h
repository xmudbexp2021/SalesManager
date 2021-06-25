#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>

#include "SQLThread.h"
#include "ServerSetting.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private:
    Ui::LoginWindow *ui;

    SQLThread sqlThread;                    // 数据库服务器连接线程

    ServerSetting *serverSetting;           // 数据库设置窗口实例

public slots:
    void onLoginButtonClick();              // 登录按钮点击事件
    void onExitButtonClick();               // 退出按钮点击事件
    void onSettingButtonClick();            // 设置按钮点击事件

    void onConnectDone(bool result);        // 数据库完成连接事件
    void onDatabaseInfoChanged();           // 数据库连接信息改变事件

};

#endif // LOGINWINDOW_H

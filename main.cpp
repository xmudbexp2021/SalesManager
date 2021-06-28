#include "loginwindow.h"

#include "application.h"

QApplication* app;

int main(int argc, char *argv[])
{
    // 初始化 QT 窗口
    app = new QApplication(argc, argv);

    // 设置 DLL 目录
    QString strLibPath(QDir::toNativeSeparators(QApplication::applicationDirPath()) + QDir::separator() + "plugins");
    app->addLibraryPath(strLibPath);

    // 启动程序时，显示登录窗口
    LoginWindow w;
    w.show();

    return app->exec();
}

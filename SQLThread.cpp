#include "SQLThread.h"


// 数据库连接信息
QString databaseHost = "";
QString databaseName = "";
QString databaseUser = "";
QString databasePassword = "";
int databasePort = 3306;


// 数据库实例
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");


/**
 * @brief 启动线程
 */
void SQLThread::run()
{
    connect();
}


/**
 * @brief 读取数据库配置文件config.json
 */
void SQLThread::readConfig()
{
    QString raw;
    QFile configFile;

    configFile.setFileName(":/config.json");

    if (!configFile.exists()) {
        qCritical() << "配置文件 config.json 不存在！";
        return;
    }

    configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    raw = configFile.readAll();
    configFile.close();

    try {
        QJsonDocument doc = QJsonDocument::fromJson(raw.toUtf8());
        QJsonObject object = doc.object();
        QVariantMap objectMap = object.toVariantMap();

        databaseHost = objectMap["databaseHost"].toString();
        databaseName = objectMap["databaseName"].toString();
        databaseUser = objectMap["databaseUser"].toString();
        databasePassword = objectMap["databasePassword"].toString();
        databasePort = objectMap["databasePort"].toInt();
    } catch (std::exception e) {
        qCritical() << "配置文件 config.json 无效。";
        qCritical() << e.what();
    }
}


/**
 * @brief 连接 MySQL 数据库
 * @return 如果连接成功，返回 true
 */
void SQLThread::connect()
{
    qInfo() << "[SQLThread] Connecting Database...";

    if (databaseHost.length() == 0)
        readConfig();

    db.setHostName(databaseHost);
    db.setDatabaseName(databaseName);
    db.setUserName(databaseUser);
    db.setPassword(databasePassword);
    db.setPort(databasePort);
    db.open();

    if (db.isOpenError()) {
        qInfo() << "[SQLThread] Database connection failed.";
        qInfo() << db.lastError();

        emit connectDone(false);
    } else {
        qInfo() << "[SQLThread] Database connected!";

        emit connectDone(true);
    }
}


void SQLThread::execute()
{
    start(HighestPriority);
}

void SQLThread::terminate()
{
    quit();
}

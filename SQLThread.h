#ifndef SQLTHREAD_H
#define SQLTHREAD_H

#include "application.h"

class SQLThread : public QThread
{
    Q_OBJECT

public:
    explicit SQLThread(QObject *parent = nullptr) {};
    void execute();
    void terminate();

protected:
    void run();

signals:
    void connectDone(bool result);

public slots:

private:
    void readConfig();
    void connect();
};

#endif // SQLTHREAD_H

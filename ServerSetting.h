#ifndef SERVERSETTING_H
#define SERVERSETTING_H

#include <QMainWindow>

namespace Ui {
class ServerSetting;
}

class ServerSetting : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerSetting(QWidget *parent = nullptr);
    ~ServerSetting();

private:
    Ui::ServerSetting *ui;

signals:
    void databaseInfoChanged();

public slots:
    void onApply();
    void onCancel();
};

#endif // SERVERSETTING_H

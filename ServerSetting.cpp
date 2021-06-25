#include "ServerSetting.h"
#include "ui_ServerSetting.h"

#include "application.h"

ServerSetting::ServerSetting(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerSetting)
{
    ui->setupUi(this);

    ui->serverEdit->setText(databaseHost);
    ui->portEdit->setText(QString::number(databasePort));
    ui->userEdit->setText(databaseUser);
    ui->passEdit->setText(databasePassword);
    ui->nameEdit->setText(databaseName);

    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(onApply()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));
}

ServerSetting::~ServerSetting()
{
    delete ui;
}

void ServerSetting::onApply()
{
    databaseHost = ui->serverEdit->text();
    databaseName = ui->nameEdit->text();
    databaseUser = ui->userEdit->text();
    databasePassword = ui->passEdit->text();
    databasePort = ui->portEdit->text().toInt();

    QMessageBox::information(NULL, "成功", "数据库信息设置成功", QMessageBox::Yes, QMessageBox::Yes);

    emit databaseInfoChanged();

    QWidget::close();
}

void ServerSetting::onCancel()
{
    QWidget::close();
}

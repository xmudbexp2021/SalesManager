#include "DiscountEdit.h"
#include "ui_DiscountEdit.h"

#include "application.h"

DiscountEdit::DiscountEdit(QWidget *parent,
                           QString action,
                           int did,
                           int gid) :
    QMainWindow(parent),
    ui(new Ui::DiscountEdit)
{
    ui->setupUi(this);

    userAction = action;
    currentDid = did;
    currentGid = gid;

    fetchDiscountInfo();

    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(onSaveButtonClick()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClick()));
}

DiscountEdit::~DiscountEdit()
{
    delete ui;
}

void DiscountEdit::fetchDiscountInfo()
{
    QSqlQuery query;

    if (userAction == "modify") {
        query.prepare("SELECT discounts.gid, title, "
                      "start_time, end_time, "
                      "discount, restock_value, selling_value "
                      "FROM discounts "
                      "JOIN goods "
                      "ON discounts.gid = goods.gid "
                      "WHERE did  = ?");
        query.addBindValue(currentDid);

        if (!query.exec() || !query.next()) {
            qDebug() << query.lastError();
            return;
        }

        currentGid = query.value(0).toInt();

        ui->gidLabel->setText(query.value(0).toString());
        ui->titleLabel->setText(query.value(1).toString());
        ui->restockLabel->setText(query.value(5).toString());
        ui->customLabel->setText(query.value(6).toString());

        ui->valueEdit->setText(query.value(4).toString());
        ui->startTime->setDateTime(query.value(2).toDateTime());
        ui->endTime->setDateTime(query.value(3).toDateTime());
    } else {
        query.prepare("SELECT gid, title, "
                      "restock_value, selling_value "
                      "FROM goods "
                      "WHERE gid  = ?");
        query.addBindValue(currentGid);

        if (!query.exec() || !query.next()) {
            qDebug() << query.lastError();
            return;
        }

        ui->gidLabel->setText(query.value(0).toString());
        ui->titleLabel->setText(query.value(1).toString());
        ui->restockLabel->setText(query.value(2).toString());
        ui->customLabel->setText(query.value(3).toString());
    }
}

void DiscountEdit::onSaveButtonClick()
{
    double discountValue = ui->valueEdit->text().toDouble();
    QDateTime startTime = ui->startTime->dateTime();
    QDateTime endTime = ui->endTime->dateTime();

    if (endTime < startTime) {
        createMessageBox("折扣结束时间不能早于开始时间！");
        return;
    }

    QSqlQuery query;

    if (userAction == "add") {
        query.prepare("INSERT INTO discounts (gid, uid, start_time, end_time, discount) VALUES(?, ?, ?, ?, ?)");
        query.addBindValue(currentGid);
        query.addBindValue(loginUserID);
        query.addBindValue(startTime);
        query.addBindValue(endTime);
        query.addBindValue(discountValue);
        if (!query.exec()) {
            qDebug() << query.lastError();
            return;
        }

        createMessageBox("新建折扣信息成功！");
        emit tableChanged();
        QWidget::close();
    }
    else {
        query.prepare("UPDATE discounts SET discount = ?, start_time = ?, end_time = ? WHERE did = ?");
        query.addBindValue(discountValue);
        query.addBindValue(startTime);
        query.addBindValue(endTime);
        query.addBindValue(currentDid);
        if (!query.exec()) {
            qDebug() << query.lastError();
            return;
        }

        createMessageBox("编辑折扣信息成功！");
        emit tableChanged();
        QWidget::close();
    }
}

void DiscountEdit::onCancelButtonClick()
{
    QWidget::close();
}

#include "SellPage.h"
#include "ui_SellPage.h"
#include "application.h"
sellPage::sellPage(QWidget *parent,int Qgid) :
    QDialog(parent),
    ui(new Ui::sellPage)
{
    ui->setupUi(this);
    this->gid = Qgid;
    initMessage();

    //绑定按钮事件
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClick()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(onNextButtonClick()));
}

sellPage::~sellPage()
{
    delete ui;
}

/* 确定按钮点击事件 */
void sellPage::onNextButtonClick()
{
    QString customName = ui->customLine->text();
    QString phoneNumb = ui->phoneLine->text();
    int sellNumb = ui->sellNumBox->value();
    QString sellPrice = ui->priceLine->text();
    float priceEventually = sellPrice.toFloat();
    if (sellNumb <= 0){
        createMessageBox("售卖数量不能为0或为负！");
    }
    QSqlQuery query;
    query.prepare("insert into goods_sellouts(gid,uid,number,price,customer_name,customer_contact,sell_time) values( ?, ?, ?, ?, ?, ?,CURRENT_TIMESTAMP )");
    query.addBindValue(gid);
    query.addBindValue(loginUserID);
    query.addBindValue(sellNumb);
    query.addBindValue(priceEventually);
    query.addBindValue(customName);
    query.addBindValue(phoneNumb);
    if(!query.exec()){
        createMessageBox("售卖商品失败！");
        qCritical() << query.lastError();
    }
    createMessageBox("售卖成功！");
    emit tableChanged();
    QDialog::close();
}

/* 取消按钮点击事件 */
void sellPage::onBackButtonClick()
{
    QDialog::close();
}

/* 初始化默认信息 */
void sellPage::initMessage()
{
    QSqlQuery query;
    query.prepare("select gid,title,sku,selling_value,inventory from goods where gid = ?");
    query.addBindValue(gid);
    query.exec();
    query.next();
    QString uidHere = QString::number(loginUserID);
    QString gidHere = QString::number(gid);
    QString nameHere = query.value(1).toString();
    QString skuHere = query.value(2).toString();
    QString valueHere = query.value(3).toString();
    int inventoryHere = query.value(4).toInt();
    ui->gidLine->setText(gidHere);
    ui->nameLine->setText(nameHere);
    ui->priceLine->setText(valueHere);
    ui->skuLine->setText(skuHere);
    ui->userLine->setText(loginUserName);

    //除需要自己填写的信息和price外其他信息设为只读
    ui->gidLine->setReadOnly(true);
    ui->nameLine->setReadOnly(true);
    ui->skuLine->setReadOnly(true);
    ui->userLine->setReadOnly(true);
    ui->sellNumBox->setMaximum(inventoryHere);
}

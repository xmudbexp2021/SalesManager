#include "GoodDetail.h"
#include "ui_GoodDetail.h"

#include "application.h"

GoodDetail::GoodDetail(QWidget *parent, int gid) :
    QMainWindow(parent),
    ui(new Ui::GoodDetail),
    imageIndex(0)
{
    ui->setupUi(this);

    // 渲染商品基础信息
    QSqlQuery query;
    query.prepare("SELECT username, sku, title, subtitle, "
                  "inventory, restock_value,selling_value, "
                  "length, width, height, weight, color "
                  "FROM goods "
                  "LEFT JOIN users ON goods.uid = users.uid "
                  "WHERE gid = ?");
    query.addBindValue(gid);
    if (!query.exec()) {
        qDebug() << query.lastError();
        return;
    }

    if (!query.next()) {
        createMessageBox("商品信息不存在！");
        return;
    }

    ui->userLabel->setText(query.value(0).toString());
    ui->skuLabel->setText(query.value(1).toString());
    ui->titleLabel->setText(query.value(2).toString());
    ui->subTitleLabel->setText(query.value(3).toString());
    ui->inventoryLabel->setText(query.value(4).toString());
    ui->restockLabel->setText(query.value(5).toString());
    ui->customLabel->setText(QString::number(query.value(6).toDouble()));

    QString length = query.value(7).toString(),
            width = query.value(8).toString(),
            height = query.value(9).toString();

    ui->sizeLabel->setText(length + "×" + width + "×" + height);
    ui->weightLabel->setText(query.value(10).toString());
    ui->colorLabel->setText(query.value(11).toString());

    // 渲染图片
    query.prepare("SELECT pid, pictures "
                  "FROM pictures "
                  "WHERE gid = ?");
    query.addBindValue(gid);
    query.exec();

    for (int i = 0; query.next(); i++) {
        QString imageBase64 = query.value(1).toString();
        QPixmap pixMap;
        pixMap.loadFromData(QByteArray::fromBase64(imageBase64.toLatin1()));
        images.push_back(pixMap);
    }

    if (images.size())
        renderImage();
    else
        ui->imageLabel->setText("无展示图片。");

    // 绑定按钮事件
    connect(ui->prevImageButton, SIGNAL(clicked()), this, SLOT(onPrevButtonClick()));
    connect(ui->nextImageButton, SIGNAL(clicked()), this, SLOT(onNextButtonClick()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClick()));

    // todo: 进货、售货按钮事件
}

void GoodDetail::renderImage()
{
    int containerHeight = ui->imageLabel->height();
    int width = images[imageIndex].width(),
        height = images[imageIndex].height();
    double ratio = 1.0 * width / height;
    width = ratio * containerHeight;
    height = containerHeight;

    ui->imageLabel->setPixmap(images[imageIndex].scaled(width, height));
    ui->imageLabel->show();
}

void GoodDetail::onPrevButtonClick()
{
    imageIndex = images.size() ? (imageIndex + images.size() - 1) % images.size() : -1;
    if (imageIndex >= 0)
        renderImage();
}

void GoodDetail::onNextButtonClick()
{
    imageIndex = images.size() ? (imageIndex + 1) % images.size() : -1;
    if (imageIndex >= 0)
        renderImage();
}

GoodDetail::~GoodDetail()
{
    delete ui;
}

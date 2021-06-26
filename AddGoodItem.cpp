#include "AddGoodItem.h"
#include "ui_AddGoodItem.h"

#include "application.h"

AddGoodItem::AddGoodItem(QWidget *parent, QString action, int gid) :
    QMainWindow(parent),
    ui(new Ui::AddGoodItem)
{
    ui->setupUi(this);

    // 创建 TableView 的模型
    model = new QStandardItemModel;


    // 注入分类信息
    QSqlQuery query;
    query.prepare("SELECT cid, name FROM categories");
    query.exec();

    for (int idx = 0; query.next(); idx++) {
        int categoryId = query.value(0).toInt();
        QString categoryName = query.value(1).toString();

        indexToCid[idx] = categoryId;
        cidToIndex[categoryId] = idx;
        ui->categoryComboBox->addItem(categoryName);
    }

    // 根据不同操作执行不同逻辑
    this->userAction = action;
    if (action == "add") {
        setWindowTitle("新建商品");
        ui->actionLabel->setText("新建商品:");
    }
    else if (action == "modify") {
        setWindowTitle("修改商品信息");
        ui->actionLabel->setText("修改商品信息:");

        fetchGoodInfo(gid);
    }

    // 设置图片 TableView 样式
    ui->imageView->verticalHeader()->hide();

    // 绑定按钮事件
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(onSaveButtonClick()));
    connect(ui->deleteImageButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClick()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClick()));
    connect(ui->uploadImageButton, SIGNAL(clicked()), this, SLOT(onUploadButtonClick()));
}

AddGoodItem::~AddGoodItem()
{
    imagesList.clear();
    indexToCid.clear();
    cidToIndex.clear();
    newImages.clear();

    delete ui;
    delete model;
}

/* 获取已存在的商品信息并填入 LineEdit 中 */
void AddGoodItem::fetchGoodInfo(int gid)
{
    // 获取商品信息
    QSqlQuery query;
    query.prepare("SELECT gno, cid, title, subtitle, sku, inventory, "
                  "restock_value, selling_value, length, width, height, weight, color "
                  "FROM goods "
                  "WHERE gid = ?");
    query.addBindValue(gid);
    query.exec();

    if (!query.next()) {
        createMessageBox("未找到 ID 为" + QString::number(gid) + "的商品！");
        return;
    }

    ui->gidEdit->setText(QString::number(gid));
    ui->gnoEdit->setText(query.value(0).toString());
    ui->categoryComboBox->setCurrentIndex(cidToIndex[query.value(1).toInt()]);
    ui->titleEdit->setText(query.value(2).toString());
    ui->subtitleEdit->setText(query.value(3).toString());
    ui->skuEdit->setText(query.value(4).toString());
    ui->inventoryEdit->setText(query.value(5).toString());
    ui->restockValueEdit->setText(query.value(6).toString());
    ui->sellingValueEdit->setText(query.value(7).toString());
    ui->lengthEdit->setText(query.value(8).toString());
    ui->widthEdit->setText(query.value(9).toString());
    ui->heightEdit->setText(query.value(10).toString());
    ui->weightEdit->setText(query.value(11).toString());
    ui->colorEdit->setText(query.value(12).toString());

    getImages(gid);
}

/* 获取商品图片 */
void AddGoodItem::getImages(int gid)
{
    // 清空映射信息
    imagesList.clear();
    colToPid.clear();

    model->clear();

    // 获取商品图片
    QSqlQuery query;
    query.prepare("SELECT pid, pictures "
                  "FROM pictures "
                  "WHERE gid = ?");
    query.addBindValue(gid);
    query.exec();


    // 将图片加入 ImageView
    int containerHeight = ui->imageView->height() - 10;          // 容器高度
    for (int i = 0; query.next(); i++) {
        int pid = query.value(0).toInt();
        colToPid[i] = pid;

        QString imageBase64 = query.value(1).toString();
        QPixmap pixMap;
        pixMap.loadFromData(QByteArray::fromBase64(imageBase64.toLatin1()));

        QStandardItem *item = new QStandardItem;

        // 按照长宽等比缩放至容器高度
        int width = pixMap.width(), height = pixMap.height();
        double ratio = 1.0 * width / height;
        if (width > height) {
            width = containerHeight;
            height = width / ratio;
        } else {
            height = containerHeight;
            width = height * ratio;
        }

        item->setData(QVariant(pixMap.scaled(width, height)), Qt::DecorationRole);
        imagesList.push_back(item);
    }

    // 重绘 TableView
    if (imagesList.size())
        redrawImageTableView();
}

/* 上传图片按钮点击事件 */
void AddGoodItem::onUploadButtonClick()
{
    QString filename = QFileDialog::getOpenFileName(this, "选择图像", "", "Images (*.bmp *.png *.jpg  *.jpeg)");

    int containerHeight = ui->imageView->height() - 10;          // 容器高度

    if (filename.isEmpty())
        return;
    else {
        QImage* img = new QImage;
        if (img->load(filename)) {
            // 添加到数组中
            newImages.push_back(img);

            // 在 ImageView 中显示
            QStandardItem *item = new QStandardItem;
            QPixmap pixMap = QPixmap::fromImage(*img);

            // 按照长宽等比缩放至容器高度
            int width = pixMap.width(), height = pixMap.height();
            double ratio = 1.0 * width / height;
            if (width > height) {
                width = containerHeight;
                height = width / ratio;
            } else {
                height = containerHeight;
                width = height * ratio;
            }

            item->setData(QVariant(pixMap.scaled(width, height)), Qt::DecorationRole);
            imagesList.push_back(item);

            redrawImageTableView();
        }
    }
}

/* 重新绘制图片的 TableView */
void AddGoodItem::redrawImageTableView()
{
    // 重新渲染 TableView 图片
    for (int i = 0; i < (int)imagesList.size(); i++) {
        model->setItem(0, i, imagesList[i]);
    }

    int containerHeight = ui->imageView->height() - 10;

    // 设置图片高度撑满容器高度，宽度与高度保持一致
    ui->imageView->setModel(model);
    ui->imageView->setRowHeight(0, containerHeight);
    for (int i = 0; i < (int)imagesList.size(); i++)
        ui->imageView->setColumnWidth(i, containerHeight);
}

/* 删除按钮点击事件 */
void AddGoodItem::onDeleteButtonClick()
{
    // 获取当前选择
    if (ui->imageView->selectionModel()->hasSelection()) {
        QModelIndexList selection = ui->imageView->selectionModel()->selectedColumns();
        for (int i = 0; i < selection.count(); i++) {
            QModelIndex index = selection.at(i);
            int colId = index.column();

            qDebug() << "colId: " << colId;

            QSqlQuery query;
            // 删除的是尚未上传的图片
            if (colId >= (int)colToPid.size()) {
                imagesList.erase(std::next(imagesList.begin(), colId));
                newImages.erase(std::next(newImages.begin(), colId -  (int)colToPid.size()));
                redrawImageTableView();
            }
            // 删除的是已经上传的图片
            else {
                qDebug() << "delete image pid = " << colToPid[colId];

                QSqlQuery query;
                query.prepare("DELETE FROM pictures WHERE pid = ?");
                query.addBindValue(colToPid[colId]);
                if (!query.exec()) {
                    qDebug() << query.lastError();
                }
            }
        }

        // 完成之后，上传图片，并刷新 TableView
        int gid = -1;
        QString gidRaw = ui->gidEdit->text();
        if (gidRaw.length())
            gid = gidRaw.toInt();

        if (gid != -1) {
            processImageUpload(gid);
            getImages(gid);
            redrawImageTableView();
        }
    } else {
        createMessageBox("未选中任何图片！");
    }
}

/* 保存按钮点击事件 */
void AddGoodItem::onSaveButtonClick()
{
    /* 获取用户输入 */
    int gid;                                        // 商品自增 ID
    QString gidRaw = ui->gidEdit->text();
    if (gidRaw.length())
        gid = gidRaw.toInt();

    int cid = indexToCid[ui->categoryComboBox->currentIndex()];     // 商品分类 ID

    QString gno = ui->gnoEdit->text();              // 商品编号
    QString title = ui->titleEdit->text();          // 商品名称
    QString subtitle = ui->subtitleEdit->text();    // 商品描述
    QString sku = ui->skuEdit->text();              // 商品规格

    double inventory = ui->inventoryEdit->text().toDouble();            // 商品库存
    double restockValue = ui->restockValueEdit->text().toDouble();      // 进货价
    double sellingValue = ui->sellingValueEdit->text().toDouble();      // 售价
    double length = ui->lengthEdit->text().toDouble();      // 长度
    double width = ui->widthEdit->text().toDouble();        // 宽度
    double height = ui->heightEdit->text().toDouble();      // 高度
    double weight = ui->weightEdit->text().toDouble();
    QString color = ui->colorEdit->text();                  // 颜色

    QSqlQuery query;

    // 添加操作
    if (userAction == "add") {
        // 添加商品信息到 goods 表中
        query.prepare(
            "INSERT INTO goods("
                "gno, cid, uid, title, subtitle, sku, inventory, "
                "restock_value, selling_value, length, width, height, weight, color"
            ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(gno);
        query.addBindValue(cid);
        query.addBindValue(loginUserID);
        query.addBindValue(title);
        query.addBindValue(subtitle);
        query.addBindValue(sku);
        query.addBindValue(inventory);
        query.addBindValue(restockValue);
        query.addBindValue(sellingValue);
        query.addBindValue(length);
        query.addBindValue(width);
        query.addBindValue(height);
        query.addBindValue(weight);
        query.addBindValue(color);

        if (!query.exec()) {
            createMessageBox("添加商品失败！");
            qCritical() << query.lastError();
            return;
        }

        // 获取插入的表 ID
        int gid = query.lastInsertId().toInt();
        processImageUpload(gid);
        createMessageBox("添加商品成功！");
    }

    // 修改操作
    else if (userAction == "modify") {
        query.prepare(
            "UPDATE goods "
            "SET gno = ?,"
            "title = ?,"
            "subtitle = ?,"
            "sku = ?,"
            "inventory = ?,"
            "restock_value = ?, "
            "selling_value = ?, "
            "length = ?,"
            "width = ?, "
            "height = ?, "
            "weight = ?, "
            "color = ? "
            "WHERE gid = ?");

        query.addBindValue(gno);
        query.addBindValue(title);
        query.addBindValue(subtitle);
        query.addBindValue(sku);
        query.addBindValue(inventory);
        query.addBindValue(restockValue);
        query.addBindValue(sellingValue);
        query.addBindValue(length);
        query.addBindValue(width);
        query.addBindValue(height);
        query.addBindValue(weight);
        query.addBindValue(color);
        query.addBindValue(gid);

        if (!query.exec()) {
            createMessageBox("修改商品信息失败！");
            qCritical() << query.lastError();
            return;
        }

        processImageUpload(gid);
        createMessageBox("修改商品信息成功！");
    }
}

/* 处理图片上传 */
void AddGoodItem::processImageUpload(int gid)
{
    // 上传图片
    for (auto img : newImages) {
        QByteArray ba;
        QBuffer buffer(&ba);
        img->save(&buffer, "jpg");
        QByteArray imageHex = ba.toBase64();
        buffer.close();

        QSqlQuery query;
        query.prepare("INSERT INTO pictures (gid, pictures) VALUES (?, ?)");
        query.addBindValue(gid);
        query.addBindValue(imageHex);
        query.exec();
    }

    // 上传完成所有图片后清空数组
    newImages.clear();
}

/* 取消按钮点击事件 */
void AddGoodItem::onCancelButtonClick()
{
    QWidget::close();
}


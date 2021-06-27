#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "AddGoodItem.h"

int loginUserID;
int loginUserRole;
QString loginUserName;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化各个表格
    setupTables();

    // 绑定按钮事件
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(onSearchButtonClick()));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(onAddButtonClick()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClick()));
    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(onEditButtonClick()));
    connect(ui->clearFilterButton, SIGNAL(clicked()), this, SLOT(onClearFilterButtonClick()));


    // 绑定表格点击事件
    connect(ui->typeTable, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onApplyCategoryFilter(const QModelIndex&)));
}

/* 搜索按钮点击事件 (使用过滤条件更新 resultTable) */
void MainWindow::onSearchButtonClick()
{
    // 获取搜索框输入
    QString filter = ui->searchLineEdit->text();

    if (filter.length() == 0) {
        setupTables();
        return;
    }

    // 根据过滤条件获取查询
    QSqlQuery query;
    QSqlQueryModel* filterModel = new QSqlQueryModel;

    query.prepare("SELECT gid, gno, goods.cid, name, sku, title, subtitle, "
                  "inventory, restock_value, selling_value, length, width, height, weight, color "
                  "FROM goods "
                  "JOIN categories ON goods.cid = categories.cid "
                  "WHERE gno LIKE ? "
                  "OR title LIKE ? "
                  "OR subtitle LIKE ? "
                  "OR sku LIKE ? "
                  "ESCAPE '@'");

    // 转义字符串
    filter = filter.replace('@', "@@");
    filter = filter.replace('%', "@%");
    filter = filter.replace('_', "@%");
    filter = "%" + filter + "%";

    for (int i = 0; i < 4; i++)
        query.addBindValue(filter);

    if (!query.exec()) {
        qDebug() << "Search query failed.";
        qDebug() << query.lastError();
        return;
    }

    filterModel->setQuery(query);
    refreshResultTable(filterModel);
}

/* 添加按钮点击事件 */
void MainWindow::onAddButtonClick()
{
    AddGoodItem* addItemPage = new AddGoodItem(this);

    // 监听数据库信息变动事件
    connect(addItemPage, SIGNAL(tableChanged()), this, SLOT(onTableChanged()));

    addItemPage->show();
}

/* 编辑按钮点击事件 */
void MainWindow::onEditButtonClick()
{
    if (!ui->resultTable->selectionModel()->hasSelection()) {
        createMessageBox("需要至少从列表中选择一个商品来编辑！");
        return;
    }

    // 获取 TableView 所选行
    QModelIndexList  selection = ui->resultTable->selectionModel()->selectedRows();

    // 获取商品 ID
    int rowId = selection.at(0).row();
    int gid = ui->resultTable->model()->data(ui->resultTable->model()->index(rowId, 0)).toInt();

    // 进入编辑窗口
    AddGoodItem* addItemPage = new AddGoodItem(this, "modify", gid);

    // 监听数据库信息变动事件
    connect(addItemPage, SIGNAL(tableChanged()), this, SLOT(onTableChanged()));

    addItemPage->show();
}

/* 删除按钮点击事件 */
void MainWindow::onDeleteButtonClick()
{
    if (!ui->resultTable->selectionModel()->hasSelection()) {
        createMessageBox("需要至少从列表中选择一个商品来编辑！");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "确认删除", "删除商品会连带删除所有与该商品相关的进货、售货记录和图片，是否删除？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    // 获取 TableView 所选行
    QModelIndexList  selection = ui->resultTable->selectionModel()->selectedRows();

    // 获取商品 ID
    bool result = true;
    for (int i = 0; i < selection.count(); i++) {
        int rowId = selection.at(i).row();
        int gid = ui->resultTable->model()->data(ui->resultTable->model()->index(rowId, 0)).toInt();

        // 删除与该商品相关的所有信息
        QSqlQuery query;
        std::vector<QString> tables = {
            "discounts",
            "goods_logs",
            "goods_sellouts",
            "pictures",
            "goods"
        };
        for (auto table : tables) {
            query.prepare("DELETE FROM " + table + " WHERE gid = ?");
            query.addBindValue(gid);
            if (!query.exec()) {
                result = false;
                qDebug() << query.lastError();
            }
        }
    }

    if (result)
        createMessageBox("删除商品成功！");
    onSearchButtonClick();              // 直接使用搜索过滤的方法更新 resultTable
}

/* 清除过滤条件 */
void MainWindow::onClearFilterButtonClick()
{
    ui->typeTable->clearSelection();
    setupTables();
}

/* 应用分类过滤条件 */
void MainWindow::onApplyCategoryFilter(const QModelIndex& index)
{
    // 获取 TableView 所选行
    QModelIndexList  selection = ui->typeTable->selectionModel()->selectedRows();

    // 获取分类名
    std::vector<QString> filterList;
    for (int i = 0; i < selection.count(); i++) {
        int rowId = selection.at(i).row();
        QString categoryName = ui->typeTable->model()->data(ui->typeTable->model()->index(rowId, 0)).toString();
        filterList.push_back(categoryName);
    }

    // 更新 resultTable
    // 选出所有选中分类的数据
    QSqlQuery query;
    QString queryString = "SELECT gid, gno, goods.cid, name, sku, title, subtitle, "
                          "inventory, restock_value, selling_value, length, width, height, weight, color "
                          "FROM goods "
                          "JOIN categories ON goods.cid = categories.cid "
                          "WHERE name IN (";
    // 生成并填写占位符
    for (int i = 0; i < (int)filterList.size(); i++) {
        if (i != (int)filterList.size() - 1)
            queryString = queryString + "?, ";
        else
            queryString = queryString + "?) ";
    }
    query.prepare(queryString);
    for (int i = 0; i < (int)filterList.size(); i++)
        query.addBindValue(filterList[i]);

    // 更新表格
    QSqlQueryModel* filterModel = new QSqlQueryModel;
    if (!query.exec()) {
        qDebug() << "Search query failed.";
        qDebug() << query.lastError();
        return;
    }

    filterModel->setQuery(query);
    refreshResultTable(filterModel);
}

/* 表格更新信号槽 */
void MainWindow::onTableChanged()
{
    setupTables();
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*

以下几个函数都是对各自分管的表的刷新操作，执行前必须先 new QSqlQueryModel，
记为model，然后指明选择语句，将查询结果存在model，model作为下面函数的参数
以刷新各个表。

*/
void MainWindow::refreshTypeTable(QSqlQueryModel *model)
{
    //提交数据并显示
    model->setHeaderData(0,Qt::Horizontal,tr("分类名称"));
    ui->typeTable->setModel(model);
    ui->typeTable->show();
}

void MainWindow::refreshResultTable(QSqlQueryModel *model)
{
    model->setHeaderData(0,Qt::Horizontal,tr("商品ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("商品编号"));
    model->setHeaderData(2,Qt::Horizontal,tr("分类ID"));
    model->setHeaderData(3,Qt::Horizontal,tr("分类名称"));
    model->setHeaderData(4,Qt::Horizontal,tr("规格"));
    model->setHeaderData(5,Qt::Horizontal,tr("商品名"));
    model->setHeaderData(6,Qt::Horizontal,tr("商品描述"));
    model->setHeaderData(7,Qt::Horizontal,tr("库存"));
    model->setHeaderData(8,Qt::Horizontal,tr("进价"));
    model->setHeaderData(9,Qt::Horizontal,tr("售价"));
    model->setHeaderData(10,Qt::Horizontal,tr("长"));
    model->setHeaderData(11,Qt::Horizontal,tr("宽"));
    model->setHeaderData(12,Qt::Horizontal,tr("高"));
    model->setHeaderData(13,Qt::Horizontal,tr("重量"));
    model->setHeaderData(14,Qt::Horizontal,tr("颜色"));
    ui->resultTable->setModel(model);
    ui->resultTable->show();

    ui->resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);           // 设置每次选中一整行
    ui->resultTable->verticalHeader()->hide();                                      // 设置不显示行号
}

void MainWindow::refreshTypeManagerTable(QSqlQueryModel *model)
{
    model->setHeaderData(0,Qt::Horizontal,tr("分类ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("分类名称"));
    model->setHeaderData(2,Qt::Horizontal,tr("该分类商品数"));
    model->setHeaderData(3,Qt::Horizontal,tr("总持货数量"));
    model->setHeaderData(4,Qt::Horizontal,tr("均价"));

    ui->typeManagerTable->setModel(model);
    ui->typeManagerTable->show();
    ui->typeManagerTable->setColumnWidth(1, 250);
    ui->typeManagerTable->verticalHeader()->hide();
}

void MainWindow::refreshUserTable(QSqlQueryModel *model)
{
    model->setHeaderData(0,Qt::Horizontal,tr("用户ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("用户名"));
    model->setHeaderData(2,Qt::Horizontal,tr("权限"));
    model->setHeaderData(3,Qt::Horizontal,tr("电话"));
    model->setHeaderData(4,Qt::Horizontal,tr("邮箱"));
    model->setHeaderData(5,Qt::Horizontal,tr("上次登陆"));
    ui->userTable->setModel(model);
    ui->userTable->show();
}

void MainWindow::setupTables()
{
    //查询数据库中种类信息并刷新typeTable
    QSqlQueryModel *modelTypeFirst = new QSqlQueryModel;
    modelTypeFirst->setQuery("select name from goods,categories where goods.cid = categories.cid group by goods.cid");
    refreshTypeTable(modelTypeFirst);

    //查询goods表中各信息并刷新resultTable
    QSqlQueryModel *modelResultFirst = new QSqlQueryModel;
    modelResultFirst->setQuery("select gid,gno,goods.cid,categories.name,sku,title,subtitle,inventory,restock_value,selling_value,length,width,height,weight,color from goods,categories where goods.cid = categories.cid");
    refreshResultTable(modelResultFirst);

    //查询种类信息并刷新typeManagerTable
    QSqlQueryModel *modelTypeManagerFirst = new QSqlQueryModel;
    modelTypeManagerFirst->setQuery("select goods.cid,name,count(*),sum(inventory),avg(selling_value) from goods,categories where goods.cid = categories.cid group by goods.cid");
    refreshTypeManagerTable(modelTypeManagerFirst);

    //查询用户信息并刷新userTable
    QSqlQueryModel *modelUserFirst = new QSqlQueryModel;
    modelUserFirst->setQuery("select uid,username,role,phone,email,last_login from users");
    refreshUserTable(modelUserFirst);
}

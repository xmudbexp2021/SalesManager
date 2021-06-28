#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "AddGoodItem.h"
#include "GoodDetail.h"
#include "UserEdit.h"
#include "SellPage.h"
#include "loginwindow.h"
#include "DiscountEdit.h"

int loginUserID = -1;
int loginUserRole = -1;
QString loginUserName = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化各个表格
    setupTables();

    // 绑定按钮事件
    /// 商品管理页面
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(onSearchButtonClick()));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(onAddButtonClick()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClick()));
    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(onEditButtonClick()));
    connect(ui->clearFilterButton, SIGNAL(clicked()), this, SLOT(onClearFilterButtonClick()));
    connect(ui->detailButton, SIGNAL(clicked()), this, SLOT(onDetailButtonClick()));
    connect(ui->stockButton, SIGNAL(clicked()), this, SLOT(onStockButtonClick()));
    connect(ui->sellButton, SIGNAL(clicked()), this, SLOT(onSellButtonClick()));
    connect(ui->createDiscountButton, SIGNAL(clicked()), this, SLOT(onCreateDiscountButtonClick()));

    /// 分类管理 页面
    connect(ui->newCategoryButton, SIGNAL(clicked()), this, SLOT(onNewCategoryButtonClick()));
    connect(ui->alterCategoryButton, SIGNAL(clicked()), this, SLOT(onAlterCategoryButtonClick()));
    connect(ui->dropCategoryButton, SIGNAL(clicked()), this, SLOT(onDropCategoryButtonClick()));

    /// 用户管理 页面
    connect(ui->createUserButton, SIGNAL(clicked()), this, SLOT(onCreateUserButtonClick()));
    connect(ui->modifyUserButton, SIGNAL(clicked()), this, SLOT(onModifyUserButtonClick()));
    connect(ui->modifyPermissionButton, SIGNAL(clicked()), this, SLOT(onModifyUserButtonClick()));
    connect(ui->deleteUserButton, SIGNAL(clicked()), this, SLOT(onDeleteUserButtonClick()));

    /// 售出信息 & 库存信息 页面
    connect(ui->deleteSelloutButton, SIGNAL(clicked()), this, SLOT(onDeleteSelloutButtonClick()));
    connect(ui->deleteLogButton, SIGNAL(clicked()), this, SLOT(onDeleteLogButtonClick()));

    /// 折扣管理页面
    connect(ui->modifyDiscountButton, SIGNAL(clicked()), this, SLOT(onModifyDiscountButtonClick()));
    connect(ui->deleteDiscountButton, SIGNAL(clicked()), this, SLOT(onDeleteDiscountButtonClick()));

    /// 菜单
    connect(ui->menuLogout, SIGNAL(aboutToShow()), this, SLOT(logout()));
    connect(ui->menuExit, SIGNAL(aboutToShow()), this, SLOT(exit()));

    // 绑定表格点击事件
    connect(ui->typeTable, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onApplyCategoryFilter(const QModelIndex&)));

    // 动态绑定属性
    ui->menuCurrentUser->setTitle("当前登录用户：" + loginUserName);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
* 商品信息页面的逻辑和事件回调函数等
*/
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

    query.prepare("SELECT gid, gno, title, name, sku, subtitle, "
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
    if (!hasPermission(loginUserID, "goods", "insert")) {
        createMessageBox("您无权添加商品！");
        return;
    }

    AddGoodItem* addItemPage = new AddGoodItem(this);

    // 监听数据库信息变动事件
    connect(addItemPage, SIGNAL(tableChanged()), this, SLOT(onTableChanged()));

    addItemPage->show();
}

/* 编辑按钮点击事件 */
void MainWindow::onEditButtonClick()
{
    if (!hasPermission(loginUserID, "goods", "edit")) {
        createMessageBox("您无权编辑商品信息！");
        return;
    }

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
    if (!hasPermission(loginUserID, "goods", "delete")) {
        createMessageBox("您无权删除商品！");
        return;
    }

    if (!ui->resultTable->selectionModel()->hasSelection()) {
        createMessageBox("需要至少从列表中选择一个商品来删除！");
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

void MainWindow::onDetailButtonClick()
{
    if (!ui->resultTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中选择一个商品来查看详情！");
        return;
    }

    // 获取 TableView 所选行
    QModelIndexList  selection = ui->resultTable->selectionModel()->selectedRows();

    // 获取商品 ID
    int rowId = selection.at(0).row();
    int gid = ui->resultTable->model()->data(ui->resultTable->model()->index(rowId, 0)).toInt();

    GoodDetail *page = new GoodDetail(this, gid);
    page->show();
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
    QString queryString = "SELECT gid, gno, title, name, sku, subtitle, "
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

/* 商品入库点击事件 */
void MainWindow::onStockButtonClick()
{
    if (!hasPermission(loginUserID, "goods_logs", "insert")) {
        createMessageBox("您无权执行入库操作！");
        return;
    }
    if (!ui->resultTable->selectionModel()->hasSelection()) {
        createMessageBox("需要至少从列表中选择一个商品来入库！");
        return;
    }
    bool isOK, result = false;
    int stockNumb = QInputDialog::getInt(NULL,"入库信息","输入入库数量",QLineEdit::Normal,0,30000,1, &isOK);
    if (isOK && stockNumb <= 0){
        createMessageBox("请输入正确的入库数");
        return;
    }
    if(isOK)
    {
        QModelIndexList selection = ui->resultTable->selectionModel()->selectedRows();
        int rowId = selection.at(0).row();
        int gid = ui->resultTable->model()->data(ui->resultTable->model()->index(rowId,0)).toInt();
        QSqlQuery queryLog;
        queryLog.prepare("insert into goods_logs(gid, uid, type, number, time) values(?, ?, ?, ?, CURRENT_TIMESTAMP)");
        queryLog.addBindValue(gid);
        queryLog.addBindValue(loginUserID);
        queryLog.addBindValue(1);
        queryLog.addBindValue(stockNumb);
        if (!queryLog.exec()) {
            result = false;
            qDebug() << queryLog.lastError();
        }
        result = true;
    }
    if(result)
        createMessageBox("商品入库成功！");
    setupTables();
}

/* 商品售卖点击事件 */
void MainWindow::onSellButtonClick()
{
    if (!hasPermission(loginUserID, "goods_sellouts", "insert")) {
        createMessageBox("您无权执行售卖操作！");
        return;
    }
    if (!ui->resultTable->selectionModel()->hasSelection()) {
        createMessageBox("需要至少从列表中选择一个商品来售卖！");
        return;
    }
    QModelIndexList selection = ui->resultTable->selectionModel()->selectedRows();
    int rowId = selection.at(0).row();
    int gid = ui->resultTable->model()->data(ui->resultTable->model()->index(rowId,0)).toInt();
    sellPage* inputSellInfo = new sellPage(this,gid);
    connect(inputSellInfo,SIGNAL(tableChanged()),this,SLOT(onTableChanged()));
    inputSellInfo->show();
}

/* 表格更新信号槽 */
void MainWindow::onTableChanged()
{
    setupTables();
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
    ui->typeTable->verticalHeader()->hide();
    ui->typeTable->show();
}

void MainWindow::refreshResultTable(QSqlQueryModel *model)
{
    model->setHeaderData(0,Qt::Horizontal,tr("ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("商品编号"));
    model->setHeaderData(2,Qt::Horizontal,tr("商品名"));
    model->setHeaderData(3,Qt::Horizontal,tr("分类名称"));
    model->setHeaderData(4,Qt::Horizontal,tr("规格"));
    model->setHeaderData(5,Qt::Horizontal,tr("商品描述"));
    model->setHeaderData(6,Qt::Horizontal,tr("库存"));
    model->setHeaderData(7,Qt::Horizontal,tr("进价"));
    model->setHeaderData(8,Qt::Horizontal,tr("售价"));
    model->setHeaderData(9,Qt::Horizontal,tr("长"));
    model->setHeaderData(10,Qt::Horizontal,tr("宽"));
    model->setHeaderData(11,Qt::Horizontal,tr("高"));
    model->setHeaderData(12,Qt::Horizontal,tr("重量"));
    model->setHeaderData(13,Qt::Horizontal,tr("颜色"));
    ui->resultTable->setModel(model);

    // 设置某些列的默认长度
    ui->resultTable->setColumnWidth(0, 50);
    ui->resultTable->setColumnWidth(2, 250);
    ui->resultTable->setColumnWidth(5, 200);

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
    ui->typeManagerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->typeManagerTable->verticalHeader()->hide();
}

void MainWindow::refreshUserTable(QSqlQueryModel *model)
{
    model->setHeaderData(0,Qt::Horizontal,tr("用户ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("用户名"));
    model->setHeaderData(2,Qt::Horizontal,tr("用户角色"));
    model->setHeaderData(3,Qt::Horizontal,tr("联系电话"));
    model->setHeaderData(4,Qt::Horizontal,tr("联系邮箱"));
    model->setHeaderData(5,Qt::Horizontal,tr("上次登陆"));
    ui->userTable->setModel(model);
    ui->userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->userTable->setColumnWidth(1, 100);
    ui->userTable->setColumnWidth(2, 150);
    ui->userTable->setColumnWidth(3, 150);
    ui->userTable->setColumnWidth(4, 150);
    ui->userTable->setColumnWidth(5, 200);
    ui->userTable->verticalHeader()->hide();
    ui->userTable->show();
}

void MainWindow::refreshSaleTable(QSqlQueryModel *model)
{
    model->setHeaderData(0,Qt::Horizontal,tr("售货编号"));
    model->setHeaderData(1,Qt::Horizontal,tr("商品ID"));
    model->setHeaderData(2,Qt::Horizontal,tr("操作用户ID"));
    model->setHeaderData(3,Qt::Horizontal,tr("卖出数量"));
    model->setHeaderData(4,Qt::Horizontal,tr("卖出价格"));
    model->setHeaderData(5,Qt::Horizontal,tr("顾客姓名"));
    model->setHeaderData(6,Qt::Horizontal,tr("联系方式"));
    model->setHeaderData(7,Qt::Horizontal,tr("卖出时间"));
    ui->saleTable->setModel(model);
    ui->saleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->saleTable->verticalHeader()->hide();
    ui->saleTable->show();
}

void MainWindow::refreshStockTable(QSqlQueryModel *model)
{
    model->setHeaderData(0, Qt::Horizontal, tr("记录ID"));
    model->setHeaderData(1, Qt::Horizontal,tr("商品ID"));
    model->setHeaderData(2, Qt::Horizontal,tr("商品名称"));
    model->setHeaderData(3, Qt::Horizontal,tr("操作类型"));
    model->setHeaderData(4, Qt::Horizontal,tr("操作数量"));
    model->setHeaderData(5, Qt::Horizontal,tr("操作用户"));
    model->setHeaderData(6, Qt::Horizontal,tr("操作时间"));
    ui->stockTable->setModel(model);
    ui->stockTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->stockTable->verticalHeader()->hide();
    ui->stockTable->show();
}


void MainWindow::refreshDiscountTable(QSqlQueryModel *model)
{
    model->setHeaderData(0, Qt::Horizontal, tr("折扣记录ID"));
    model->setHeaderData(1, Qt::Horizontal,tr("商品ID"));
    model->setHeaderData(2, Qt::Horizontal,tr("商品名称"));
    model->setHeaderData(3, Qt::Horizontal,tr("操作用户"));
    model->setHeaderData(4, Qt::Horizontal,tr("开始时间"));
    model->setHeaderData(5, Qt::Horizontal,tr("结束时间"));
    model->setHeaderData(6, Qt::Horizontal,tr("折扣价格"));
    ui->discountTable->setModel(model);
    ui->discountTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->discountTable->verticalHeader()->hide();
    ui->discountTable->show();
}

void MainWindow::setupTables()
{
    //查询数据库中种类信息并刷新typeTable
    if (hasPermission(loginUserID, "categories", "select")) {
        QSqlQueryModel *modelTypeFirst = new QSqlQueryModel;
        modelTypeFirst->setQuery("select name from categories");
        refreshTypeTable(modelTypeFirst);
    }

    //查询goods表中各信息并刷新resultTable
    if (hasPermission(loginUserID, "goods", "select")) {
        QSqlQueryModel *modelResultFirst = new QSqlQueryModel;
        modelResultFirst->setQuery("select gid, gno, title, name, sku, subtitle, "
                                   "inventory, restock_value, selling_value, "
                                   "length, width, height, weight, color "
                                  "from goods  left join categories on goods.cid = categories.cid");
        refreshResultTable(modelResultFirst);
    }

    //查询种类信息并刷新typeManagerTable
    if (hasPermission(loginUserID, "categories", "select")) {
        QSqlQueryModel *modelTypeManagerFirst = new QSqlQueryModel;
        modelTypeManagerFirst->setQuery("select cid, name, "
                                        "(select count(*) from goods where goods.cid = categories.cid) as count, "
                                        "(select sum(inventory) from goods where goods.cid = categories.cid) as sum, "
                                        "(select avg(selling_value) from goods where goods.cid = categories.cid) as avg "
                                        "from categories "
                                        "order by cid asc");
        refreshTypeManagerTable(modelTypeManagerFirst);
    }

    //查询用户信息并刷新userTable
    if (hasPermission(loginUserID, "users", "select")) {
        QSqlQueryModel *modelUserFirst = new QSqlQueryModel;
        modelUserFirst->setQuery("select uid, username, "
                                "case role "
                                    "when 0 then '系统管理员' "
                                    "when 1 then '库存经理' "
                                    "when 2 then '人力资源经理' "
                                    "when 3 then '普通用户' end as roleStr, "
                                "phone, email, last_login from users");
        refreshUserTable(modelUserFirst);
    }

    //查询售卖信息并刷新 saleTable
    if (hasPermission(loginUserID, "goods_sellouts", "select")) {
        QSqlQueryModel *modelSaleFirst = new QSqlQueryModel;
        modelSaleFirst->setQuery("select * from goods_sellouts");
        refreshSaleTable(modelSaleFirst);
    }

    //查询库存信息并刷新 stockTable
    if (hasPermission(loginUserID, "goods_logs", "select")) {
        QSqlQueryModel *modelStockFirst = new QSqlQueryModel;
        modelStockFirst->setQuery("select log_id, goods_logs.gid,title,case type when 1 then '进货' when 2 then '上架' when 3 then '下架' end,number,username,time from goods,goods_logs,users where goods.gid = goods_logs.gid and goods_logs.uid = users.uid");
        refreshStockTable(modelStockFirst);
    }

    // 查询折扣信息并刷新 discountTable
    if (hasPermission(loginUserID, "discounts", "select"))  {
        QSqlQueryModel *modelDiscount = new QSqlQueryModel;
        modelDiscount->setQuery("SELECT did, discounts.gid, title, username, start_time, end_time, discount "
                                "FROM discounts "
                                "JOIN goods ON discounts.gid = goods.gid "
                                "JOIN users ON discounts.uid = users.uid");
        refreshDiscountTable(modelDiscount);
    }
}


/**
 * 分类管理页面事件回调函数
 */
/* 新建分类 */
void MainWindow::onNewCategoryButtonClick()
{
    if (!hasPermission(loginUserID, "categories", "insert")) {
        createMessageBox("您无权新建分类！");
        return;
    }

    bool isOK;
    QString categoryName = QInputDialog::getText(NULL, "新建分类", "请输入新分类的名称", QLineEdit::Normal, "", &isOK);

    if (isOK) {
        QSqlQuery query;
        query.prepare("INSERT INTO categories (name, number) VALUES (?, ?)");
        query.addBindValue(categoryName);
        query.addBindValue(0);

        if (!query.exec()) {
            qDebug() << query.lastError();
            return;
        }
        createMessageBox("创建新商品分类成功！");
        setupTables();
    }
}

/* 编辑分类 */
void MainWindow::onAlterCategoryButtonClick()
{
    if (!ui->typeManagerTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中选择一个类来修改类信息！");
        return;
    }
    if (!hasPermission(loginUserID, "categories", "update")) {
        createMessageBox("您无权编辑分类！");
        return;
    }

    // 获取 TableView 所选行
    QModelIndexList selection = ui->typeManagerTable->selectionModel()->selectedRows();

    // 获取分类 ID
    int rowId = selection.at(0).row();
    int cid = ui->typeManagerTable->model()->data(ui->typeManagerTable->model()->index(rowId, 0)).toInt();

    bool isOK;
    QString categoryName = QInputDialog::getText(NULL, "修改分类名称", "请输入修改后的分类名称", QLineEdit::Normal, "", &isOK);

    if (isOK) {
        QSqlQuery query;
        query.prepare("UPDATE categories SET name = ? WHERE cid = ?");
        query.addBindValue(categoryName);
        query.addBindValue(cid);

        if (!query.exec()) {
            qDebug() << query.lastError();
            return;
        }
        createMessageBox("修改商品分类信息成功！");
        setupTables();
    }
}

/* 删除分类 */
void MainWindow::onDropCategoryButtonClick()
{
    if (!ui->typeManagerTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中选择一个类删除！");
        return;
    }
    if (!hasPermission(loginUserID, "categories", "delete")) {
        createMessageBox("您无权删除分类！");
        return;
    }

    // 获取 TableView 所选行
    QModelIndexList selection = ui->typeManagerTable->selectionModel()->selectedRows();

    // 获取分类 ID
    int rowId = selection.at(0).row();
    int cid = ui->typeManagerTable->model()->data(ui->typeManagerTable->model()->index(rowId, 0)).toInt();

    if (cid == 0) {
        createMessageBox("无法删除默认分类！");
        return;
    }

    QMessageBox::StandardButton reply =  QMessageBox::question(NULL,
        "删除分类确认", "确定要删除分类吗？该分类下原有的所有商品将被归类到 “默认分类” 下。", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("UPDATE goods SET cid = 0 WHERE cid = ?");
        query.addBindValue(cid);
        if (!query.exec())
            qDebug() << query.lastError();
        query.prepare("DELETE FROM categories WHERE cid = ?");
        query.addBindValue(cid);
        if (!query.exec())
            qDebug() << query.lastError();
        createMessageBox("分类已删除！");
        setupTables();
    }
}


/**
 * 用户管理 / 权限管理页面
 */
void MainWindow::onCreateUserButtonClick()
{
    if (!hasPermission(loginUserID, "users", "insert")) {
        createMessageBox("您无权新建用户！");
        return;
    }

    UserEdit *userEditPage = new UserEdit(this, -1, "add");
    connect(userEditPage, SIGNAL(tableChanged()), this, SLOT(onTableChanged()));
    userEditPage->show();
}

void MainWindow::onModifyUserButtonClick()
{
    if (!hasPermission(loginUserID, "users", "update")) {
        createMessageBox("您无权修改用户！");
        return;
    }
    if (!ui->userTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中选择一个用户操作。");
        return;
    }

    QModelIndexList selection = ui->userTable->selectionModel()->selectedRows();
    int rowId = selection.at(0).row();
    int uid = ui->userTable->model()->data(ui->userTable->model()->index(rowId, 0)).toInt();

    UserEdit *userEditPage = new UserEdit(this, uid);
    connect(userEditPage, SIGNAL(tableChanged()), this, SLOT(onTableChanged()));
    userEditPage->show();
}

void MainWindow::onDeleteUserButtonClick()
{
    if (!ui->userTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中选择一个用户删除！");
        return;
    }

    if (!hasPermission(loginUserID, "users", "delete") != 1) {
        createMessageBox("您无权删除用户！");
        return;
    }

    QModelIndexList selection = ui->userTable->selectionModel()->selectedRows();

    int rowId = selection.at(0).row();
    int uid = ui->userTable->model()->data(ui->userTable->model()->index(rowId, 0)).toInt();

    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE uid = ?");
    query.addBindValue(uid);
    if (!query.exec() || !query.next())
        qDebug() << query.lastError();

    if ((query.value(0).toInt() == 0 && loginUserID != 1) || uid == 1) {
        createMessageBox("您无权删除管理员用户！");
        return;
    }

    QMessageBox::StandardButton reply =  QMessageBox::question(NULL,
        "删除用户确认", "确定要删除用户吗？删除用户会同时删除用户创建的所有商品和售卖记录等。", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        query.prepare("DELETE FROM users WHERE uid = ?");
        query.addBindValue(uid);
        if (!query.exec())
            qDebug() << query.lastError();
        createMessageBox("删除用户成功！");

        setupTables();
    }
}

void MainWindow::onDeleteSelloutButtonClick()
{
    if (!hasPermission(loginUserID, "goods_sellouts", "delete")) {
        createMessageBox("您无权删除记录！");
        return;
    }
    if (!ui->saleTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中至少选择一条记录删除！");
        return;
    }

    QModelIndexList selection = ui->saleTable->selectionModel()->selectedRows();
    QSqlQuery query;
    for (int i = 0; i < selection.count(); i++) {
        int rowId = selection.at(i).row();
        int sid = ui->saleTable->model()->data(ui->saleTable->model()->index(rowId, 0)).toInt();
        query.prepare("DELETE FROM goods_sellouts WHERE sid = ?");
        query.addBindValue(sid);
        if (!query.exec())
            qDebug() << query.lastError();
    }

    createMessageBox("删除记录成功！");
    setupTables();
}

void MainWindow::onDeleteLogButtonClick()
{
    if (!hasPermission(loginUserID, "goods_logs", "delete")) {
        createMessageBox("您无权删除记录！");
        return;
    }
    if (!ui->stockTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中至少选择一条记录删除！");
        return;
    }

    QModelIndexList selection = ui->stockTable->selectionModel()->selectedRows();
    QSqlQuery query;
    for (int i = 0; i < selection.count(); i++) {
        int rowId = selection.at(i).row();
        int sid = ui->stockTable->model()->data(ui->saleTable->model()->index(rowId, 0)).toInt();
        query.prepare("DELETE FROM goods_sellouts WHERE sid = ?");
        query.addBindValue(sid);
        if (!query.exec())
            qDebug() << query.lastError();
    }

    createMessageBox("删除记录成功！");
    setupTables();
}

/**
 * 折扣页面
 */
void MainWindow::onCreateDiscountButtonClick()
{
    if (!hasPermission(loginUserID, "discounts", "insert")) {
        createMessageBox("您无权添加折扣信息！");
        return;
    }
    if (!ui->resultTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中选择一个商品来添加折扣！");
        return;
    }
    QModelIndexList selection = ui->resultTable->selectionModel()->selectedRows();
    int rowId = selection.at(0).row();
    int gid = ui->resultTable->model()->data(ui->resultTable->model()->index(rowId, 0)).toInt();

    DiscountEdit* discountPage = new DiscountEdit(this, "add", -1, gid);
    connect(discountPage, SIGNAL(tableChanged()), this, SLOT(onTableChanged()));
    discountPage->show();
}

void MainWindow::onModifyDiscountButtonClick()
{
    if (!hasPermission(loginUserID, "discounts", "update")) {
        createMessageBox("您无权编辑折扣信息！");
        return;
    }
    if (!ui->discountTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中选择一条记录编辑！");
        return;
    }
    QModelIndexList selection = ui->discountTable->selectionModel()->selectedRows();
    int rowId = selection.at(0).row();
    int did = ui->discountTable->model()->data(ui->discountTable->model()->index(rowId, 0)).toInt();

    DiscountEdit* discountPage = new DiscountEdit(this, "modify", did, -1);
    connect(discountPage, SIGNAL(tableChanged()), this, SLOT(onTableChanged()));
    discountPage->show();
}

void MainWindow::onDeleteDiscountButtonClick()
{
    if (!hasPermission(loginUserID, "discounts", "delete")) {
        createMessageBox("您无权删除折扣信息！");
        return;
    }
    if (!ui->discountTable->selectionModel()->hasSelection()) {
        createMessageBox("需要从列表中选择一条记录删除！");
        return;
    }
    QModelIndexList selection = ui->discountTable->selectionModel()->selectedRows();
    int rowId = selection.at(0).row();
    int did = ui->discountTable->model()->data(ui->discountTable->model()->index(rowId, 0)).toInt();

    QSqlQuery query;
    query.prepare("DELETE FROM discounts WHERE did = ?");
    query.addBindValue(did);
    if (!query.exec()) {
        qDebug() << query.lastError();
        return;
    }
    createMessageBox("折扣记录删除成功！");
    setupTables();
}

// 注销当前用户
void MainWindow::logout()
{
    loginUserID = -1;
    loginUserRole = -1;
    loginUserName = "";

    LoginWindow *login = new LoginWindow;
    login->show();
    QWidget::close();
}

// 退出程序
void MainWindow::exit()
{
    app->exit(0);
}

#include "MainWindow.h"
#include "ui_MainWindow.h"

int loginUserID;
int loginUserRole;
QString loginUserName;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //初始化各个表格
    setupTables();
    //绑定按钮事件
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(onSearchButtonClick()));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(onAddButtonClick()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClick()));

}

void MainWindow::onSearchButtonClick()
{

}

void MainWindow::onAddButtonClick()
{

}

void MainWindow::onDeleteButtonClick()
{

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
    model->setHeaderData(0,Qt::Horizontal,tr("商品类型"));
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
}

void MainWindow::refreshTypeManagerTable(QSqlQueryModel *model)
{
    model->setHeaderData(0,Qt::Horizontal,tr("分类ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("商品名"));
    model->setHeaderData(2,Qt::Horizontal,tr("该分类商品数"));
    model->setHeaderData(3,Qt::Horizontal,tr("总持货数量"));
    model->setHeaderData(4,Qt::Horizontal,tr("均价"));
    ui->typeManagerTable->setModel(model);
    ui->typeManagerTable->show();
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

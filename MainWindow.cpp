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
    //刷新类型表
    //查询数据库中种类信息
    //创建QSqlQueryModel实例model记录查询信息
    QSqlQueryModel *modelTypeFirst = new QSqlQueryModel;
    modelTypeFirst->setQuery("select name from goods,categories where goods.cid = categories.cid group by goods.cid");
    refreshTypeTable(modelTypeFirst);
    QSqlQueryModel *modelResultFirst = new QSqlQueryModel;
    modelResultFirst->setQuery("select *,categories.name from goods,categories where goods.cid = categories.cid");
    refreshResultTable(modelResultFirst);
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

void MainWindow::refreshTypeTable(QSqlQueryModel *model)
{
    //提交数据并显示
    model->setHeaderData(0,Qt::Horizontal,tr("类型"));
    ui->typeTable->setModel(model);
    ui->typeTable->show();
}

void MainWindow::refreshResultTable(QSqlQueryModel *model)
{
    ui->resultTable->setModel(model);
    ui->resultTable->show();
}


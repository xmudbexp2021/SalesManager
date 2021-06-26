#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "application.h"

int loginUserID;
int loginUserRole;
QString loginUserName;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //查询数据库中种类信息
    //创建QSqlQueryModel实例model记录查询信息
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("select cid from goods");
    model->setHeaderData(0,Qt::Horizontal,tr("cid"));
    //提交数据并显示
    ui->typeTable->setModel(model);
    ui->typeTable->show();

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

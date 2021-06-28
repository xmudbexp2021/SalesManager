#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "application.h"

#include "AddGoodItem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void refreshTypeTable(QSqlQueryModel *model);
    void refreshResultTable(QSqlQueryModel *model);
    void refreshTypeManagerTable(QSqlQueryModel *model);
    void refreshUserTable(QSqlQueryModel *model);
    void refreshSaleTable(QSqlQueryModel *model);
    void refreshStockTable(QSqlQueryModel *model);
    void refreshDiscountTable(QSqlQueryModel *model);
    void setupTables();

private:
    Ui::MainWindow *ui;

public slots:
    // 商品管理主页面
    void onSearchButtonClick();        // 搜索按钮点击事件
    void onAddButtonClick();           // 添加商品按钮点击事件
    void onDeleteButtonClick();        // 删除按钮点击事件
    void onEditButtonClick();          // 编辑按钮点击事件
    void onClearFilterButtonClick();   // 清除过滤器点击事件
    void onDetailButtonClick();        // 详情按钮点击事件
    void onApplyCategoryFilter(const QModelIndex& index);       // 点击类别的 TableView，应用分类器
    void onTableChanged();             // 表格数据变更，触发重新渲染
    void onStockButtonClick();        //入库按钮点击事件
    void onSellButtonClick();

    // 分类管理页面
    void onNewCategoryButtonClick();
    void onAlterCategoryButtonClick();
    void onDropCategoryButtonClick();

    // 用户管理页面
    void onCreateUserButtonClick();
    void onModifyUserButtonClick();
    void onDeleteUserButtonClick();

    // 售卖信息页面
    void onDeleteSelloutButtonClick();

    // 库存管理页面
    void onDeleteLogButtonClick();

    // 折扣页面
    void onCreateDiscountButtonClick();
    void onModifyDiscountButtonClick();
    void onDeleteDiscountButtonClick();

    // 菜单事件
    void logout();
    void exit();
};

#endif // MAINWINDOW_H

#ifndef ADDGOODITEM_H
#define ADDGOODITEM_H

#include <QMainWindow>
#include <QStandardItem>

using std::map;
using std::vector;

namespace Ui {
class AddGoodItem;
}

class AddGoodItem : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddGoodItem(QWidget *parent = nullptr,
                         QString action = "add",
                         int gid = 0);
    ~AddGoodItem();

private:
    Ui::AddGoodItem *ui;

    QString userAction;                 // 用户操作 [add, modify]
    QStandardItemModel *model;          // TableView 模型

    map<int, int> indexToCid;           // 将选项标号映射到分类 ID
    map<int, int> cidToIndex;           // 将分类 ID 映射到选项标号
    map<int, int> colToPid;             // TableView 列号 to PID
    vector<QStandardItem*> imagesList;  // 图片数组
    vector<QImage*> newImages;          // 新上传的图片

    void redrawImageTableView();        // 重绘图片的 TableView
    void fetchGoodInfo(int gid);        // 获取商品信息
    void getImages(int gid);            // 获取商品图片
    void processImageUpload(int gid);   // 处理当前的图片上传

signals:
    void tableChanged();

public slots:
    void onUploadButtonClick();         // 上传按钮点击事件
    void onDeleteButtonClick();         // 删除按钮点击事件
    void onSaveButtonClick();           // 保存按钮点击事件
    void onCancelButtonClick();         // 取消按钮点击事件
};

#endif // ADDGOODITEM_H

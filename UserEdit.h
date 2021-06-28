#ifndef USEREDIT_H
#define USEREDIT_H

#include <QMainWindow>

using std::vector;

namespace Ui {
class UserEdit;
}

class UserEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserEdit(QWidget *parent = nullptr, int uid = -1, QString action = "modify");
    ~UserEdit();

private:
    QString userAction;
    Ui::UserEdit *ui;

    int currentUid;

    // 权限域
    vector<QString> domain;

    void renderPermissionDomain();
    void renderPermissionType();
    void renderPermissionOperation();

    void refreshUserInfo();

signals:
    void tableChanged();

public slots:
    void onSelectionChange(int index);
    void onSavePermissionButtonClick();
    void onSaveButtonClick();
    void onCancelButtonClick();
};

#endif // USEREDIT_H

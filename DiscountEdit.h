#ifndef DISCOUNTEDIT_H
#define DISCOUNTEDIT_H

#include <QMainWindow>

namespace Ui {
class DiscountEdit;
}

class DiscountEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit DiscountEdit(QWidget *parent = nullptr,
                          QString action = "add",
                          int did = -1,
                          int gid = -1);
    ~DiscountEdit();

private:
    Ui::DiscountEdit *ui;
    int currentDid, currentGid;
    QString userAction;

    void fetchDiscountInfo();

signals:
    void tableChanged();

public slots:
    void onSaveButtonClick();
    void onCancelButtonClick();
};

#endif // DISCOUNTEDIT_H

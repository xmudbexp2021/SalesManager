#ifndef SELLPAGE_H
#define SELLPAGE_H

#include <QDialog>

namespace Ui {
class sellPage;
}

class sellPage : public QDialog
{
    Q_OBJECT

public:
    explicit sellPage(QWidget *parent = nullptr,int Qgid = 0);
    ~sellPage();
    void initMessage();

private:
    Ui::sellPage *ui;
    int gid;

signals:
    void tableChanged();

public slots:
    void onNextButtonClick();
    void onBackButtonClick();
};

#endif // SELLPAGE_H

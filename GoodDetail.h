#ifndef GOODDETAIL_H
#define GOODDETAIL_H

#include <QMainWindow>

namespace Ui {
class GoodDetail;
}

class GoodDetail : public QMainWindow
{
    Q_OBJECT

public:
    explicit GoodDetail(QWidget *parent = nullptr, int gid = -1);
    ~GoodDetail();

    void renderImage();

private:
    Ui::GoodDetail *ui;

    int imageIndex;
    std::vector<QPixmap> images;

public slots:
    void onPrevButtonClick();
    void onNextButtonClick();
    void onCloseButtonClick() { QWidget::close(); };
};

#endif // GOODDETAIL_H

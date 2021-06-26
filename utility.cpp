#include "application.h"

void createMessageBox(QString content)
{
    QMessageBox::information(NULL, "提示", content, QMessageBox::Yes, QMessageBox::Yes);
}

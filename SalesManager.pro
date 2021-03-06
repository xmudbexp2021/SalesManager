QT       += core gui sql concurrent
RC_ICONS = appIcon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AddGoodItem.cpp \
    DiscountEdit.cpp \
    GoodDetail.cpp \
    MainWindow.cpp \
    SQLThread.cpp \
    SellPage.cpp \
    ServerSetting.cpp \
    UserEdit.cpp \
    loginwindow.cpp \
    main.cpp \
    utility.cpp

HEADERS += \
    AddGoodItem.h \
    DiscountEdit.h \
    GoodDetail.h \
    MainWindow.h \
    SQLThread.h \
    SellPage.h \
    ServerSetting.h \
    UserEdit.h \
    application.h \
    loginwindow.h

FORMS += \
    AddGoodItem.ui \
    DiscountEdit.ui \
    GoodDetail.ui \
    MainWindow.ui \
    SellPage.ui \
    ServerSetting.ui \
    UserEdit.ui \
    loginwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.json

RESOURCES += \
    config.qrc \
    dropdown.qrc


QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addfriendalert.cpp \
    addfrienddialog.cpp \
    addgroupalert.cpp \
    alarmwidget.cpp \
    choosefile.cpp \
    chooseimage.cpp \
    filemeta.cpp \
    imagewin.cpp \
    itemwidget.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    registerdialog.cpp \
    tcpconnect.cpp \
    updateinfo.cpp \
    voice.cpp \
    voicealert.cpp

HEADERS += \
    addfriendalert.h \
    addfrienddialog.h \
    addgroupalert.h \
    alarmwidget.h \
    choosefile.h \
    chooseimage.h \
    filemeta.h \
    imagewin.h \
    information.h \
    itemwidget.h \
    json.hpp \
    logindialog.h \
    mainwindow.h \
    public.hpp \
    registerdialog.h \
    tcpconnect.h \
    updateinfo.h \
    voice.h \
    voicealert.h

FORMS += \
    addfriendalert.ui \
    addfrienddialog.ui \
    addgroupalert.ui \
    alarmwidget.ui \
    choosefile.ui \
    chooseimage.ui \
    imagewin.ui \
    itemwidget.ui \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    updateinfo.ui \
    voice.ui \
    voicealert.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rs.qrc

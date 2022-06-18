#include "mainwindow.h"
#include "logindialog.h"

#include <QApplication>
#include <QTextCodec>
#include <qdesktopwidget.h>

int main(int argc, char *argv[])
{
//    QTextCodec* codec = QTextCodec::codecForName( "UTF-8");//这一行以及下面3行根据主界面窗口决定要不要
//    QTextCodec::setCodecForLocale(codec);
//    QApplication a(argc, argv);
//    MainWindow w;
//    LoginDialog loginlg;
//    loginlg.setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);//设置窗口置顶，即：去除默认边框
//    loginlg.show();
//    w.resize(1500,750);
//    w.setAttribute(Qt::WA_DeleteOnClose);
//    QObject::connect(&loginlg,SIGNAL(showmain()),&w,SLOT(receiveLogin()));
//    w.move((QApplication::desktop()->width() - w.width()) / 2, (QApplication::desktop()->height() - w.height()) / 2);  // 窗口设置居中显示
//    return a.exec();
    QApplication a(argc,argv);
    MainWindow w;//执行构造函数，构造函数里初始化界面和设计登录界面
    if(w.getLoginStatus()==false){//用户未登录
        return 0;//退出程序
    }
    w.setAttribute(Qt::WA_DeleteOnClose);
    w.move((QApplication::desktop()->width() - w.width()) / 2, (QApplication::desktop()->height() - w.height()) / 2);  // 窗口设置居中显示
    w.show();//用户登录成功，显示主界面
    return a.exec();
}

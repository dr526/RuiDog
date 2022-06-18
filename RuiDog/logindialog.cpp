#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    //初始化系统托盘
    systemtrayicon=new QSystemTrayIcon(this);
    //设置图标
    QIcon icon=QIcon(":/img/RuiDog.png");
    //添加图标
    systemtrayicon->setIcon(icon);
    //当鼠标悬浮，显示文字
    systemtrayicon->setToolTip(QObject::trUtf8("RuiDog"));
    //显示图标
    systemtrayicon->show();
    menu=new QMenu(this);
    menuShowAction=new QAction("打开主界面");
    menuCloseAction=new QAction("退出");
    menu->addAction(menuShowAction);
    menu->addSeparator();//添加分割线
    menu->addAction(menuCloseAction);
    systemtrayicon->setContextMenu(menu);//系统托盘添加菜单栏
    connect(menuShowAction,SIGNAL(triggered(bool)),this,SLOT(showMenu()));
    connect(menuCloseAction,SIGNAL(triggered(bool)),this,SLOT(closeMenu()));
//    TcpConnect::instance()->tcpClient=new QTcpSocket(this);
//    QString ipAddressStr = SERVER_ADDR;
//    quint16 port = SERVER_PORT;
//    TcpConnect::instance()->network = false;
//    TcpConnect::instance()->tcpClient->connectToHost(ipAddressStr,port);//连接服务器
////    connect(TcpConnect::instance()->tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onSocketErrorChange(QAbstractSocket::SocketError)));//连接服务器失败
////    connect(TcpConnect::instance()->tcpClient,SIGNAL(connected()),this,SLOT(onConnected()));//连接服务器成功
//    connect(TcpConnect::instance()->tcpClient,SIGNAL(readyRead()),this,SLOT(onSocketReadyRead()));
//    if(!TcpConnect::instance()->network)
//    {
//        TcpConnect::instance()->tcpClient->connectToHost(ipAddressStr, port);
//        if(TcpConnect::instance()->tcpClient->waitForConnected(10000))
//        {
//            qDebug() << "Connect2Server OK";
//            TcpConnect::instance()->network = true;
//        }
//        else
//        {
//            qDebug() << TcpConnect::instance()->tcpClient->errorString();
//            //return;
//        }
//    }
//    else
//    {
//        TcpConnect::instance()->tcpClient->close();    //触发disconnected()信号
//        TcpConnect::instance()->network = false;
//    }

    //设置对话框无窗口边窗
    setWindowFlags(Qt::FramelessWindowHint);

    //设置关于窗体为圆角
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(),15,15);
    setMask(bmp);
}

LoginDialog::~LoginDialog()
{
    qDebug()<<"~LoginDialog()\n";
    delete ui;
    delete systemtrayicon;
    delete menu;
}


void LoginDialog::on_btn_register_clicked(){//点击注册按钮
    RegisterDialog registerDialog(this);
    registerDialog.setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);//设置窗口置顶，即：去除默认边框
    registerDialog.setWindowModality(Qt::ApplicationModal);
    registerDialog.show();
    if(registerDialog.exec()==QDialog::Accepted){
        if(registerDialog.getRegisterStatus()){
            UserInfo* userInfo=registerDialog.getUserInfo();
            ui->edit_name->setText(QString::number(userInfo->id));
            qDebug()<<"登录界面：注册成功";
        }
    }
}

void LoginDialog::on_btn_log_clicked(){//点击登陆按钮
    // QByteArray str = msg.toUtf8();
    //    QByteArray strdata = str.toUtf8();
   // strdata.append('\n');
    //tcpClient->write(strdata);


    if((ui->edit_name->text()=="")||(ui->edit_pw->text()=="")){
            QMessageBox::warning(this,tr("提示"),tr("用户名或密码不能为空"), QMessageBox::Yes);
    }
    else{
        userInfo.id=ui->edit_name->text().toInt();
        QByteArray pwd=ui->edit_pw->text().toUtf8();
        memcpy(userInfo.pwd,pwd.data(),50);
        qDebug()<<userInfo.id;
        qDebug()<<"用户密码为:"+QString(userInfo.pwd);
        return accept();    //关闭登录界面，并发送accpet()信号
//            json js;
//            js["msgid"]=LOGIN_MSG;
//            js["id"]=ui->edit_name->text().toInt();
//            char pwd[50];
//            QByteArray pwd_=ui->edit_pw->text().toUtf8();
//            memcpy(pwd,pwd_.data(),50);
//            js["password"]=pwd;
//            QString msg=QString::fromStdString(js.dump());
//            QByteArray request=msg.toUtf8();
//            int len=TcpConnect::instance()->tcpClient->write(request);
//            if(len==-1){
//                qDebug()<<"send login msg error:"+msg;
//            }
    }
//    if((ui->edit_name->text()=="root")&&(ui->edit_pw->text()=="1111")){
//        this->hide();
//        emit showmain();//登陆成功，切换到主界面
//    }
//    else if((ui->edit_name->text()=="")||(ui->edit_pw->text()=="")){
//        QMessageBox::warning(this,tr("提示"),tr("用户名或密码不能为空"), QMessageBox::Yes);
//    }
}

//void LoginDialog::on_btn_clear_clicked(){//点击取消按钮
//    this->close();
//}

void LoginDialog::mousePressEvent(QMouseEvent *event){
    isPressedDialog=true;//当前鼠标按下的即是Dialog而非界面上布局的其它控件
    last=event->globalPos();
}

void LoginDialog::mouseMoveEvent(QMouseEvent *event){
    if (isPressedDialog)
        {
            int dx = event->globalX() - last.x();
            int dy = event->globalY() - last.y();
            last = event->globalPos();
            move(x()+dx, y()+dy);//移动窗口，x()与y()为当前窗口的位置
        }
}

void LoginDialog::mouseReleaseEvent(QMouseEvent *event)
{
    int dx = event->globalX() - last.x();
    int dy = event->globalY() - last.y();
    move(x()+dx, y()+dy);
    isPressedDialog = false; // 鼠标松开时，置为false
}

void LoginDialog::showMenu(){
    this->show();
}

void LoginDialog::closeMenu(){
    this->close();
}

void LoginDialog::on_btn_min_clicked(){
    this->showMinimized();
}

void LoginDialog::on_btn_quit_clicked(){
    this->close();
}

//void LoginDialog::onSocketErrorChange(QAbstractSocket::SocketError)
//{
//    TcpConnect::instance()->network=false;//提示网络错误
//    qDebug()<<"网络错误";
//}

//void LoginDialog::onConnected(){
//    qDebug()<<"网络连接成功";
//}

//void LoginDialog::onSocketReadyRead(){
//    QByteArray array=TcpConnect::instance()->tcpClient->readAll();
//    char buffer[1024] = {0};
//    if(array.size()){
//        memcpy(buffer,array,array.size());
//        json js=json::parse(buffer);
//        int msgtype=js["msgid"].get<int>();
//        if(msgtype==LOGIN_MSG_ACK){
//            qDebug()<<"登录成功";
//            this->hide();
//            emit showmain();//登陆成功，切换到主界面
//        }
//    }
//}

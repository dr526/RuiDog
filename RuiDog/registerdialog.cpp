#include "registerdialog.h"
#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    registerStatus=false;

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

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::mousePressEvent(QMouseEvent *event){
    isPressedDialog=true;//当前鼠标按下的即是Dialog而非界面上布局的其它控件
    last=event->globalPos();
}

void RegisterDialog::mouseMoveEvent(QMouseEvent *event){
    if (isPressedDialog)
        {
            int dx = event->globalX() - last.x();
            int dy = event->globalY() - last.y();
            last = event->globalPos();
            move(x()+dx, y()+dy);//移动窗口，x()与y()为当前窗口的位置
        }
}

void RegisterDialog::mouseReleaseEvent(QMouseEvent *event)
{
    int dx = event->globalX() - last.x();
    int dy = event->globalY() - last.y();
    move(x()+dx, y()+dy);
    isPressedDialog = false; // 鼠标松开时，置为false
}

void RegisterDialog::on_btn_register_clicked(){//点击注册按钮
    if((ui->edit_name->text()=="")||(ui->edit_pwd->text()=="")){
            QMessageBox::warning(this,tr("提示"),tr("用户名或密码不能为空"), QMessageBox::Yes);
    }
    else{
        char name[50] = {0};
        QByteArray _name=ui->edit_name->text().toUtf8();
        memcpy(name,_name.data(),50);

        //userInfo.id=ui->edit_name->text().toInt();
        QByteArray pwd=ui->edit_pwd->text().toUtf8();
        memcpy(userInfo.pwd,pwd.data(),50);
        qDebug()<<"用户名为:"+QString(name);
        qDebug()<<"用户密码为:"+QString(userInfo.pwd);

        //用户名是中文名出错
        json js;
        js["msgid"]=REG_MSG;
        js["name"] = name;
        js["password"]=userInfo.pwd;
        QString msg=QString::fromStdString(js.dump());
        QByteArray request=msg.toUtf8();
        if (TcpConnect::instance()->tcpClient->isWritable()) {
            int len=TcpConnect::instance()->tcpClient->write(request);
            if(len==-1){
                qDebug()<<"发送数据错误，当前数据为:"+request;
            }
        }
        else {
            qDebug()<<"socket is unWritable";
        }
        if(TcpConnect::instance()->tcpClient->waitForReadyRead()==false){//接收数据失败
            qDebug()<<"接收数据失败";
            return;
        }
        QByteArray array=TcpConnect::instance()->tcpClient->readAll();
        char buffer[1024] = {0};
        if(array.size()){
           memcpy(buffer,array,array.size());
           json js=json::parse(buffer);
           int errNumber=js["errno"].get<int>();
           if(!errNumber){
                qDebug()<<"注册成功";
                qDebug()<<buffer;
                userInfo.id=js["id"];
                registerStatus=true;
           }
           else{
               string errmsg=js["errmsg"];
               QMessageBox::warning(this,tr("提示"),tr(errmsg.c_str()), QMessageBox::Yes);
               qDebug()<<QString::fromStdString(errmsg);
           }
        }
        return accept();    //关闭登录界面，并发送accpet()信号
    }
}

void RegisterDialog::on_btn_cancel_clicked(){
    this->close();
}

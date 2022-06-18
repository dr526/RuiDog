#include "choosefile.h"
#include "ui_choosefile.h"
#include "alarmwidget.h"
#include <QFileDialog>//核心
#include <QDebug>
#include <QGraphicsDropShadowEffect>
//void ChooseFile::setFriendInfo(User * Friend){
//    friendInfo = Friend;
//}
ChooseFile::ChooseFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChooseFile)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint|this->windowFlags());

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);//设置向哪个方向产生阴影效果，（0,0）表示向四周发散
    shadow->setColor(QColor(38, 78, 119,255));//阴影颜色
    shadow->setBlurRadius(25); //模糊度
    ui->widget->setGraphicsEffect(shadow);

    QPoint globalPos = parent->mapToGlobal(QPoint(0,0));//父窗口绝对坐标
    int x = globalPos.x() + (parent->width() - this->width()) / 2;//x坐标
    int y = globalPos.y() + (parent->height() - this->height()) / 2;//y坐标
    this->move(x, y);//窗口移动

    this->hide();
}

ChooseFile::~ChooseFile()
{
    delete ui;
}

void ChooseFile::setGroupInfo(int userId,string userName,int groupId){
    this->userId=userId;
    this->userName=userName;
    this->groupId=groupId;
    chatType=TYPE_GROUP_CHAT;
}

void ChooseFile::setFriendInfo(int userId,string userName,int friendId){
    this->friendId = friendId;
    this->userId=userId;
    this->userName=userName;
    chatType=TYPE_PRIVATE_CHAT;
}

void ChooseFile::on_chooseFile_clicked()//点击选择文件
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                 "/home"
                 );
    qDebug() << "fileName:" << fileName;
    ui->lineEdit->setText(fileName);
    ui->lineEdit->setCursorPosition(0);//把光标移到左侧
}

bool ChooseFile::isExistFileByAbsolutePath(QString path){
    return QFile::exists(path);
}

void ChooseFile::uploadFile(FileMeta filemeta){
    tcpSocket=new QTcpSocket(this);//创建QTcpSocket对象
    bool network=false;//tcp连接状态
    QString ip=SERVER_ADDR;
    quint16 port=8000;
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(disconnectSocket()));//客户端断开连接
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readSocketData()));
    if(!network){
        tcpSocket->connectToHost(ip,port);
        if(tcpSocket->waitForConnected(3000)){
            qDebug()<<"连接服务器成功";
            network=true;//连接成功
        }
        else{
            qDebug()<<"网络错误";
        }
    }
    else{
        tcpSocket->close();//触发disconnected()信号
        network=false;
    }
    if (tcpSocket->isWritable()) {
        imgName=filemeta.getUniqueName().toStdString();
        string _imgName=":u "+imgName;
        int len=tcpSocket->write(_imgName.c_str(),_imgName.length());
        tcpSocket->waitForBytesWritten();
        if(len==-1){
            qDebug()<<"发送数据错误，当前数据为:";
        }
    }
    else {
        qDebug()<<"socket is unWritable";
    }
    tcpSocket->waitForReadyRead(-1);
}

//请求发送文件
bool ChooseFile::requestSendFile(QString imgAbsolutePath){
    //如果图片存在
    qDebug()<<"receiver:   " + QString::number(friendId);
    if(isExistFileByAbsolutePath(imgAbsolutePath)){
        //作为资源上传的部分(会保存到本地)
        qDebug()<<"是否存在本地资源："<<isExistFileByAbsolutePath(imgAbsolutePath);
        FileMeta file_info = FileMeta(userId, QFileInfo(QFile(imgAbsolutePath)).fileName() );
        uploadFile(file_info);
        return true;
    }
    return false;
}


void ChooseFile::on_sendFile_clicked()
{
    qDebug()<<"********************";
    qDebug()<<"点击发送文件按钮";
    QString path = ui->lineEdit->text();
    if(requestSendFile(path)){
        qDebug()<<"SEND IMG SUCCESS";
        emit readyShowMyFile(QString::fromStdString(imgName));
    }else{
        qDebug()<<"SEND IMG ERROR";
    }
    this->close();
}

void ChooseFile::on_cancelButton_clicked()
{
    this->close();
}

void ChooseFile::mousePressEvent(QMouseEvent *event)
{
   if(event->button() == Qt::LeftButton)
   {
       m_point = event->globalPos() - pos(); //计算移动量
       event->accept();
   }
}

//鼠标移动
void ChooseFile::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - m_point);
        event->accept();
    }
}

void ChooseFile::disconnectSocket(){
    qDebug()<<"发送图片的连接关闭";
}

void ChooseFile::readSocketData(){
    char buf[1024]={0};
    if(tcpSocket->bytesAvailable()>0){
        qDebug()<<"执行了";
        tcpSocket->read(buf,sizeof(buf));
        if(strncmp(buf,"OK",2)==0){
            qDebug()<<"服务器返回OK!";
            QTextCodec *code = QTextCodec::codecForName("GB2312");
            string name = code->fromUnicode(ui->lineEdit->text()).data();
            FILE* fp=fopen(name.c_str(),"rb");
            if(!fp){
                qDebug()<<"打开文件失败";
            }
            else{
                int rc=0;
                while(!feof(fp)){
                    memset(buf,0,sizeof(buf));
                    rc=fread(buf,1,sizeof(buf),fp);
                    if(rc<0)break;
                    if(rc==0)continue;
                    int len=tcpSocket->write(buf,rc);
                    if(len<0){
                        qDebug()<<QString("发送文件失败");
                    }
                    else
                        tcpSocket->waitForBytesWritten();
                }
                qDebug()<<"文件发送成功";
            }
            fclose(fp);
        }
    }
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
    json js;
    js["msgid"]=FILE_MSG;
    js["type"]=chatType;
    js["groupid"]=groupId;
    js["id"]=userId;
    js["name"]=userName;
    js["toid"]=friendId;
    js["fileName"]=imgName;
    qDebug()<<"发送给业务服务器的文件名为："<<QString::fromStdString(imgName);
    QString sendMsg=QString::fromStdString(js.dump());
    QByteArray request=sendMsg.toUtf8();
    if (TcpConnect::instance()->tcpClient->isWritable()) {
       int len=TcpConnect::instance()->tcpClient->write(request);
       if(len==-1){
           qDebug()<<"发送数据错误，当前数据为:"+request;
        }
     }
     else {
           qDebug()<<"socket is unWritable";
     }
}

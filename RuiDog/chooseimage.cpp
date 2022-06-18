#include "chooseimage.h"
#include "ui_chooseimage.h"
#include "QFileDialog"
#include "QDebug"
#include <QGraphicsDropShadowEffect>
#include "mainwindow.h"

void ChooseImage::setFriendInfo(int userId,string userName,int friendId){
    this->friendId = friendId;
    this->userId=userId;
    this->userName=userName;
    this->chatType=TYPE_PRIVATE_CHAT;
}

void ChooseImage::setGroupInfo(int userId,string userName,int groupId){
    this->groupId = groupId;
    this->userId=userId;
    this->userName=userName;
    this->chatType=TYPE_GROUP_CHAT;
}

ChooseImage::ChooseImage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChooseImage)
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

ChooseImage::~ChooseImage()
{
    delete ui;
}

void ChooseImage::on_chooseImg_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                 "/home"
                 );//tr("Images (*.png *.xpm *.jpg *.txt *.gz)")
//    QString fileName = QFileDialog::getOpenFileName(this);
//    qDebug() << "fileName:" << fileName;
    ui->lineEdit->setText(fileName);
    ui->lineEdit->setCursorPosition(0);//把光标移到左侧
}

//点击关闭按钮
void ChooseImage::on_cancelButton_clicked()
{
    this->close();
}

//判断图片路径对应的图片是否存在
bool ChooseImage::isExistFileByAbsolutePath(QString path){
    return QFile::exists(path);
}

//发送上传图片命令
void ChooseImage::uploadImg(FileMeta filemeta){
//    QFile file(absolutePath);
//    file.open(QIODevice::ReadOnly);
//    QByteArray block = file.readAll();
//    file.close();
//    //在上传之前先把图片保存到本地.
//    QDir *folder = new QDir;
//    if(!folder->exists("./cache/images")){
//        folder->mkpath("./cache/images");
//    }
//    QFile file_local("./cache/images/" + fileName);
//    if(file_local.open(QIODevice::WriteOnly)){
//        file_local.write(block);
//        file_local.close();
//    }
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
        //QString sendMsg=":u "+fileName;
//        QString sendMsg="test";
//        QByteArray ctm=sendMsg.toUtf8();
//        char rnm[15]="text.png";
        imgName=filemeta.getUniqueName().toStdString();
        string _imgName=":u "+imgName;//组建上传命令，该命令要发送到文件服务器
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
//    QTextCodec *code = QTextCodec::codecForName("GB2312");
//    string name = code->fromUnicode(absolutePath).data();
//    FILE* fp=fopen(name.c_str(),"rb");
//    if(!fp){
//        qDebug()<<"文件打开失败";
//    }
//    else{
//        int rc=0;
//        while(!feof(fp)){
//            memset(buf,0,sizeof(buf));
//            rc=fread(buf,1,sizeof(buf),fp);
//            if(rc<0)break;
//            if(rc==0)continue;
//            if(tcpSocket->write(buf,rc)<0){
//                qDebug()<<QString("发送文件失败");
//                break;
//            }
//        }
//    }
//    fclose(fp);
//    tcpSocket->disconnectFromHost();
//    tcpSocket->close();
    //block是中文路径json报错
    //上传文件
    /*
        json js;
        js["msgid"] = ONE_CHAT_MSG;
        js["id"]=_userInfo.id;
        js["name"]=_userInfo.name;
        js["toid"]=friendId;
        js["msg"]=str.toStdString();
        js["time"]=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
        QString sendMsg=QString::fromStdString(js.dump());
        QByteArray request=sendMsg.toUtf8();
        SendMessage(request);//向服务器发送聊天消息
    */
//    json js;
//    js["msgid"]=IMG_MSG;
//    js["id"]=userId;
//    js["name"]=userName;
//    js["img"]=imgStr;
//    QString sendMsg=QString::fromStdString(js.dump());
//    QByteArray request=sendMsg.toUtf8();
//    if (TcpConnect::instance()->tcpClient->isWritable()) {
//        int len=TcpConnect::instance()->tcpClient->write(request);
//        if(len==-1){
//            qDebug()<<"发送数据错误，当前数据为:"+request;
//        }
//    }
//    else {
//        qDebug()<<"socket is unWritable";
//    }
}

//请求发送图片
bool ChooseImage::requestSendImg(QString imgAbsolutePath){
    //如果图片存在
    qDebug()<<"receiver:   " + QString::number(friendId);
    if(isExistFileByAbsolutePath(imgAbsolutePath)){
        //作为资源上传的部分(会保存到本地)
        qDebug()<<"是否存在本地资源："<<isExistFileByAbsolutePath(imgAbsolutePath);
        FileMeta file_info = FileMeta(userId, QFileInfo(QFile(imgAbsolutePath)).fileName() );
        uploadImg(file_info);
        return true;
    }
    return false;
}

//点击发送图片按钮
void ChooseImage::on_sendImg_clicked()
{
    qDebug()<<"********************";
    qDebug()<<"点击发送图片按钮";
    QString path = ui->lineEdit->text();//获取选中的图片路径
    if(requestSendImg(path)){
        qDebug()<<"SEND IMG SUCCESS";
        emit readyShowMyIMG(path);//图片发送成功，则向主界面发送信号，由主界面负责在聊天窗口显示该图片
    }else{
        qDebug()<<"SEND IMG ERROR";
    }
    this->close();
}

void ChooseImage::disconnectSocket(){
    qDebug()<<"发送图片的连接关闭";
}


/*
 1.接收文件服务器响应
 2.向文件服务器上传文件
 3.向业务服务器发送响应
*/

void ChooseImage::readSocketData(){
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
//                    if(tcpSocket->write(buf,rc)<0){
//                        qDebug()<<QString("发送文件失败");
//                        break;
//                    }
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
    js["msgid"]=IMG_MSG;
    js["type"]=chatType;
    js["groupid"]=groupId;
    js["id"]=userId;
    js["name"]=userName;
    js["toid"]=friendId;
    js["imgName"]=imgName;
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

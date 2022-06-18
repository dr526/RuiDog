#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitUI();
    Init();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cImag;
    delete addFriendDlg;
    delete TcpConnect::instance()->tcpClient;
}

void MainWindow::InitUI(){//设置界面样式
    this->setWindowTitle("RuiDog");
//    int width = this->width()-10;
//    int height = this->height()-10;
//    ui->centerWidget->setGeometry(5,5,width,height);
    ui->centerWidget->setStyleSheet("QWidget#centerWidget{ border-radius:4px; background:rgba(255,255,255,1); }");

    this->setWindowFlags(Qt::FramelessWindowHint);          //去掉标题栏无边框
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    //实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(0, 0);
    //设置阴影颜色
    shadow->setColor(QColor(39,40,43,100));
    //设置阴影圆角
    shadow->setBlurRadius(10);
    //给嵌套QWidget设置阴影
    ui->centerWidget->setGraphicsEffect(shadow);
}

//void MainWindow::receiveLogin(){//接受登陆界面的showmain信号并显示主界面
//    this->show();
//    connect(TcpConnect::instance()->tcpClient,SIGNAL(readyRead()),this,SLOT(onSocketRead()));
//}
void MainWindow::Init(){
    newVoice = NULL;
    addFriendDlg = new AddFriendDialog(this);
    connect(addFriendDlg, SIGNAL(sendFriendId(int)), this, SLOT(getFriendId(int)));
    connect(addFriendDlg, SIGNAL(sendGroupId(int)), this, SLOT(getGroupId(int)));
    cImag = new ChooseImage(this);
    cFile = new ChooseFile(this);
    connect(cImag,SIGNAL(readyShowMyIMG(QString)),this,SLOT(showMyImgMessage(QString)));
    connect(cFile,SIGNAL(readyShowMyFile(QString)),this,SLOT(showMyFileMessage(QString)));
    TcpConnect::instance()->tcpClient=new QTcpSocket;//创建QTcpSocket对象
    TcpConnect::instance()->network=false;//tcp连接状态
    loginStatus=false;//登录状态
    QString ip=SERVER_ADDR;
    quint16 port=SERVER_PORT;
    if(!TcpConnect::instance()->network){
        TcpConnect::instance()->tcpClient->connectToHost(ip,port);
        if(TcpConnect::instance()->tcpClient->waitForConnected(3000)){
            qDebug()<<"连接服务器成功";
            TcpConnect::instance()->network=true;//连接成功
        }
        else{
            qDebug()<<"网络错误";
        }
    }
    else{
        TcpConnect::instance()->tcpClient->close();//触发disconnected()信号
        TcpConnect::instance()->network=false;
    }
    LoginDialog loginDialog;//初始化登录界面
    loginDialog.setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);//设置窗口置顶，即：去除默认边框
    do{
        loginDialog.show();//显示登录界面
        int status=loginDialog.exec();//返回登录窗口运行状态
        if(status==QDialog::Accepted){//点击登录按钮
            UserInfo* userInfo=loginDialog.getUserInfo();//获取用户输入的账号信息
//            memmove(&_userInfo,userInfo,sizeof(UserInfo));
            _userInfo.id=userInfo->id;
            memcpy(_userInfo.pwd,userInfo->pwd,strlen(userInfo->pwd)+1);
            qDebug()<<"Login()";
            Login();//处理登录事务
        }
        else if(status==QDialog::Rejected){//点击关闭按钮
            loginStatus=false;
            return;
        }
    }while(!loginStatus);//未登录就停留在登录界面
    connect(TcpConnect::instance()->tcpClient,SIGNAL(disconnected()),this,SLOT(disconnectSocket()));//客户端断开连接
    connect(TcpConnect::instance()->tcpClient,SIGNAL(readyRead()),this,SLOT(readSocketData()));
}

int MainWindow::Login(){//处理登录事务
    //登录
    json js;
    js["msgid"]=LOGIN_MSG;
    js["id"]=_userInfo.id;
    js["password"]=_userInfo.pwd;
    QString msg=QString::fromStdString(js.dump());
    QByteArray request=msg.toUtf8();
    SendMessage(request);//向服务器发送用户账号信息
    if(TcpConnect::instance()->tcpClient->waitForReadyRead(-1)==false){//接收数据失败
        qDebug()<<"接收数据失败";
        return -1;
    }
    qDebug()<<"SendMessage(request) finish";
    QByteArray array=TcpConnect::instance()->tcpClient->readAll();
    char buffer[2028] = {0};
    if(array.size()){
       memcpy(buffer,array,array.size());
       qDebug()<<"memcpy finish";
       json js=json::parse(buffer);
       qDebug()<<"parse finish";
       int errNumber=js["errno"].get<int>();
       if(!errNumber){
            qDebug()<<"登录成功";
            _userInfo.name=js["name"];
            qDebug()<<buffer;
            qDebug()<<"开始执行getFriendInfo(js)";
            getFriendInfo(js);
            qDebug()<<"getFriendInfo(js)执行成功";
            getGroupInfo(js);
            qDebug()<<"getGroupInfo(js)执行成功";
            qDebug()<<"getOfflineMessage(js)执行开始";
            getOfflineMessage(js);
            //qDebug()<<buffer;
            loginStatus=true;
       }
       else{
           string errmsg=js["errmsg"];
           QMessageBox::warning(this,tr("提示"),tr(errmsg.c_str()), QMessageBox::Yes);
           qDebug()<<QString::fromStdString(errmsg);
       }
    }
    return true;
}

void MainWindow::getOfflineMessage(json responsejs){
    if (responsejs.contains("offlinemsg")){
        vector<string> vec = responsejs["offlinemsg"];
        for (string &str : vec){
            json js = json::parse(str);
            if (js["msgid"].get<int>() == ONE_CHAT_MSG){

                int friendId=js["id"].get<int>();
                string friendName=js["name"].get<string>();
                string recvMsg=js["msg"].get<string>();
                mapChatWidget::iterator iter = chatWigetMap.find("user"+to_string(friendId));
                if(iter!=chatWigetMap.end()){

                    QListWidget* chatWidget = iter->second;
                    ItemWidget* item=new ItemWidget(this);
                    item->setColor("rgb(221,188,254)");
                    int height=item->SetFriendsData(QString::fromStdString(friendName),":/img/defaultimg.png",QString::fromStdString(recvMsg));
                    QListWidgetItem * line = new QListWidgetItem;
                    line->setSizeHint(QSize(350, height));
                    chatWidget->addItem(line);
                    chatWidget->setItemWidget(line, item);
                    chatWidget->scrollToBottom();
                    qDebug()<<"["<<QString::fromStdString(friendName)<<"]"<<QString::fromStdString(recvMsg);
                 }
            }else{
                int groupId=js["groupid"].get<int>();
                string recvMsg=js["msg"].get<string>();
                //获取聊天群信息
                GroupInfo groupInfo;
                bool flag=false;//标志是否找到群组
                for(GroupInfo &iter:g_currentUserGroupList){
                    if(iter.m_account==groupId){
                        groupInfo=iter;
                        flag=true;
                    }
                }
                if(!flag)
                {
                    qDebug()<<"没有找到对应群组";
                }
                //从群用户信息总表中获取用户信息
                vector<GroupUser> groupUsers=groupInfo.users;
                GroupUser userInfo;
                for(GroupUser &iter:groupUsers){
                    if(iter.id==js["id"].get<int>()){
                        userInfo=iter;
                    }
                }

                mapChatWidget::iterator iter = chatWigetMap.find("group"+to_string(groupId));
                if(iter!=chatWigetMap.end()){
     /*               QListWidget* chatWidget = iter->second;
                    QListWidgetItem* item = new QListWidgetItem;
                    //这里转码，中文名称显示乱码
                    item->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + QString("[%1][%2]:%3").arg(userInfo.role.c_str()).arg(QString(userInfo.name.c_str())).arg(recvMsg.c_str()));
                    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                    chatWidget->addItem(item);*/

                    QListWidget* chatWidget = iter->second;
                    ItemWidget* item=new ItemWidget(this);
                    item->setColor("rgb(221,188,254)");
                    int height=item->SetFriendsData(QString(userInfo.name.c_str()),":/img/defaultimg.png",QString::fromStdString(recvMsg));
                    QListWidgetItem * line = new QListWidgetItem;
                    line->setSizeHint(QSize(350, height));
                    chatWidget->addItem(line);
                    chatWidget->setItemWidget(line, item);
                    chatWidget->scrollToBottom();
                    qDebug()<<"["<<groupInfo.m_groupName.c_str()<<"]"<<"["<<userInfo.name.c_str()<<"]"<<recvMsg.c_str();
                }
            }
        }
    }
}
void MainWindow::SendMessage(QByteArray request){//发送数据
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

void MainWindow::disconnectSocket(){
    qDebug()<<"连接关闭";
}

void MainWindow::readSocketData(){//接收服务器消息
    qDebug()<<"接收服务器响应数据";
    QByteArray array=TcpConnect::instance()->tcpClient->readAll();
    char buffer[1024] = {0};
    if(array.size()){
       memcpy(buffer,array,array.size());
       json js=json::parse(buffer);
       int msgType = js["msgid"].get<int>();
       qDebug()<<"msgType:"<<msgType;
       if(msgType==UPDATE_USER_ACK){
           QMessageBox::information(this, "提示", "密码修改成功", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
           //程序重启
           qApp->closeAllWindows();
           QProcess::startDetached(qApp->applicationFilePath(), QStringList());
       }
       if(msgType==ONE_VOICE_CHAT){
           qDebug()<<"接受到聊天邀请";
           string friendName=js["name"].get<string>();
           string msgInfo=friendName+"正向你发起语音聊天邀请，快加入吧!";
           VoiceAlert voiceAlert(this);
           voiceAlert.setMsgInfo(QString::fromStdString(msgInfo));
           voiceAlert.show();
           if(voiceAlert.exec() == QDialog::Accepted){//接收加入语音聊天
               ui->voiceButton->setStyleSheet("QPushButton { \
                                                border-image: url(:/img/novoice2.png);\
                                                border: none;\
                                                min-width: 20px;\
                                                max-width: 20px;\
                                                min-height: 20px;\
                                                max-height: 20px;}");
               newVoice = new Voice(this);
               newVoice->show();
               newVoice->move((QApplication::desktop()->width()-newVoice->width())/12,(QApplication::desktop()->height()-newVoice->height())/3);
           }
           else if(voiceAlert.exec() == QDialog::Rejected){//拒绝加入语音聊天
               qDebug()<<"拒绝加入语音聊天";
           }
       }
       if(msgType==GROUP_VOICE_CHAT){
            string friendName=js["name"].get<string>();
            string groupName=js["groupname"].get<string>();
            string msgInfo=groupName+"群组中的"+friendName+"正在发起聊天，快加入吧!";
            VoiceAlert voiceAlert(this);
            voiceAlert.setMsgInfo(QString::fromStdString(msgInfo));
            voiceAlert.show();
            if(voiceAlert.exec() == QDialog::Accepted){//接收加入语音聊天
                ui->voiceButton->setStyleSheet("QPushButton { \
                                                border-image: url(:/img/novoice2.png);\
                                                border: none;\
                                                min-width: 20px;\
                                                max-width: 20px;\
                                                min-height: 20px;\
                                                max-height: 20px;}");
                newVoice = new Voice(this);
                newVoice->show();
                newVoice->move((QApplication::desktop()->width()-newVoice->width())/12,(QApplication::desktop()->height()-newVoice->height())/3);
            }
            else if(voiceAlert.exec() == QDialog::Rejected){//拒绝加入语音聊天
                qDebug()<<"拒绝加入语音聊天";
            }
       }
       if(msgType==CREATE_GROUP_ACK){
           qDebug()<<"创建群组成功";
           qDebug()<<buffer;
           GroupInfo groupInfo;
           groupInfo.m_account=js["groupid"].get<int>();
           groupInfo.m_groupName=js["groupname"].get<string>();
           groupInfo.m_groupDesc=js["groupdesc"].get<string>();
           g_currentUserGroupList.push_back(groupInfo);
           Init_Group_Info(groupInfo);
           qDebug()<<"群组账号："<<groupInfo.m_account<<", name："<<QString::fromStdString(groupInfo.m_groupName);
       }
       if(msgType==ADD_FRIEND_RESPONSE){
           qDebug()<<"对方同意添加好友";
           int friendId=js["id"].get<int>();
           json addFriend;
           addFriend["msgid"] = ADD_FRIEND_MSG;
           addFriend["id"] = _userInfo.id;
           addFriend["friendid"] = friendId;
           QString addSend=QString::fromStdString(addFriend.dump());
           SendMessage(addSend.toUtf8());
       }
       if(msgType==ADD_FRIEND_REQUEST){
           qDebug()<<"接收到添加请求";
           qDebug()<<buffer;
//           js["msgid"] = ADD_FRIEND_REQUEST;
//           js["id"] = _userInfo.id;
//           js["toid"] = addFriendInfo.friendId;
//           js["requestInfo"] = addFriendInfo.requestInfo;
           int friendId=js["id"].get<int>();
           AddFriendalert addFriendalert(this);
           string request=js["requestInfo"].get<string>();
           addFriendalert.setAddRequest(QString::fromStdString(request));
           addFriendalert.show();
           if(addFriendalert.exec() == QDialog::Accepted){
               qDebug()<<"我要开始对添加好友消息进行回复了";
               json response;
               response["msgid"]=ADD_FRIEND_RESPONSE;
               response["id"]=_userInfo.id;
               response["toid"]=friendId;
               QString responseSend=QString::fromStdString(response.dump());
               SendMessage(responseSend.toUtf8());
               //再次发送数据时出现错误，写缓冲区有问题；
               json addFriend;
               addFriend["msgid"] = ADD_FRIEND_MSG;
               addFriend["id"] = _userInfo.id;
               addFriend["friendid"] = friendId;
               QString addSend=QString::fromStdString(addFriend.dump());
               //等上一个write()完毕后才继续下一个write()
               TcpConnect::instance()->tcpClient->waitForBytesWritten(-1);
               SendMessage(addSend.toUtf8());
           }
       }
       if(msgType==ADD_GROUP_RESPONSE){
           qDebug()<<"群主同意添加你加入群";
           /*
               response["msgid"]=ADD_GROUP_RESPONSE;
               response["id"]=_userInfo.id;
               response["toid"]=friendId;
           */
           int groupId=js["groupid"].get<int>();
           json addGroup;
           addGroup["msgid"]=ADD_GROUP_MSG;
           addGroup["id"]=_userInfo.id;
           addGroup["groupid"]=groupId;
           QString addSend=QString::fromStdString(addGroup.dump());
           SendMessage(addSend.toUtf8());
       }
       if(msgType==ADD_GROUP_REQUEST){
           qDebug()<<"接收到添加请求";
           qDebug()<<buffer;
           /*
            js["msgid"]=ADD_GROUP_REQUEST;
            js["groupid"]=addGroupInfo.groupId;
            js["id"]=_userInfo.id;
            js["requestInfo"]=addGroupInfo.requestInfo;
           */
           int friendId=js["id"].get<int>();
           int groupId=js["groupid"].get<int>();
           AddGroupalert addGroupalert(this);
           string request=js["requestInfo"].get<string>();
           addGroupalert.setAddRequest(QString::fromStdString(request));
           addGroupalert.show();
           if(addGroupalert.exec() == QDialog::Accepted){
               qDebug()<<"我要开始对添加好友消息进行回复了";
               json response;
               response["msgid"]=ADD_GROUP_RESPONSE;
               response["id"]=_userInfo.id;
               response["toid"]=friendId;
               response["groupid"]=groupId;
               QString responseSend=QString::fromStdString(response.dump());
               SendMessage(responseSend.toUtf8());
           }
       }
       if(msgType==Query_FRIEND_ACK){
           //根据friendId获取好友的信息
           qDebug()<<"获取好友的信息";
           qDebug()<<buffer;
           FriendInfo friendInfo;
           friendInfo.id=js["id"].get<int>();
           friendInfo.name=js["name"].get<string>();
           friendInfo.state=js["state"].get<string>();
           addFriendDlg->setAddFriendInfo(friendInfo);//将获取到的好友信息显示到添加好友窗口中，供用户查看
       }
       if(msgType==Query_GROUP_ACK){
           //根据群组id获取群组信息
           qDebug()<<"获取群组信息";
           qDebug()<<buffer;
           GroupInfo groupInfo;
           groupInfo.m_account=js["id"].get<int>();
           groupInfo.m_groupName=js["name"].get<string>();
           groupInfo.m_groupDesc=js["desc"].get<string>();
           addFriendDlg->setAddGroupInfo(groupInfo);
       }
       if(msgType==ADD_FRIEND_ACK){
            //处理好友请求后服务器的响应
           qDebug()<<"添加好友返回的信息";
           qDebug()<<buffer;
           FriendInfo friendInfo;
           friendInfo.id=js["id"].get<int>();
           friendInfo.name=js["name"].get<string>();
           friendInfo.state=js["state"].get<string>();
           g_currentUserFriendList.push_back(friendInfo);
           Init_Friend_Info(friendInfo);
           qDebug()<<"好友账号:"<<friendInfo.id<<",name:"<<QString::fromStdString(friendInfo.name);
       }
       if(msgType==ADD_GROUP_ACK){
           //处理添加群组请求后服务器的响应
           qDebug()<<"添加群组返回的信息";
           qDebug()<<buffer;
           GroupInfo groupInfo;
           groupInfo.m_account=js["id"].get<int>();
           groupInfo.m_groupName=js["name"].get<string>();
           groupInfo.m_groupDesc=js["desc"].get<string>();
           g_currentUserGroupList.push_back(groupInfo);
           Init_Group_Info(groupInfo);
           qDebug()<<"群组账号："<<groupInfo.m_account<<", name："<<QString::fromStdString(groupInfo.m_groupName);
       }
       if (msgType==ONE_CHAT_MSG){
           qDebug()<<"接收到一对一聊天数据：";
           qDebug()<<buffer;
           int friendId=js["id"].get<int>();
           string friendName=js["name"].get<string>();
           string recvMsg=js["msg"].get<string>();
           //获取聊天群信息
           mapChatWidget::iterator iter = chatWigetMap.find("user"+to_string(friendId));
           if(iter!=chatWigetMap.end()){
/*               QListWidget* chatWidget = iter->second;
               QListWidgetItem* item = new QListWidgetItem;
               //这里转码，中文名称显示乱码
               item->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + QString("[%1]:%2").arg(QString::fromStdString(friendName)).arg(QString::fromStdString(recvMsg)));
               item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
               chatWidget->addItem(item);*/

               QListWidget* chatWidget = iter->second;
               ItemWidget* item=new ItemWidget(this);
               item->setColor("rgb(221,188,254)");
               int height=item->SetFriendsData(QString::fromStdString(friendName),":/img/defaultimg.png",QString::fromStdString(recvMsg));
               QListWidgetItem * line = new QListWidgetItem;
               line->setSizeHint(QSize(350, height));
               chatWidget->addItem(line);
               chatWidget->setItemWidget(line, item);
               chatWidget->scrollToBottom();
               qDebug()<<"["<<QString::fromStdString(friendName)<<"]"<<QString::fromStdString(recvMsg);
            }
       }
       if(msgType==IMG_MSG){
           qDebug()<<"接收到图片信息";
           /*
            js["msgid"]=IMG_MSG;
            js["id"]=userId;
            js["name"]=userName;
            js["toid"]=friendId;
            js["img"]=block;
           */
           imgInfo.friendId=js["id"].get<int>();
           imgInfo.friendName=js["name"].get<string>();
           imgInfo.imgPath=js["imgName"].get<string>();
           imgInfo.groupId=js["groupid"].get<int>();
           imgInfo.chatType=js["type"].get<int>();
           //在显示之前先把图片保存到本地.
           //在上传之前先把图片保存到本地.
           QDir *folder = new QDir;
           if(!folder->exists("./rnm/images")){
               folder->mkpath("./rnm/images");
           }
           delete folder;
           tcpSocket=new QTcpSocket(this);//创建QTcpSocket对象
           bool network=false;//tcp连接状态
           QString ip=SERVER_ADDR;
           quint16 port=8001;
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
               string _imgName=":d "+imgInfo.imgPath;
               //QTcpSocket使用的是异步收发，就是说write只是写进了buff，而不是真正发送出去
               int len=tcpSocket->write(_imgName.c_str(),_imgName.length());
//               int len=tcpSocket->write("狗粮养的");
               tcpSocket->waitForBytesWritten();//阻塞等待写完成，只是将数据从用户区写入协议栈的写缓冲区中
               if(len==-1){
                   QMessageBox::critical(this,tr("日志"),"发送数据错误，当前数据为:");
                   qDebug()<<"发送数据错误，当前数据为:";
               }
           }
           else {
               QMessageBox::critical(this,tr("日志"),"socket is unWritable");
               qDebug()<<"socket is unWritable";
           }
           tcpSocket->waitForReadyRead(-1);
           char number[65];
           tcpSocket->read(number,sizeof(number));
           string str=number;
//           imgTotalSize=stoi(str);
           imgTotalSize=strtoull(str.c_str(),NULL,0);
           QMessageBox::critical(this,tr("文件大小"),QString::number(imgTotalSize));
           img=new QFile();
           img->setFileName(QString::fromStdString("./rnm/images/"+imgInfo.imgPath));
           img->open(QFile::WriteOnly);
           imgSize=0;
           connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readImgData()));
           tcpSocket->write("OK",2);
           tcpSocket->waitForBytesWritten();
//           tcpSocket->waitForReadyRead(-1);
//                      string path="./rnm/images/"+imgName;
//                      FILE* fp=fopen(path.c_str(),"w");
//                      if(!fp){
//                          QMessageBox::critical(this,tr("日志"),"创建文件失败");
//                          qDebug()<<"创建文件失败";
//                      }
//                      else{
//                          int n=0;
//                          char buffer[1024]={0};
//                          while((n=tcpSocket->read(buffer,sizeof(buffer)))>0){
//                               fwrite(buffer,1,n,fp);
//                               qDebug()<<"*";
//                          }
//                      }
//                      qDebug()<<"文件下载完毕";
//                      QMessageBox::critical(this,tr("日志"),"文件下载完毕");
//                      fclose(fp);
//                      tcpSocket->disconnectFromHost();
//                      tcpSocket->close();
//                      delete tcpSocket;
//           imgSize=0;
//           img=new QFile();
//           img->setFileName(QString::fromStdString("./rnm/images"+imgPath));
//           if(!img->open(QIODevice::WriteOnly)){
//               qDebug()<<"文件创建失败";
//               img->close();
//           }
//           else
//            connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readImgData()));

//           string path="./rnm/images/"+imgName;
//           FILE* fp=fopen(path.c_str(),"w");
//           if(!fp){
//               QMessageBox::critical(this,tr("日志"),"创建文件失败");
//               qDebug()<<"创建文件失败";
//           }
//           else{
//               int n=0;
//               char buffer[1024]={0};
//               tcpSocket->waitForReadyRead();
//               while((n=tcpSocket->read(buffer,sizeof(buffer)))>0){
//                    fwrite(buffer,1,n,fp);
//                    qDebug()<<"*";
//               }
//           }
//           qDebug()<<"文件下载完毕";
//           QMessageBox::critical(this,tr("日志"),"文件下载完毕");
//           fclose(fp);
//           tcpSocket->disconnectFromHost();
//           tcpSocket->close();
//           delete tcpSocket;
           //获取聊天群信息
//           mapChatWidget::iterator iter = chatWigetMap.find("user"+to_string(friendId));
//           if(iter!=chatWigetMap.end()){
//               /*QListWidget* chatWidget = iter->second;
//               ItemWidget* item=new ItemWidget(this);
//               item->setColor("rgb(221,188,254)");
//               int height=item->SetFriendsData(QString::fromStdString(friendName),":/img/defaultimg.png",QString::fromStdString(recvMsg));
//               QListWidgetItem * line = new QListWidgetItem;
//               line->setSizeHint(QSize(350, height));
//               chatWidget->addItem(line);
//               chatWidget->setItemWidget(line, item);
//               chatWidget->scrollToBottom();*/
//               QListWidget* chatWidget = iter->second;
//               ItemWidget* item=new ItemWidget(this);
//               item->setColor("rgb(137,214,146");
//               int height=item->SetFriendsImg(QString::fromStdString(friendName),":/img/defaultimg.png",path);
//               QListWidgetItem * line = new QListWidgetItem;
//               line->setSizeHint(QSize(350, height));
//               chatWidget->addItem(line);
//               chatWidget->setItemWidget(line, item);
//               chatWidget->scrollToBottom();
//            }
       }
       if(msgType==FILE_MSG){
           qDebug()<<"接收到图片信息";
           imgInfo.friendId=js["id"].get<int>();
           imgInfo.friendName=js["name"].get<string>();
           imgInfo.imgPath=js["fileName"].get<string>();
           imgInfo.groupId=js["groupid"].get<int>();
           imgInfo.chatType=js["type"].get<int>();
           //在显示之前先把图片保存到本地.
           //在上传之前先把图片保存到本地.
           QDir *folder = new QDir;
           if(!folder->exists("./rnm/files")){
               folder->mkpath("./rnm/files");
           }
           delete folder;
           fileSocket=new QTcpSocket(this);//创建QTcpSocket对象
           bool network=false;//tcp连接状态
           QString ip=SERVER_ADDR;
           quint16 port=8001;
           if(!network){
               fileSocket->connectToHost(ip,port);
               if(fileSocket->waitForConnected(3000)){
                   qDebug()<<"连接服务器成功";
                   network=true;//连接成功
               }
               else{
                   qDebug()<<"网络错误";
               }
           }
           else{
               fileSocket->close();//触发disconnected()信号
               network=false;
           }
           if (fileSocket->isWritable()) {
               string _imgName=":d "+imgInfo.imgPath;
               int len=fileSocket->write(_imgName.c_str(),_imgName.length());
               fileSocket->waitForBytesWritten();
               if(len==-1){
                   QMessageBox::critical(this,tr("日志"),"发送数据错误，当前数据为:");
                   qDebug()<<"发送数据错误，当前数据为:";
               }
           }
           else {
               QMessageBox::critical(this,tr("日志"),"socket is unWritable");
               qDebug()<<"socket is unWritable";
           }
           fileSocket->waitForReadyRead(-1);
           char number[65];
           fileSocket->read(number,sizeof(number));
           string str=number;
           imgTotalSize=strtoull(str.c_str(),NULL,0);
           QMessageBox::critical(this,tr("文件大小"),QString::number(imgTotalSize));
           img=new QFile();
           img->setFileName(QString::fromStdString("./rnm/files/"+imgInfo.imgPath));
           img->open(QFile::WriteOnly);
           imgSize=0;
           connect(fileSocket,SIGNAL(readyRead()),this,SLOT(readFileData()));
           fileSocket->write("OK",2);
           fileSocket->waitForBytesWritten();
       }
       if (msgType==GROUP_CHAT_MSG){
           qDebug()<<"接收到群消息：";
           qDebug()<<buffer;
           int groupId=js["groupid"].get<int>();
           string recvMsg=js["msg"].get<string>();
           qDebug()<<"群聊账号:"<<groupId;
           //获取聊天群信息
           GroupInfo groupInfo;
           bool flag=false;//标志是否找到群组
           for(GroupInfo &iter:g_currentUserGroupList){
               if(iter.m_account==groupId){
                   groupInfo=iter;
                   flag=true;
               }
           }
           if(!flag)
           {
               qDebug()<<"没有找到对应群组";
           }
           //从群用户信息总表中获取用户信息
           vector<GroupUser> groupUsers=groupInfo.users;
           GroupUser userInfo;
           for(GroupUser &iter:groupUsers){
               if(iter.id==js["id"].get<int>()){
                   userInfo=iter;
               }
           }

           mapChatWidget::iterator iter = chatWigetMap.find("group"+to_string(groupId));
           if(iter!=chatWigetMap.end()){
/*               QListWidget* chatWidget = iter->second;
               QListWidgetItem* item = new QListWidgetItem;
               //这里转码，中文名称显示乱码
               item->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + QString("[%1][%2]:%3").arg(userInfo.role.c_str()).arg(QString(userInfo.name.c_str())).arg(recvMsg.c_str()));
               item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
               chatWidget->addItem(item);*/

               QListWidget* chatWidget = iter->second;
               ItemWidget* item=new ItemWidget(this);
               item->setColor("rgb(221,188,254)");
               int height=item->SetFriendsData(QString(userInfo.name.c_str()),":/img/defaultimg.png",QString::fromStdString(recvMsg));
               QListWidgetItem * line = new QListWidgetItem;
               line->setSizeHint(QSize(350, height));
               chatWidget->addItem(line);
               chatWidget->setItemWidget(line, item);
               chatWidget->scrollToBottom();
               qDebug()<<"["<<groupInfo.m_groupName.c_str()<<"]"<<"["<<userInfo.name.c_str()<<"]"<<recvMsg.c_str();
           }
       }
    }
}

void MainWindow::readFileData(){
    QByteArray recvMsg;
    if(fileSocket->bytesAvailable()<1024){
        return;
    }
    recvMsg=fileSocket->readAll();
    img->write(recvMsg);
    imgSize+=recvMsg.length();
    if(imgSize>=imgTotalSize){
        QMessageBox::critical(this,tr("日志"),"文件下载完毕");
        imgSize=0;
        img->close();
        fileSocket->disconnectFromHost();
        fileSocket->close();
        //获取聊天群信息
        mapChatWidget::iterator iter;
        if(imgInfo.chatType==TYPE_PRIVATE_CHAT){
            iter = chatWigetMap.find("user"+to_string(imgInfo.friendId));
            qDebug()<<"单聊图片信息";
        }
        else if(imgInfo.chatType==TYPE_GROUP_CHAT){
            iter = chatWigetMap.find("group"+to_string(imgInfo.groupId));
        }
           if(iter!=chatWigetMap.end()){
               QListWidget* chatWidget = iter->second;
               ItemWidget* item=new ItemWidget(this);
               item->setColor("rgb(137,214,146");
               int height=item->SetFriendsFile(QString::fromStdString(imgInfo.friendName),":/img/defaultimg.png",QString::fromStdString(imgInfo.imgPath));
               QListWidgetItem * line = new QListWidgetItem;
               line->setSizeHint(QSize(350, height));
               chatWidget->addItem(line);
               chatWidget->setItemWidget(line, item);
               chatWidget->scrollToBottom();
            }
    }
}

void MainWindow::readImgData(){
    QByteArray recvMsg;
    if(tcpSocket->bytesAvailable()<1024){
        return;
    }
    recvMsg=tcpSocket->readAll();
    img->write(recvMsg);
    imgSize+=recvMsg.length();
    if(imgSize>=imgTotalSize){
        QMessageBox::critical(this,tr("日志"),"文件下载完毕");
        imgSize=0;
        img->close();
        tcpSocket->disconnectFromHost();
        tcpSocket->close();
        //获取聊天群信息
        mapChatWidget::iterator iter;
        if(imgInfo.chatType==TYPE_PRIVATE_CHAT){
            iter = chatWigetMap.find("user"+to_string(imgInfo.friendId));
            qDebug()<<"单聊图片信息";
        }
        else if(imgInfo.chatType==TYPE_GROUP_CHAT){
            iter = chatWigetMap.find("group"+to_string(imgInfo.groupId));
        }
           //mapChatWidget::iterator iter = chatWigetMap.find("group"+to_string(groupId));
           //mapChatWidget::iterator iter = chatWigetMap.find("user"+to_string(imgInfo.friendId));
           if(iter!=chatWigetMap.end()){
               /*QListWidget* chatWidget = iter->second;
               ItemWidget* item=new ItemWidget(this);
               item->setColor("rgb(221,188,254)");
               int height=item->SetFriendsData(QString::fromStdString(friendName),":/img/defaultimg.png",QString::fromStdString(recvMsg));
               QListWidgetItem * line = new QListWidgetItem;
               line->setSizeHint(QSize(350, height));
               chatWidget->addItem(line);
               chatWidget->setItemWidget(line, item);
               chatWidget->scrollToBottom();*/
               QListWidget* chatWidget = iter->second;
               ItemWidget* item=new ItemWidget(this);
               item->setColor("rgb(137,214,146");
               int height=item->SetFriendsImg(QString::fromStdString(imgInfo.friendName),":/img/defaultimg.png",QString::fromStdString("./rnm/images/"+imgInfo.imgPath));
               QListWidgetItem * line = new QListWidgetItem;
               line->setSizeHint(QSize(350, height));
               chatWidget->addItem(line);
               chatWidget->setItemWidget(line, item);
               chatWidget->scrollToBottom();
            }
    }
//    if(tcpSocket->bytesAvailable()<1024)
//        return;
//    char buffer[1024];
//    qint64 len=tcpSocket->read(buffer,1024);
//    img->write(buffer);
//    imgSize+=len;
//    if(imgSize==imgTotalSize){
//        img->close();
//        tcpSocket->disconnectFromHost();
//        tcpSocket->close();
//    }
////    imgPath="./rnm/images/"+imgPath;
//////    QMessageBox::critical(this,tr("日志"),QString::fromStdString(imgPath));
////    FILE* fp=fopen(imgPath.c_str(),"w");
////    if(!fp){
////        QMessageBox::critical(this,tr("日志"),"创建文件失败");
////        qDebug()<<"创建文件失败";
////    }
////    else{
////        int n=0;

////        char buffer[1024]={0};
//////        tcpSocket->waitForReadyRead(-1);
////        while((n=tcpSocket->read(buffer,sizeof(buffer)))>0){
////             fwrite(buffer,1,n,fp);
////             qDebug()<<"+";
////        }
////    }
////    qDebug()<<"文件下载完毕";
////    QMessageBox::critical(this,tr("日志"),"文件下载完毕");
////    fclose(fp);
////    tcpSocket->disconnectFromHost();
////    tcpSocket->close();
////    delete tcpSocket;
}

void MainWindow::getGroupInfo(json responsejs){// 记录当前用户的群组列表信息
    if(responsejs.contains("groups")){
        g_currentUserGroupList.clear();
        vector<string> vec=responsejs["groups"];
        for(string &groupstr:vec){
            json groupjs=json::parse(groupstr);
            GroupInfo groupInfo;
            groupInfo.m_account=groupjs["id"].get<int>();
            groupInfo.m_groupName=groupjs["groupname"].get<string>();
            groupInfo.m_groupDesc=groupjs["groupdesc"].get<string>();
            vector<string> _vec=groupjs["users"];
            for(string &userstr:_vec){
                json userjs=json::parse(userstr);
                GroupUser groupUser;
                groupUser.id=userjs["id"].get<int>();
                groupUser.name=userjs["name"].get<string>();
                groupUser.role=userjs["role"].get<string>();
                groupInfo.users.push_back(groupUser);
            }
            g_currentUserGroupList.push_back(groupInfo);
            Init_Group_Info(groupInfo);
            qDebug()<<QString("群组：[%1]%2").arg(groupInfo.m_account).arg(groupInfo.m_groupName.c_str());
        }
    }
}

void MainWindow::getFriendInfo(json responsejs){//将好友信息存储到g_currentUserFriendList中
    // 记录当前用户的好友列表信息
    qDebug()<<"getFriendInfo begin";
    if (responsejs.contains("friends"))
    {
        // 初始化
        qDebug()<<"没出现friends而执行了";
        g_currentUserFriendList.clear();

        vector<string> vec = responsejs["friends"];
        for (string &str : vec)
        {
            json js = json::parse(str);
            FriendInfo friendInfo;
            friendInfo.id=js["id"].get<int>();
            friendInfo.name=js["name"].get<string>();
            friendInfo.state=js["state"];
            g_currentUserFriendList.push_back(friendInfo);
            Init_Friend_Info(friendInfo);
        }
    }
    qDebug()<<"getFriendInfo end";
}

static int stackWidgetIndex = 0;

void MainWindow::Init_Friend_Info(FriendInfo friendInfo){//在主界面显示好友列表
    //string state=friendInfo.state=="offline"?"离线":"在线";
    ui->listWidget_info->addItem(QString::fromStdString("[好友]["+friendInfo.name+"]"));

    ui->stackedWidget_Msg->setCurrentIndex(stackWidgetIndex++);
    //每一个好友都会对应一个聊天窗口页面
    if(stackWidgetIndex >=ui->stackedWidget_Msg->count())
    {
        QWidget* page = new QWidget();
        page->setObjectName(QString("page%1").arg(stackWidgetIndex));
        ui->stackedWidget_Msg->addWidget(page);
        qDebug()<<"stackedWidget_Msg account:"<<ui->stackedWidget_Msg->count()<<",stackWidgetIndex:"<<stackWidgetIndex;
    }
    QListWidget* listWidget = new QListWidget;
    QGridLayout* mainLayout = new QGridLayout;
    ui->stackedWidget_Msg->currentWidget()->setLayout(mainLayout);
    listWidget->setStyleSheet("QListWidget{"\
                              "background-color: rgb(247,247,247);"\
                              "border-style: none;"\
                              "border-top:1px solid #D6D6D6;"\
                              "}");
    mainLayout->addWidget(listWidget);
    mainLayout->setMargin(0); //设置外边距
    mainLayout->setSpacing(0);//设置内边距
    chatWigetMap.insert(std::make_pair("user"+to_string(friendInfo.id),listWidget));//设置聊天列表
    chatWidgetInfo* chatInfo = new chatWidgetInfo;
    chatInfo->m_account = friendInfo.id;
    chatInfo->m_type = TYPE_PRIVATE_CHAT;

    chatWidgetInfoList.push_back(chatInfo);
    ui->listWidget_info->setCurrentRow(stackWidgetIndex - 1);
}

void MainWindow::Init_Group_Info(GroupInfo groupInfo){
    ui->listWidget_info->addItem(QString("[群][%1] %2").arg(groupInfo.m_groupName.c_str()).arg(groupInfo.m_groupDesc.c_str()));

    ui->stackedWidget_Msg->setCurrentIndex(stackWidgetIndex++);
    if(stackWidgetIndex >=ui->stackedWidget_Msg->count())
    {
        QWidget* page = new QWidget();
        page->setObjectName(QString("page%1").arg(stackWidgetIndex));
        ui->stackedWidget_Msg->addWidget(page);
        qDebug()<<"stackedWidget_Msg account:"<<ui->stackedWidget_Msg->count()<<",stackWidgetIndex:"<<stackWidgetIndex;
    }
    QListWidget* listWidget = new QListWidget;
    QGridLayout* mainLayout = new QGridLayout;
    ui->stackedWidget_Msg->currentWidget()->setLayout(mainLayout);
    listWidget->setStyleSheet("QListWidget{"\
                              "background-color: rgb(247,247,247);"\
                              "border-style: none;"\
                              "}");
    mainLayout->addWidget(listWidget);
    mainLayout->setMargin(0); //设置外边距
    mainLayout->setSpacing(0);//设置内边距

    chatWigetMap.insert(std::make_pair("group"+to_string(groupInfo.m_account),listWidget));
    chatWidgetInfo* chatInfo = new chatWidgetInfo;
    chatInfo->m_account = groupInfo.m_account;
    chatInfo->m_type = TYPE_GROUP_CHAT;

    chatWidgetInfoList.push_back(chatInfo);

    ui->listWidget_info->setCurrentRow(stackWidgetIndex - 1);
}

void MainWindow::on_listWidget_info_itemClicked(QListWidgetItem *item){//点击主界面左侧列表项，即：好友列表
    int currentRow = ui->listWidget_info->currentRow();
    //设置索引index所在的页面为当前页面
    ui->stackedWidget_Msg->setCurrentIndex(currentRow);

    qDebug()<<item->text()<<"current Row chicked.."<<currentRow;
}

void MainWindow::on_btn_hide_clicked(){
    this->showMinimized();
}

void MainWindow::on_btn_close_clicked(){
    this->close();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    isPressedWindow=true;//当前鼠标按下的即是Dialog而非界面上布局的其它控件
    last=event->globalPos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if (isPressedWindow)
        {
            int dx = event->globalX() - last.x();
            int dy = event->globalY() - last.y();
            last = event->globalPos();
            move(x()+dx, y()+dy);//移动窗口，x()与y()为当前窗口的位置
        }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    int dx = event->globalX() - last.x();
    int dy = event->globalY() - last.y();
    move(x()+dx, y()+dy);
    isPressedWindow = false; // 鼠标松开时，置为false
}

void MainWindow::on_btn_send_clicked(){//点击发送按钮，发送信息
    qDebug()<<"on_pushBtn_send_clicked";
    if(ui->textEdit->toPlainText().isEmpty()){
        qDebug()<<"Msg is null";
        return;
    }
    int currentRow = ui->listWidget_info->currentRow();
    if(currentRow < 0){
        qDebug()<<"未选择聊天窗口";
        return;
    }
    else{
        qDebug()<<"当前聊天窗口为：";
        qDebug()<<currentRow;
    }
    int isfind = 0;
    int currentIndex = 0;
    QListWidget* chatWidget = NULL;
    chatWidgetInfo* chatInfo = NULL;
    //根据当前选中的listWidget的编号，找到对应的chatInfo
    for(listChatWidgetInfo::iterator iter = chatWidgetInfoList.begin();iter!=chatWidgetInfoList.end();iter++,currentIndex++){
        if(currentIndex == currentRow){
            chatInfo = *iter;
            isfind = 1;
        }
    }
    if(isfind){
    }
    else {
        qDebug()<<"no friend";
        return ;
    }
    //根据chatInfo的好友账号信息，找到chatWidgetMap对应的好友聊天窗口
    string materStr;
    if(chatInfo->m_type==TYPE_PRIVATE_CHAT){
        materStr="user"+to_string(chatInfo->m_account);
    }
    if(chatInfo->m_type==TYPE_GROUP_CHAT){
        materStr="group"+to_string(chatInfo->m_account);
    }
    mapChatWidget::iterator iter = chatWigetMap.find(materStr);////这里可能出错了
    if(iter!=chatWigetMap.end()){
        chatWidget = iter->second;
    }
    else {
        qDebug()<<"not find..";
        return;
    }

    QString str = ui->textEdit->toPlainText();
    if(str.toStdString().size() > MAX_SEND_LENGTH){
        qDebug()<<"超出最大发送大小..";
        return;
    }
    ui->textEdit->clear();

    //在聊天对话框中显示聊天信息
//    chatWidget->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + "[" + "我" + "]" +str);

    ItemWidget* itemw = new ItemWidget(this);
    itemw->setColor("rgb(254,208,142)");//    color = "rgb(254,208,142)";
    int height=itemw->SetMyData(QString("我"),":/img/defaultimg.png",str);//id,头像，信息:/img/defaultimg.png
    QListWidgetItem* line = new QListWidgetItem();
    line->setSizeHint(QSize(350, height));
    chatWidget->addItem(line);
    chatWidget->setItemWidget(line, itemw);
    chatWidget->scrollToBottom();

    qDebug()<<"["<<QString::fromStdString(_userInfo.name)<<"]["<<_userInfo.id<<"]"<<str<<",size:"<<str.toStdString().size();

    //发送消息
    if(chatInfo->m_type == TYPE_PRIVATE_CHAT){
        //私聊
        int friendId=chatInfo->m_account;
        string message=str.toStdString();
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
     }
    if(chatInfo->m_type == TYPE_GROUP_CHAT){
        //群聊
        qDebug()<<"发送群聊信息";
        int groupId=chatInfo->m_account;
        string message=str.toStdString();
        json js;
        js["msgid"] = GROUP_CHAT_MSG;
        js["id"]=_userInfo.id;
        js["name"]=_userInfo.name;
        js["groupid"] = groupId;
        js["msg"]=str.toStdString();
        js["time"]=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
        QString sendMsg=QString::fromStdString(js.dump());
        QByteArray request=sendMsg.toUtf8();
        SendMessage(request);//向服务器发送聊天消息
    }
}

void MainWindow::on_btn_addFriend_clicked(){
    addFriendDlg->dialogClear();
    addFriendDlg->show();
    if(addFriendDlg->exec()==QDialog::Accepted){
        int index=addFriendDlg->getCurrentIndex();
        if(index==0){
            qDebug()<<"执行添加好友页";
            AddFriendInfo addFriendInfo = addFriendDlg->getAddFriendInfo();
            addFriendInfo.myId = _userInfo.id;
            qDebug()<<"待添加好友账号:"<<addFriendInfo.friendId;
            json js;
            js["msgid"] = ADD_FRIEND_REQUEST;
            js["id"] = _userInfo.id;
            js["toid"] = addFriendInfo.friendId;
            js["requestInfo"] = addFriendInfo.requestInfo;
            QString sendMsg=QString::fromStdString(js.dump());
            QByteArray request=sendMsg.toUtf8();
            SendMessage(request);
        }
        else if(index==1){
            qDebug()<<"执行添加群组页";
            AddGroupInfo addGroupInfo = addFriendDlg->getAddGroupInfo();
            addGroupInfo.myId=_userInfo.id;
            qDebug()<<"待添加群组账号："<<addGroupInfo.groupId;
            json js;
            js["msgid"]=ADD_GROUP_REQUEST;
            js["groupid"]=addGroupInfo.groupId;
            js["id"]=_userInfo.id;
            js["requestInfo"]=addGroupInfo.requestInfo;
            QString sendMsg=QString::fromStdString(js.dump());
            QByteArray request=sendMsg.toUtf8();
            SendMessage(request);
        }
        else if(index==2){
            qDebug()<<"执行创建群组页";
            /*
            json js;
            js["msgid"] = CREATE_GROUP_MSG;
            js["id"] = g_currentUser.getId();
            js["groupname"] = groupname;
            js["groupdesc"] = groupdesc;
            */
            CreateGroupInfo createGroupInfo=addFriendDlg->getCreateGroupInfo();
            qDebug()<<"创建的群组名为："<<QString::fromStdString(createGroupInfo.m_groupName);
            qDebug()<<"创建群组描述为："<<QString::fromStdString(createGroupInfo.m_groupDesc);
            json js;
            js["msgid"] = CREATE_GROUP_MSG;
            js["id"] = _userInfo.id;
            js["groupname"] = createGroupInfo.m_groupName;
            js["groupdesc"] = createGroupInfo.m_groupDesc;
            QString sendMsg=QString::fromStdString(js.dump());
            QByteArray request=sendMsg.toUtf8();
            SendMessage(request);
            qDebug()<<"发送数据成功";
        }
    }
//    if(addFriendDlg->exec() == QDialog::Accepted)
//    {
//        qDebug()<<"我邓锐又回来了";
//        AddFriendInfo addFriendInfo = addFriendDlg->getAddFriendInfo();
//        addFriendInfo.myId = _userInfo.id;
//        qDebug()<<"待添加好友账号:"<<addFriendInfo.friendId;
//        json js;
//        js["msgid"] = ADD_FRIEND_REQUEST;
//        js["id"] = _userInfo.id;
//        js["toid"] = addFriendInfo.friendId;
//        js["requestInfo"] = addFriendInfo.requestInfo;
//        QString sendMsg=QString::fromStdString(js.dump());
//        QByteArray request=sendMsg.toUtf8();
//        SendMessage(request);
//    }
}

void MainWindow::getFriendId(int friendId){
    qDebug()<<"getFriendId："<<friendId;
    json js;
    js["msgid"]=Query_FRIEND_MSG;
    js["friendid"]=friendId;
    QString sendMsg=QString::fromStdString(js.dump());
    QByteArray request=sendMsg.toUtf8();
    SendMessage(request);
//    addFriendDlg->setAddFriendInfo(friendId);
}

void MainWindow::getGroupId(int groupId){
    qDebug()<<"getGroupId: "<<groupId;
    json js;
    js["msgid"]=Query_GROUP_MSG;
    js["groupid"]=groupId;
    QString sendMsg=QString::fromStdString(js.dump());
    QByteArray request=sendMsg.toUtf8();
    SendMessage(request);
}

void MainWindow::showMyFileMessage(QString file){
    int currentRow = ui->listWidget_info->currentRow();
    if(currentRow < 0){
        qDebug()<<"未选择聊天窗口";
        return;
    }
    else{
        qDebug()<<"当前聊天窗口为：";
        qDebug()<<currentRow;
    }
    int isfind = 0;
    int currentIndex = 0;
    QListWidget* chatWidget = NULL;
    chatWidgetInfo* chatInfo = NULL;
    //根据当前选中的listWidget的编号，找到对应的chatInfo
    for(listChatWidgetInfo::iterator iter = chatWidgetInfoList.begin();iter!=chatWidgetInfoList.end();iter++,currentIndex++){
        if(currentIndex == currentRow){
            chatInfo = *iter;
            isfind = 1;
        }
    }
    if(isfind){
    }
    else {
        qDebug()<<"no friend";
        return ;
    }
    //根据chatInfo的好友账号信息，找到chatWidgetMap对应的好友聊天窗口
    string materStr;
    if(chatInfo->m_type==TYPE_PRIVATE_CHAT){
        materStr="user"+to_string(chatInfo->m_account);
    }
    if(chatInfo->m_type==TYPE_GROUP_CHAT){
        materStr="group"+to_string(chatInfo->m_account);
    }
    mapChatWidget::iterator iter = chatWigetMap.find(materStr);////这里可能出错了
    if(iter!=chatWigetMap.end()){
        chatWidget = iter->second;
    }
    else {
        qDebug()<<"not find..";
        return;
    }
    qDebug()<<"测试文件路径为："<<file;
//    qDebug()<<"测试图片路径为："<<img;
    ItemWidget* item=new ItemWidget(this);
    item->setColor("rgb(137,214,146)");
    int height=item->SetMyFile(QString("我"),":/img/defaultimg.png",file);
    QListWidgetItem * line = new QListWidgetItem;
    line->setSizeHint(QSize(350, height));
    chatWidget->addItem(line);
    chatWidget->setItemWidget(line, item);
    chatWidget->scrollToBottom();
}

void MainWindow::showMyImgMessage(QString img){//发送框内显示图片
    int currentRow = ui->listWidget_info->currentRow();
    if(currentRow < 0){
        qDebug()<<"未选择聊天窗口";
        return;
    }
    else{
        qDebug()<<"当前聊天窗口为：";
        qDebug()<<currentRow;
    }
    int isfind = 0;
    int currentIndex = 0;
    QListWidget* chatWidget = NULL;
    chatWidgetInfo* chatInfo = NULL;
    //根据当前选中的listWidget的编号，找到对应的chatInfo
    for(listChatWidgetInfo::iterator iter = chatWidgetInfoList.begin();iter!=chatWidgetInfoList.end();iter++,currentIndex++){
        if(currentIndex == currentRow){
            chatInfo = *iter;
            isfind = 1;
        }
    }
    if(isfind){
    }
    else {
        qDebug()<<"no friend";
        return ;
    }
    //根据chatInfo的好友账号信息，找到chatWidgetMap对应的好友聊天窗口
    string materStr;
    if(chatInfo->m_type==TYPE_PRIVATE_CHAT){
        materStr="user"+to_string(chatInfo->m_account);
    }
    if(chatInfo->m_type==TYPE_GROUP_CHAT){
        materStr="group"+to_string(chatInfo->m_account);
    }
    mapChatWidget::iterator iter = chatWigetMap.find(materStr);////这里可能出错了
    if(iter!=chatWigetMap.end()){
        chatWidget = iter->second;
    }
    else {
        qDebug()<<"not find..";
        return;
    }
    qDebug()<<"测试图片路径为："<<img;
    ItemWidget* item=new ItemWidget(this);
    item->setColor("rgb(137,214,146)");
    int height=item->SetMyImg(QString("我"),":/img/defaultimg.png",img);
    qDebug()<<"fuck your monther";
    QListWidgetItem * line = new QListWidgetItem;
    line->setSizeHint(QSize(350, height));
    chatWidget->addItem(line);
    chatWidget->setItemWidget(line, item);
    chatWidget->scrollToBottom();
}

void MainWindow::on_btn_img_clicked(){
    qDebug()<<"我思故我在";
    qDebug()<<"fuck ntm";
    int currentRow = ui->listWidget_info->currentRow();
    if(currentRow < 0){
        return;
    }
    else{
    }
    int currentIndex = 0;
    chatWidgetInfo* chatInfo = NULL;
    //根据当前选中的listWidget的编号，找到对应的chatInfo
    for(listChatWidgetInfo::iterator iter = chatWidgetInfoList.begin();iter!=chatWidgetInfoList.end();iter++,currentIndex++){
        if(currentIndex == currentRow){
            chatInfo = *iter;
        }
    }
    if(chatInfo){
        qDebug()<<"当前好友id为："<<chatInfo->m_account;
//        connect(w,SIGNAL(readyShowMyIMG(QString)),this,SLOT(showMyImgMessage(QString)));
        if(chatInfo->m_type==TYPE_PRIVATE_CHAT){
            cImag->setFriendInfo(_userInfo.id,_userInfo.name,chatInfo->m_account);
        }
        if(chatInfo->m_type==TYPE_GROUP_CHAT){
            cImag->setGroupInfo(_userInfo.id,_userInfo.name,chatInfo->m_account);
        }
        cImag->show();
    }
}

void MainWindow::on_btn_file_clicked(){//点击发送文件按钮
    int currentRow = ui->listWidget_info->currentRow();
    if(currentRow < 0){
        return;
    }
    else{
    }
    int currentIndex = 0;
    chatWidgetInfo* chatInfo = NULL;
    //根据当前选中的listWidget的编号，找到对应的chatInfo
    for(listChatWidgetInfo::iterator iter = chatWidgetInfoList.begin();iter!=chatWidgetInfoList.end();iter++,currentIndex++){
        if(currentIndex == currentRow){
            chatInfo = *iter;
        }
    }
    if(chatInfo){
        qDebug()<<"当前好友id为："<<chatInfo->m_account;
//        connect(w,SIGNAL(readyShowMyIMG(QString)),this,SLOT(showMyImgMessage(QString)));
        //cFile->setFriendInfo(_userInfo.id,_userInfo.name,chatInfo->m_account);
        //cFile->show();
        if(chatInfo->m_type==TYPE_PRIVATE_CHAT){
            cFile->setFriendInfo(_userInfo.id,_userInfo.name,chatInfo->m_account);
        }
        if(chatInfo->m_type==TYPE_GROUP_CHAT){
            cFile->setGroupInfo(_userInfo.id,_userInfo.name,chatInfo->m_account);
        }
        cFile->show();
    }
}

void MainWindow::on_voiceButton_clicked(bool checked)
{
    checked = !(newVoice == NULL);
    if(checked)//已经点击了语音聊天按钮，再次点击就关闭
    {
        newVoice->close();
        delete newVoice;
        newVoice = NULL;
        ui->voiceButton->setStyleSheet("QPushButton {\
                                        border-image: url(:/img/novoice.png);\
                                        border: none;\
                                        min-width: 20px;\
                                        max-width: 20px;\
                                        min-height: 20px;\
                                        max-height: 20px;}");
    }
    else//发起语音聊天
    {
        //发送语音聊天请求
        int currentRow = ui->listWidget_info->currentRow();
        if(currentRow < 0){
            return;
        }
        int isfind = 0;
        int currentIndex = 0;
        chatWidgetInfo* chatInfo = NULL;
        //根据当前选中的listWidget的编号，找到对应的chatInfo
        for(listChatWidgetInfo::iterator iter = chatWidgetInfoList.begin();iter!=chatWidgetInfoList.end();iter++,currentIndex++){
            if(currentIndex == currentRow){
                chatInfo = *iter;
                isfind = 1;
            }
        }
        if(isfind){
        }
        else {
            qDebug()<<"no friend";
            return ;
        }
        if(chatInfo->m_type == TYPE_PRIVATE_CHAT){
            //私聊
            int friendId=chatInfo->m_account;
            json js;
            js["msgid"] = ONE_VOICE_CHAT;
            js["id"]=_userInfo.id;
            js["name"]=_userInfo.name;
            js["toid"]=friendId;
            QString sendMsg=QString::fromStdString(js.dump());
            QByteArray request=sendMsg.toUtf8();
            SendMessage(request);//向服务器发送聊天消息
         }
        if(chatInfo->m_type == TYPE_GROUP_CHAT){
            //群聊
            qDebug()<<"发送群聊信息";
            int groupId=chatInfo->m_account;
            json js;
            js["msgid"] = GROUP_VOICE_CHAT;
            js["id"]=_userInfo.id;
            js["name"]=_userInfo.name;
            js["groupid"] = groupId;
            GroupInfo groupInfo;
            bool flag=false;//标志是否找到群组
            for(GroupInfo &iter:g_currentUserGroupList){
                if(iter.m_account==groupId){
                    groupInfo=iter;
                    flag=true;
                }
            }
            if(!flag)
            {
                qDebug()<<"没有找到对应群组";
            }
            else
                js["groupname"]=groupInfo.m_groupName;
            QString sendMsg=QString::fromStdString(js.dump());
            QByteArray request=sendMsg.toUtf8();
            SendMessage(request);//向服务器发送聊天消息
        }





        ui->voiceButton->setStyleSheet("QPushButton {\
                                        border-image: url(:/img/novoice2.png);\
                                        border: none;\
                                        min-width: 20px;\
                                        max-width: 20px;\
                                        min-height: 20px;\
                                        max-height: 20px;}");
        newVoice = new Voice(this);
        newVoice->show();
        newVoice->move((QApplication::desktop()->width()-newVoice->width())/12,(QApplication::desktop()->height()-newVoice->height())/3);
    }
}

void MainWindow::on_btn_update_clicked(){//点击修改用户信息按钮
    qDebug()<<"点击修改用户信息按钮";
    UpdateInfo updateInfo(_userInfo.id,_userInfo.pwd);
    updateInfo.show();
    if(updateInfo.exec() == QDialog::Accepted){
        json response;
        response["msgid"]=UPDATE_USER_INFO;
        response["id"]=_userInfo.id;
        response["password"]=updateInfo.getPassword();
        QString responseSend=QString::fromStdString(response.dump());
        SendMessage(responseSend.toUtf8());
    }
}

#include "itemwidget.h"
#include "ui_itemwidget.h"

ItemWidget::ItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemWidget)
{
    ui->setupUi(this);
    ui->widget->setStyleSheet("QWidget{background-color:rgba(255,255,255,0);border:none;}");
    countLine = 0;
    color = "rgb(254,208,142)";
}

ItemWidget::~ItemWidget()
{
    delete ui;
}

void ItemWidget::setColor(QString c)
{
    color = c;
}
int ItemWidget::SetMyData(QString name, QString picon, QString txt)
{
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(500,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    idlab->setText(name);
    idlab->setGeometry(480-fm.horizontalAdvance(name),10,fm.horizontalAdvance(name),fm.height());

    //文本
    QLabel* txtlab = new QLabel(this);
    txtlab->setStyleSheet("QLabel{color:#c0392b;\
                              background-color:"+color+";border:5px solid "+color+";\
                              border-top-left-radius:15px;\
                              border-bottom-left-radius:15px;\
                              border-bottom-right-radius:15px;}");
    int txtheight = fm.height();
    countLine = 0;
    longestWidth = 0;
    txt = Splite(txt,fm);
    qDebug()<<countLine;
    txtlab->setText(txt);
    txtlab->setGeometry(480-longestWidth-30, 30, longestWidth+30, txtheight*countLine+40);
    txtlab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    txtlab->setWordWrap(true);
    return 30+10+txtheight*countLine+40;
}

int ItemWidget::SetFriendsData(QString name, QString picon, QString txt)
{
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(10,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    idlab->setText(name);
    idlab->setGeometry(70,10,fm.horizontalAdvance(name)+20,fm.height());

    //文本
    QLabel* txtlab = new QLabel(this);
    txtlab->setStyleSheet("QLabel{color:#8e44ad;\
                            background-color:rgb(241, 196, 15);border:2px solid "+color+";\
                            border-top-right-radius:15px;\
                            border-bottom-left-radius:15px;\
                            border-bottom-right-radius:15px}");
    int txtheight = fm.height();
    countLine = 0;
    longestWidth = 0;
    countLine = 0;
    txt = Splite(txt,fm);
    qDebug()<<countLine;
    txtlab->setText(txt);
    txtlab->setGeometry(70, 30, longestWidth+30, txtheight*countLine+35);
    txtlab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    txtlab->setWordWrap(true);
    return 30+10+txtheight*countLine+35;
}

QString ItemWidget::Splite(QString txt, QFontMetrics& font)
{
    int txtwidth = txt.size(); //当前字符串长度
    qDebug()<<txt.indexOf("\n");
    if(txt.indexOf("\n") != -1)
    {
        QString qstrLeftData = txt.left(txt.indexOf("\n"));
        qDebug()<<qstrLeftData;
        QString qstrMidData = txt.mid(txt.indexOf("\n")+1);
        qDebug()<<qstrMidData;
        return Splite(qstrLeftData, font) + "\n" + Splite(qstrMidData, font);
    }
    int txtwidth2 = font.horizontalAdvance(txt); //当前字符串所占像素长度
    //qDebug()<<txtwidth;
    qDebug()<<txtwidth2;
    if(txtwidth2<=200)  //不需要继续划分
    {
        countLine++;
        if(txtwidth2>longestWidth)
            longestWidth = txtwidth2;
        qDebug()<<longestWidth;
        return txt;
    }
    else
    {
        countLine++;
        longestWidth = 200;
        QString qstrLeftData = txt.left(txtwidth*200/txtwidth2);
        qDebug()<<qstrLeftData;
        QString qstrMidData = txt.mid(txtwidth*200/txtwidth2);
        qDebug()<<qstrMidData;
        return qstrLeftData + "\n" + Splite(qstrMidData, font);
    }
}
int ItemWidget::SetMyImg(QString name,QString picon ,QString imgpth)
{
    imgpath="";
    imgpath=imgpth;
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(500,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    idlab->setText(name);
    idlab->setGeometry(480-fm.horizontalAdvance(name),10,fm.horizontalAdvance(name),fm.height());

    //图片
    qDebug()<<"加载图片ing";
    QToolButton* img = new QToolButton(this);
    img->setStyleSheet("QToolButton{border:none}");
    img->setToolButtonStyle(Qt::ToolButtonIconOnly);
    img->setIcon(QIcon(imgpth));
    qDebug()<<"rnm: "+imgpth;
//    QImage* pimg = new QImage(imgpth);
    int height=150;
    QImageReader reader(imgpth);
    reader.setDecideFormatFromContent(true);
    qDebug() << "Supported formats:" << QImageReader::supportedImageFormats();
    if(reader.canRead()){
        QImage pimg;
        if(reader.read(&pimg)){
            qDebug()<<"open is successful...";
            height = 150*pimg.height()/pimg.width();
        }
        else{
            qDebug()<<"open is failed..."<<reader.errorString();
        }
    }
    else{
        qDebug()<<"can not read..."<<reader.errorString();
    }
    int width = 150;
//    int height = 150*pimg->height()/pimg->width();
    qDebug()<<"你来了？";
    img->setIconSize(QSize(width,height));
    qDebug()<<"嗯！";
    img->setGeometry(480-width-30, 32, width+30, height+35);
    connect(img,SIGNAL(clicked()),this,SLOT(scaleImg()));//点击图片缩放图片
    return 30+10+height+35;
}

int ItemWidget::SetFriendsImg(QString name,QString picon ,QString imgpth)
{
    imgpath="";
    imgpath=imgpth;
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(10,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    idlab->setText(name);
    idlab->setGeometry(70,10,fm.horizontalAdvance(name)+20,fm.height());

    //图片
    QToolButton* img = new QToolButton(this);
    img->setStyleSheet("QToolButton{border:none}");
    img->setToolButtonStyle(Qt::ToolButtonIconOnly);
    img->setIcon(QIcon(imgpth));
    QImage* pimg = new QImage(imgpth);
    int width = 150;
    int height = 150*pimg->height()/pimg->width();
    img->setIconSize(QSize(width,height));
    img->setGeometry(70, 32, width+30, height+35);
    connect(img,SIGNAL(clicked()),this,SLOT(scaleImg()));//点击图片缩放图片
    return 30+10+height+35;
}

void ItemWidget::scaleImg(){
    qDebug()<<"点击缩放按钮";
    QImage img(imgpath);   
    QRect rect = this->geometry();//获取当前窗口坐标及大小 x、y、w、h
    //通过自定义的窗口显示图片
    //ImageWin *showImageWidget = new ImageWin(imgpath, QRect(this->pos().x(), this->pos().y(), img.width(), img.height()));
    ImageWin *showImageWidget = new ImageWin(imgpath, QRect(rect.x(), rect.y()+rect.y()*1/4, img.width(), img.height()));
    showImageWidget->show();
}

int ItemWidget::SetMyFile(QString name,QString picon ,QString Filename){
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(500,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    idlab->setText(name);
    idlab->setGeometry(480-fm.horizontalAdvance(name),10,fm.horizontalAdvance(name),fm.height());

    //图片
    //文本
    filename = Filename;
    qDebug()<<"fuck 文件名为："<<Filename;
    QToolButton* File = new QToolButton(this);
    File->setText(Filename);
    File->setStyleSheet("QToolButton{text-decoration:underline;\
                            background-color:rgb(255,255,255);border:2px solid "+color+";\
                            border-top-left-radius:15px;\
                            border-top-right-radius:15px;\
                            border-bottom-left-radius:15px;\
                            border-bottom-right-radius:15px}");
    File->setToolButtonStyle(Qt::ToolButtonIconOnly);
    int txtheight = fm.height();
    countLine = 0;
    longestWidth = 0;
    Splite(Filename,fm);
    File->setGeometry(480-longestWidth-30, 30, longestWidth+30, txtheight*countLine+40);
//    connect(File,SIGNAL(clicked()),this,SLOT(downloadFile()));//点击文件可以下载
    return 30+10+txtheight*countLine+40;
}

int ItemWidget::SetFriendsFile(QString name,QString picon,QString Filename)
{
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(10,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    idlab->setText(name);
    idlab->setGeometry(70,10,fm.horizontalAdvance(name)+20,fm.height());

    //文本
    filename = Filename;
    qDebug()<<"fuck 文件名为："<<Filename;
    QToolButton* File = new QToolButton(this);
    File->setText(Filename);
    File->setStyleSheet("QToolButton{text-decoration:underline;\
                            background-color:rgb(255,255,255);border:2px solid "+color+";\
                            border-top-right-radius:15px;\
                            border-bottom-left-radius:15px;\
                            border-bottom-right-radius:15px}");
    File->setToolButtonStyle(Qt::ToolButtonIconOnly);
    int txtheight = fm.height();
    File->setGeometry(70, 32, fm.horizontalAdvance(Filename)+30, txtheight+35);
    connect(File,SIGNAL(clicked()),this,SLOT(downloadFile()));//点击文件可以下载
    return 30+10+txtheight+35;
}

void ItemWidget::downloadFile(){
    qDebug()<<"另存文件";
    qDebug()<<filename;
    //获得另存地址
    QString FileName = QFileDialog::getSaveFileName(this, tr("另存"),"./rnm/files/"+filename);
    if(FileName==""){
        return;
    }
    qDebug()<<FileName;
    //打开缓存区的文件
    QFile file("./rnm/files/"+filename);
    file.open(QIODevice::ReadOnly);
    QByteArray block = file.readAll();
    file.close();
    //将缓存区的文件存入另存地址中
    QFile file_local(FileName);
    if(file_local.open(QIODevice::WriteOnly)){
        file_local.write(block);
        file_local.close();
    }
    QMessageBox::warning(this,tr("提示"),tr("另存文件成功"));
}

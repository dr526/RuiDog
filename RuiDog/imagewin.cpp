#include "imagewin.h"
#include "ui_imagewin.h"

ImageWin::ImageWin(QString imgpth, QRect rect, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageWin)
{
    ui->setupUi(this);

    qApp->installEventFilter(this);
    this->setGeometry(rect);//设置显示图片窗口的x、y、w、h
    this->setFixedSize(rect.width(), rect.height());//设置窗口固定大小
    this->setWindowIcon(QIcon(imgpth));
    this->setWindowTitle("查看图片");
    this->setWindowModality(Qt::ApplicationModal);//阻塞除当前窗体外的其他所有窗体

    //通过QLabel加载item上的图片
    QLabel *lab = new QLabel(this);
    lab->setFixedSize(this->width(), this->height());
    lab->setPixmap(QIcon(imgpth).pixmap(QSize(this->width(), this->height())).scaled(lab->width(),lab->height()));
}

ImageWin::~ImageWin()
{
    delete ui;
}

//事件过滤器，实现点击窗口任意位置，窗口就可以关闭
bool ImageWin::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == QEvent::MouseButtonPress)
    {
        this->close();
    }
    return QWidget::eventFilter(obj, e);
}

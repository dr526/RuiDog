#include "addgroupalert.h"
#include "ui_addgroupalert.h"

AddGroupalert::AddGroupalert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddGroupalert)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(this->windowFlags()|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);//设置向哪个方向产生阴影效果，（0,0）表示向四周发散
    shadow->setColor(QColor(38, 78, 119,255));//阴影颜色
    shadow->setBlurRadius(20); //模糊度
    ui->widget->setGraphicsEffect(shadow);
}

AddGroupalert::~AddGroupalert()
{
    delete ui;
}
void AddGroupalert::setAddRequest(QString addRequest){
    ui->addRequest->setText(addRequest);
}

void AddGroupalert::on_agree_clicked()
{
    return accept();
}


void AddGroupalert::on_refuse_clicked()
{
    this->close();
}


void AddGroupalert::on_toolButton_clicked()
{
    this->close();
}

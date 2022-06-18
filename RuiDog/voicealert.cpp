#include "voicealert.h"
#include "ui_voicealert.h"

VoiceAlert::VoiceAlert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VoiceAlert)
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

VoiceAlert::~VoiceAlert()
{
    delete ui;
}

void VoiceAlert::setMsgInfo(QString message){
    ui->addRequest->clear();
    ui->addRequest->setText(message);
}

void VoiceAlert::on_agree_clicked(){
    return accept();
}
void VoiceAlert::on_refuse_clicked(){
    return reject();
}
void VoiceAlert::on_toolButton_clicked(){
    this->close();
}

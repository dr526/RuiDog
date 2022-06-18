#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QDebug>
#include <QToolButton>
#include <QImageReader>
#include <QFileDialog>
#include <QMessageBox>

#include "imagewin.h"

namespace Ui {
class ItemWidget;
}

class ItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidget(QWidget *parent = nullptr);
    ~ItemWidget();

    int SetMyData(QString name, QString picon, QString txt); //用户消息框
    int SetFriendsData(QString name,QString picon, QString txt); //用户消息框
    void setColor(QString c);
    int SetMyImg(QString name,QString picon ,QString imgpth);
    int SetFriendsImg(QString name,QString picon ,QString imgpth);
    int SetMyFile(QString name,QString picon ,QString Filename);
    int SetFriendsFile(QString name,QString picon,QString Filename);

private slots:
    void downloadFile();//另存文件
    void scaleImg();//缩放图片

private:
    Ui::ItemWidget *ui;
    QString Splite(QString txt, QFontMetrics& font);
    int countLine;
    int longestWidth;
    QString color; //气泡颜色
    QString filename;
    QString imgpath;
};

#endif // ITEMWIDGET_H

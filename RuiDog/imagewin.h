#ifndef IMAGEWIN_H
#define IMAGEWIN_H

#include <QWidget>
#include <QIcon>
#include <QLabel>

namespace Ui {
class ImageWin;
}

class ImageWin : public QWidget
{
    Q_OBJECT

public:
    explicit ImageWin(QString imgpth, QRect rect, QWidget *parent = nullptr);
    ~ImageWin();

private:
    Ui::ImageWin *ui;

protected:
    bool eventFilter(QObject *obj, QEvent *e);//事件过滤器，实现点击窗口任意位置，窗口就可以关闭
};

#endif // IMAGEWIN_H

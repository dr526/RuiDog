#ifndef VOICE_H
#define VOICE_H

#include <QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QAudio>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>

/*
 * UDP组播
 * https://www.cnblogs.com/doker/p/11149713.html
 * https://blog.csdn.net/sjp1992/article/details/119533357
*/


namespace Ui {
class Voice;
}

class Voice : public QMainWindow
{
    Q_OBJECT

public:
    explicit Voice(QWidget *parent = 0);
    ~Voice();

private:
    Ui::Voice *ui;
    QUdpSocket *udpSocket;//用于音频信号的发送
    QString ip;
    QAudioInput *input;
    QAudioOutput *output;
    QIODevice *inputDevice;
    QIODevice *outputDevice;
    static int i;
    QString getIP();

    struct video
    {
        char audiodata[1024];
        int lens;
    };

private slots:
    void onReadyRead();
    void ReadyReadSlot();
};

#endif // VOICE_H

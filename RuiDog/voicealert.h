#ifndef VOICEALERT_H
#define VOICEALERT_H

#include <QDialog>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class VoiceAlert;
}

class VoiceAlert : public QDialog
{
    Q_OBJECT

public:
    explicit VoiceAlert(QWidget *parent = nullptr);
    ~VoiceAlert();
    void setMsgInfo(QString message);

private slots:
    void on_agree_clicked();
    void on_refuse_clicked();
    void on_toolButton_clicked();

private:
    Ui::VoiceAlert *ui;
};

#endif // VOICEALERT_H

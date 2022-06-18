#ifndef FILEMETA_H
#define FILEMETA_H

#include <QObject>
#include <QDateTime>

class FileMeta
{
public:
    //在生成文件时指定user
    explicit FileMeta() = default;
    explicit FileMeta(int userId, QString original_name = "");

    QString toQString();
    static FileMeta fromQByteArray(QByteArray msg_content);

    QString getUniqueName(){ return unique_name;};
    QString getExtendedName(){ return extended_name;};
    QString getBaseName(){return original_name;};
    QString GetDateTimeWithNoSpace(){
            return (QDateTime::currentDateTime()).toString("yyyy_MM_dd_hh_mm_ss_ms");
        }

private:

    //由于文件名不能含有冒号 ":", 因此可以以此来作为分隔
    QString unique_name;
    QString original_name;
    QString extended_name;

};

#endif // FILEMETA_H

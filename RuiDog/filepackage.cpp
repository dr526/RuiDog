#include "filepackage.h"

#include <QtNetwork>

QByteArray FilePackage::toQByteArray(){

    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);

    int size_index = block.size();
    out << int(0);
    out << info.toQString();
    out.device()->seek(size_index);
    out << (int)(block.size() - size_index - sizeof(int));
    out.device()->seek(block.size());

    size_index = block.size();
    out << int(0);
    out << content;
    out.device()->seek(size_index);
    out << (int)(block.size() - size_index - sizeof(int));
    out.device()->seek(block.size());

    out << int(sender);
    out << int(receiver);

    return block;

}
FilePackage FilePackage::fromQByteArray(QByteArray data){
    QDataStream in(data);
    in.setVersion(QDataStream::Qt_5_12);
    FilePackage tmp_file;

    int size;
    in >> size;
    QByteArray tmpMeta;
    tmpMeta.resize(size);
    in >> tmpMeta;
    tmp_file.info = FileMeta::fromQByteArray(tmpMeta);

    in >> size;
    QByteArray tmpContent;
    tmpContent.resize(size);
    in >> tmpContent;
    tmp_file.content = tmpContent;

    int tmp;
    in >> tmp;
    tmp_file.sender = tmp;
    in >> tmp;
    tmp_file.receiver = tmp;

    return tmp_file;
}

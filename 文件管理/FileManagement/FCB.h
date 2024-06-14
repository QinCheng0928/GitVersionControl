#ifndef FCB_H
#define FCB_H

#include<QString>
#include <QDateTime>
#include <QMetaType>
#include"Block.h"
#include"FAT.h"

class FCB
{
public:
    QString name;
    int start;
    int isFile;
    QDateTime createTime;
    QDateTime updateTime;

    int state;
public:
    FCB(){};
    FCB(QString name,int isFile);
    FCB(QString name,int start,int isFile,QDateTime createTime,QDateTime updateTime);

    void FCB_updata(QString newData,FAT& myfat,Block** disk);
    void FCB_delete(FAT& myfat,Block** disk);
    QString FCB_read(FAT& myfat,Block** disk);
};
Q_DECLARE_METATYPE(FCB)
#endif // FCB_H

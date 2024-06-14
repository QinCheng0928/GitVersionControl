#include "FCB.h"

FCB::FCB(QString name,int isFile)
{
    this->name=name;
    this->start=0;
    this->isFile=isFile;
    this->createTime=QDateTime::currentDateTime();
    this->updateTime=QDateTime::currentDateTime();
    this->state=0;
}

FCB::FCB(QString name,int start,int isFile,QDateTime createTime,QDateTime updateTime)
{
    this->name=name;
    this->start=start;
    this->isFile=isFile;
    this->createTime=createTime;
    this->updateTime=updateTime;
    this->state=0;
}


void FCB::FCB_updata(QString newData,FAT& myfat,Block** disk)
{
    start=myfat.FAT_write(newData,disk);
}

void FCB::FCB_delete(FAT& myfat,Block** disk)
{
    myfat.FAT_delete(start,disk);
}

QString FCB::FCB_read(FAT& myfat,Block** disk)
{
    if(-1==start)
        return "";
    else
        return myfat.FAT_read(start,disk);
}

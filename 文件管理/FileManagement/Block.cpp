#include "Block.h"


Block::Block(int i,QString str)
{
    blockIndex=i;
    data=str;
}

QString Block::Block_write(QString str)
{
    //字符串的切片操作
    data=str.mid(0,blockSize);
    return str.mid(blockSize,-1);
}

QString Block::Block_read()
{
    return data;
}

int Block::Block_ifFull()
{
    return data.length()==blockSize;
}

QString Block::Block_append(QString str)
{
    int remainSpace=blockSize-data.length();
    if(remainSpace>=str)
    {
        data+=str;
        return "";
    }
    else
    {
        data+=str.midRef(0,remainSpace);
        return str.mid(remainSpace,-1);
    }
}

void Block::Block_clear()
{
    data="";
}

QString Block::serialize() const {
    return QString::number(blockIndex) + "|" + data;
}



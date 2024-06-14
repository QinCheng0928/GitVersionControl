#ifndef FAT_H
#define FAT_H

#include"Block.h"
#include<QString>

class FAT
{



public:
    int* fat;


    FAT();
    ~FAT();

    //找一个空闲的块
    int findBlock();
    //写数据，返回文件起始位置
    int FAT_write(QString str,Block** disk);
    //删除操作
    void FAT_delete(int start,Block** disk);
    //清空以start开始的fat表以及磁盘空间,然后再重新写入
    int FAT_updata(int start,QString str,Block** disk);
    //读取从start开始的文件内容
    QString FAT_read(int start,Block** disk);
};

#endif // FAT_H

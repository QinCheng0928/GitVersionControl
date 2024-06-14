#include "FAT.h"
#include"configure.h"

#include <QMessageBox>

FAT::FAT()
{
    fat=new int[blockNum];
    //初始化fat，表示磁盘块都未使用
    //-1代表文件结束，>=0代表下一个物理块
    for(int i=0;i<blockNum;i++)
    {
        if(0==i)
            fat[i]=-1;
        else
            fat[i]=-2;
    }

}

FAT::~FAT()
{
    delete[] fat;
}

//找一个空闲的块
int FAT::findBlock()
{
    for(int i=0;i<blockNum;i++)
        if(-2==fat[i])
            return i;
    return -1;
}

//写数据，返回文件起始位置
int FAT::FAT_write(QString str,Block** disk)
{
    int start=-1;
    int curr=-1;

    while(str!="")
    {
        int nextloc=findBlock();
        if(-1==nextloc){
            QMessageBox::warning(nullptr, "警告", "磁盘空间不足!");
            return -1;
        }
        if(-1!=curr)
            fat[curr]=nextloc;
        else
            start=nextloc;

        curr=nextloc;
        str=disk[curr]->Block_write(str);
        fat[curr]=-1;
    }
    return start;
}

//删除操作
void FAT::FAT_delete(int start,Block** disk)
{
    if(-1==start)
    {
        return;
    }

    while(fat[start]!=-1)
    {
        disk[start]->Block_clear();
        int temp=fat[start];
        fat[start]=-2;
        start=temp;
    }
    fat[start]=-2;
    disk[start]->Block_clear();
}

//清空以start开始的fat表以及磁盘空间,然后再重新写入
int FAT::FAT_updata(int start,QString str,Block** disk)
{
    return this->FAT_write(str,disk);
}

//读取从start开始的文件内容
QString FAT::FAT_read(int start,Block** disk)
{
    QString data="";
    while(fat[start]!=-1)
    {
        data+=disk[start]->Block_read();
        start=fat[start];
    }
    data+=disk[start]->Block_read();
    return data;
}

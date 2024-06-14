#ifndef BLOCK_H
#define BLOCK_H
#include<QString>
#include<QStringList>
#include"configure.h"

class Block
{
private:
    int blockIndex;
    QString data;

public:
    Block(int i,QString str="");
    QString Block_write(QString str);
    QString Block_read();
    int Block_ifFull();
    QString Block_append(QString str);
    void Block_clear();
    QString serialize() const;
    static Block deserialize(const QString& str)
    {
        QStringList parts = str.split("|");
        if (parts.size() != 2) {
            return Block(-1); // 或者抛出异常
        }
        return Block(parts[0].toInt(), parts[1]);
    }

};

#endif // BLOCK_H

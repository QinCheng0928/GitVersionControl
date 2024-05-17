#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <cstdlib>
#include <ctime>

#include <QDebug>
#include<QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 使用当前时间作为种子
    std::srand(std::time(nullptr));

    col=4;
    rowPosition=0;
    rowPosition2=0;
    algorithm=!ui->radioButton->isChecked();//0是FIFO,1是LRU
    speed=1;
    memorystart=std::rand()%100+1;//随机生成内存起始地址
    Number_of_instructions_executed=0;//已执行指令数
    Number_of_missing_pages=0;
    for(int i=0;i<instructionnum;i++)
        instructions[i]=-1;
    current_random_number=instructionnum;//控制随机数取值范围的变量
    current_state=0;
    curr_ins=-1;//当前取的指令
    //保存内存页中的逻辑页号
    for(int i=0;i<4;i++)
        memory[i]=-1;
    memoryindex=0;//下一条指令要放入内存页的页号

    ui->label_memorystart1->setText(QString::number(memorystart));
    ui->label_memorystart2->setText(QString::number(memorystart+10));
    ui->label_memorystart3->setText(QString::number(memorystart+20));
    ui->label_memorystart4->setText(QString::number(memorystart+30));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//计算缺页率
double MainWindow::Calculate_percentage()
{
    if(Number_of_instructions_executed==0)
        return 0.0;
    return 1.0*Number_of_missing_pages/Number_of_instructions_executed;
}

void MainWindow::change_memoryindex()
{
    memoryindex=(memoryindex+1)%4;
}

void MainWindow::change_state_index()
{
    //current_state的范围是0-5
    current_state=(current_state+1)%6;
}

//计算指令序号
int MainWindow::Instruction_selection()
{
    if(0==current_state)
    {
        curr_ins=std::rand()%current_random_number;
        //curr_ins=319;
        if(0==curr_ins)
            curr_ins=1;
        if(319==curr_ins)
            curr_ins=318;
    }
    else if(1==current_state)
    {
        curr_ins++;
        current_random_number=curr_ins-2;
    }
    else if(2==current_state)
    {
        curr_ins=!current_random_number?0:std::rand()%current_random_number;
    }
    else if(3==current_state)
    {
        curr_ins++;
        current_random_number=curr_ins+2;
    }
    else if(4==current_state)
    {
        if(319==current_random_number)
            curr_ins=318;
        else
            curr_ins=std::rand()%(instructionnum-current_random_number)+current_random_number;
    }
    else
    {
        curr_ins++;
        current_random_number=instructionnum-1;
    }

    change_state_index();
    return curr_ins;
}


void MainWindow::update()
{
    //qDebug() << "The memoryindex is: " <<memoryindex;

    //更新与缺页率有关的数据
    ui->label_executed->setText(QString::number(Number_of_instructions_executed));
    ui->label_currentnum->setText(QString::number(Number_of_missing_pages));
    ui->label_currentpro->setText(QString::number(Calculate_percentage()*100)+'%');

    //更新与逻辑页号有关的数据
    ui->label_logicnum1->setText(memory[0]==-1?"null":QString::number(memory[0]));
    ui->label_logicnum2->setText(memory[1]==-1?"null":QString::number(memory[1]));
    ui->label_logicnum3->setText(memory[2]==-1?"null":QString::number(memory[2]));
    ui->label_logicnum4->setText(memory[3]==-1?"null":QString::number(memory[3]));

    this->show();
}


int MainWindow::LRU()
{
    int current_index=Number_of_instructions_executed-2;
    int min_index=INT_MAX;
    int min_memory=-1;
    for(int j=0;j<4;j++)
    {
        for(int i=current_index;i>=0;i--)
        {
            if(instructions[i]==memory[j])
            {
                if(i<min_index)
                {
                    min_index=i;
                    min_memory=j;
                }
                break;
            }
        }
    }
    return min_memory;
}

void MainWindow::on_singlestep_bott_clicked()
{

    //每点击一次，指令数增加1
    Number_of_instructions_executed++;
    //qDebug() << "The Number_of_instructions_executed is: " <<Number_of_instructions_executed;

    //确定算法
    algorithm=!ui->radioButton->isChecked();//0是FIFO,1是LRU

    //随机取出的指令序号
    int temp=Instruction_selection();
    //保存指令序号
    instructions[Number_of_instructions_executed-1]=temp/10;
    //qDebug() << "The Instruction_selection is: " <<temp;
    //qDebug() << "The Number_of_instructions_executed is: " <<Number_of_instructions_executed-1;

    //停止条件
    if(Number_of_instructions_executed>320)
    {
        timer->stop();
        ui->singlestep_bott->setEnabled(false);
        ui->auto_bott->setEnabled(false);
        return;
    }



    //暂存调出的逻辑页号
    int temp_logic=-1;
    //检查当前指令是否在内存中
    int i;
    for(i=0;i<4;i++)
    {
        if(memory[i]==temp/10)
            break;
    }


    if(i!=4)//内存中有当前指令
    {
        rowPosition=ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowPosition);

        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(temp));
        item1->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(rowPosition, 0, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(temp/10));
        item2->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(rowPosition, 1, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(i+1));
        item3->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(rowPosition, 2, item3);

        QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(memorystart+i*10+temp%10));
        item4->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(rowPosition, 3, item4);
        // 添加行后自动滚动到底部
        QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
        vScrollBar->setValue(vScrollBar->maximum());


    }
    else//内存中没有当前指令
    {
        Number_of_missing_pages++;

        //内存中没有当前指令分为两种情况:一是内存还未存满，二是需要替换


        //算法1和算法2只有在替换时有所不同
        if(0==algorithm||(1==algorithm&&!isfull()))//算法1  或算法2内存未满且内存中内有该指令
        {
            rowPosition=ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowPosition);

            QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(temp));
            item1->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(rowPosition, 0, item1);

            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(temp/10));
            item2->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(rowPosition, 1, item2);

            QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(memoryindex+1));
            item3->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(rowPosition, 2, item3);

            QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(memorystart+memoryindex*10+temp%10));
            item4->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(rowPosition, 3, item4);
            QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
            vScrollBar->setValue(vScrollBar->maximum());


            temp_logic=memory[memoryindex];
            memory[memoryindex]=temp/10;
            change_memoryindex();
            //没有当前指令才需要显示内存变化，如果有内存不发生变化
            rowPosition2=ui->tableWidget_2->rowCount();
            ui->tableWidget_2->insertRow(rowPosition2);

            QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(temp/10));
            item5->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(rowPosition2, 0, item5);

            QTableWidgetItem *item6 = new QTableWidgetItem(QString::number(0==memoryindex?4:memoryindex));
            item6->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(rowPosition2, 1, item6);

            QTableWidgetItem *item7 = new QTableWidgetItem(-1==temp_logic?"null":QString::number(temp_logic));
            item7->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(rowPosition2, 2, item7);
            QScrollBar *vScrollBar2 = ui->tableWidget_2->verticalScrollBar();
            vScrollBar2->setValue(vScrollBar->maximum());
            rowPosition2++;
        }
        else//算法2  内存已满且没有该指令，需要替换
        {
            //找到替被替换的内存块
            int temp_index=LRU();
            qDebug() << "The LRU index is: " <<temp_index;

            rowPosition=ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowPosition);

            QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(temp));
            item1->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(rowPosition, 0, item1);

            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(temp/10));
            item2->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(rowPosition, 1, item2);

            QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(temp_index+1));
            item3->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(rowPosition, 2, item3);

            QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(memorystart+temp_index*10+temp%10));
            item4->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(rowPosition, 3, item4);
            QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
            vScrollBar->setValue(vScrollBar->maximum());




            rowPosition2=ui->tableWidget_2->rowCount();
            ui->tableWidget_2->insertRow(rowPosition2);

            QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(temp/10));
            item5->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(rowPosition2, 0, item5);

            QTableWidgetItem *item6 = new QTableWidgetItem(QString::number(temp_index+1));
            item6->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(rowPosition2, 1, item6);

            QTableWidgetItem *item7 = new QTableWidgetItem(QString::number(memory[temp_index]));
            item7->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(rowPosition2, 2, item7);
            QScrollBar *vScrollBar2 = ui->tableWidget_2->verticalScrollBar();
            vScrollBar2->setValue(vScrollBar->maximum());
            rowPosition2++;

            memory[temp_index]=temp/10;
        }
    }
    rowPosition++;

    //每一帧更新
    this->update();
}


void MainWindow::on_reset_bott_clicked()
{
    timer->stop();
    Number_of_instructions_executed=0.0;
    Number_of_missing_pages=0.0;
    current_random_number=instructionnum;
    current_state=0;
    curr_ins=-1;
    rowPosition=0;
    rowPosition2=0;
    memorystart=std::rand()%100+1;//随机生成内存起始地址
    memoryindex=0;
    ui->label_memorystart1->setText(QString::number(memorystart));
    ui->label_memorystart2->setText(QString::number(memorystart+10));
    ui->label_memorystart3->setText(QString::number(memorystart+20));
    ui->label_memorystart4->setText(QString::number(memorystart+30));
    for(int i=0;i<4;i++)
        memory[i]=-1;
    int rowCount = ui->tableWidget->rowCount();
    for(int i = 0; i < rowCount; ++i) {
        ui->tableWidget->removeRow(0);
    }

    int rowCount2 = ui->tableWidget_2->rowCount();
    for(int i = 0; i < rowCount2; ++i) {
        ui->tableWidget_2->removeRow(0);
    }

    ui->singlestep_bott->setEnabled(true);
    ui->auto_bott->setEnabled(true);
    this->update();


}


void MainWindow::on_auto_bott_clicked()
{
    speed=ui->input->value();
    //打开计时器，当计时器超时时，会调用相应的函数
    connect(timer, &QTimer::timeout, this, &MainWindow::on_singlestep_bott_clicked);
    timer->start(1000/speed);
}

int MainWindow::isfull()
{
    for(int i=0;i<4;i++)
        if(-1==memory[i])
            return 0;
    return 1;
}









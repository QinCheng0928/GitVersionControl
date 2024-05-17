#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include<QTimer>
//指令总数
const int instructionnum=320;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
private:
    int col;
    int rowPosition;
    int rowPosition2;
    int algorithm;
    int speed;
    int memorystart;
    int Number_of_instructions_executed;
    int Number_of_missing_pages;
    int instructions[instructionnum];
    int current_random_number;
    int current_state;
    int curr_ins;

    //FIFO算法
    int memory[4];
    int memoryindex;


    QTimer *timer = new QTimer(this);

public:

    double Calculate_percentage();
    void change_state_index();
    int Instruction_selection();
    void update();
    int isfull();
    int LRU();


    //FIFO算法
    void change_memoryindex();



private slots:
    void on_singlestep_bott_clicked();
    void on_reset_bott_clicked();
    void on_auto_bott_clicked();
};
#endif // MAINWINDOW_H

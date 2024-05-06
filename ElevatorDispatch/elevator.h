#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QWidget>
#include<QPushButton>
#include<vector>
#include<QString>
#include<QFrame>
#include<QLabel>

namespace Ui {
class elevator;
}

class elevator : public QWidget
{
    Q_OBJECT

public:
    explicit elevator(QWidget *parent = nullptr);
    explicit elevator(int elevatorNum, int floor, QWidget *parent = nullptr);
    ~elevator();

private:
    Ui::elevator *ui;


public:
    std::vector<QPushButton *> bottom;
    //模拟电梯的运动
    QFrame *rectangle;
    //分别保存电梯内部、外部、所有的控制信息
    std::vector<int> out_control_information;
    std::vector<int> in_control_information;
    int totle_floor;
    int state;
    /*
     0==下降
     1==上升
     2==停止
     3==开门
     4==关门
    */
     QLabel *stateLable;
    int current_floor;
    int closed;
    int tag;


    QString updata_state();
    void Live_Update();
    void Schedule_from_stop();
    void Schedule_from_run();
    void open_door();
    void close_door();
    void updata();
    bool out_receive(int isup,int target_floor);
};

#endif // ELEVATOR_H

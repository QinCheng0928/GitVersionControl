#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include<elevator.h>

namespace Ui {
class Controller;
}

class Controller : public QWidget
{
    Q_OBJECT

public:
    explicit Controller(QWidget *parent = nullptr);
    explicit Controller(int floor,std::vector<elevator *> keep_elevators,QWidget *parent = nullptr);
    ~Controller();

private:
    Ui::Controller *ui;

public:
    std::vector<elevator *> elevators;
    std::vector<QPushButton *> keep_upbottom;
    std::vector<QPushButton *> keep_downbottom;
    int elenum;

    void Controller_main();
    void Scheduling(int isup,int target_floor);
    bool send(int isup, int target_floor, elevator *elenum);
};

#endif // CONTROLLER_H

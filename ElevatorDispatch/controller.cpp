#include "controller.h"
#include "ui_controller.h"
#include"elevator.h"
#include <QLabel>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QPushButton>
#include<QTimer>


Controller::Controller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Controller)
{
    ui->setupUi(this);
}

Controller::Controller(int floor,std::vector<elevator *> keep_elevators,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Controller)
{
    ui->setupUi(this);
    this->resize(floor*60,150);
    this->setWindowTitle("Controller");

    elevators=keep_elevators;
    elenum=keep_elevators.size();

    // 创建一个水平布局对象
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    for(int i = 0; i < floor; ++i)
    {
        // 创建一个垂直布局对象
        QVBoxLayout *verticalLayout = new QVBoxLayout;
        verticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        // 创建一个标签
        QLabel *label = new QLabel("FLOOR_"+QString::number(i+1));
        // 设置标签的字体大小
        QFont font;
        font.setPointSize(10);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        // 设置标签的文本颜色
        label->setStyleSheet("color: rgb(39,39,39);");

        // 将标签添加到垂直布局中
        verticalLayout->addWidget(label);

        // 将垂直布局添加到水平布局中
        horizontalLayout->addLayout(verticalLayout);

        // 创建上箭头按钮，并设置图标
        QPushButton *upButton = new QPushButton(QChar(0x25B2)); // 上箭头的Unicode编码是U+25B2
        verticalLayout->addWidget(upButton);
        // 创建下箭头按钮，并设置图标
        QPushButton *downButton = new QPushButton(QChar(0x25BC)); // 下箭头的Unicode编码是U+25BC
        verticalLayout->addWidget(downButton);

        connect(upButton, &QPushButton::clicked, this, [=] {
            upButton->setEnabled(false);
            Scheduling(1,i+1);
        });
        connect(downButton, &QPushButton::clicked, this, [=] {
            downButton->setEnabled(false);
            Scheduling(0,i+1);
        });
        keep_upbottom.push_back(upButton);
        keep_downbottom.push_back(downButton);
    }
    // 将水平布局添加到窗口中
    setLayout(horizontalLayout);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Controller::Controller_main);
    timer->start(1000);
}

void Controller::Scheduling(int isup,int target_floor)
{
    bool sent = false; // 记录是否有电梯发送了请求

    for (auto i : elevators) {
        if (i->tag==1&&send(isup, target_floor, i)) {
            sent = true; // 有电梯发送了请求
            break;
        }
    }

    if (!sent) {
        QTimer::singleShot(5000, this,[=]() {
            Scheduling(isup, target_floor); // 5秒后再次尝试
        });
    }

}

bool Controller::send(int isup, int target_floor, elevator *ele)
{
    return ele->out_receive(isup,target_floor);
}

void Controller::Controller_main()
{

    for(int i=0;i<elenum;i++)
    {
        keep_upbottom[elevators[i]->current_floor]->setEnabled(true);
        keep_downbottom[elevators[i]->current_floor]->setEnabled(true);
    }
}

Controller::~Controller()
{
    delete ui;
}

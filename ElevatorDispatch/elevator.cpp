#include "elevator.h"
#include "ui_elevator.h"
#include"QDesktopWidget"
#include"controller.h"
#include <QLabel>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QTimer>
#include<QElapsedTimer>

int labelWidth = 100;
int labelHeight = 30;
int labelX = 10;
int labelY = 10;

int buttonWidth = 90;
int buttonHeight = 37;
int buttonX = labelX;
int buttonY = labelY + labelHeight + 10;

elevator::elevator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::elevator)
{
    ui->setupUi(this);

}


elevator::elevator(int elevatorNum, int floor, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::elevator)
{
    ui->setupUi(this);
    state=2;
    current_floor=1;
    totle_floor=floor;
    closed=0;
    tag=1;

    // 设置窗口位置为屏幕中心
    int screenWidth = QApplication::desktop()->width();
    int screenHeight = QApplication::desktop()->height();
    int windowWidth = this->width();
    int windowHeight = this->height();
    this->move((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
    this->setWindowTitle(QString("E %1").arg(elevatorNum+1));
    this->resize(300,(floor+2)*40);


    QLabel *label = new QLabel("FLOOR",this);
    label->setFont(QFont("Arial", 12));
    label->setStyleSheet("color: rgb(39,39,39);");
    label->setFixedSize(labelWidth, labelHeight);
    label->move(labelX, labelY);



    for(int i = 0; i < floor; ++i)
    {
        QPushButton *butt = new QPushButton(QString::number(floor - i), this);
            butt->setFixedSize(buttonWidth, buttonHeight);
            butt->setStyleSheet("QPushButton {"
                                  "background-color: rgb(203,203,203);"
                                  "color: rgb(13,13,13);"
                                  "border-style: solid;"
                                  "border-width: 0px;"
                                  "border-radius: 5px;"
                                  "padding: 5px 10px;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: rgb(203,203,203);"
                                  "color: rgb(10,10,10);"
                                  "border-width: 2px;"
                                  "border-color: gray;"
                                  "}");
            butt->move(buttonX, buttonY + i * (buttonHeight + 2)); // 根据索引调整按钮位置
            connect(butt, &QPushButton::clicked, this, [=] {
                butt->setEnabled(false);
                in_control_information.push_back(floor - i);
            });

        bottom.push_back(butt);
    }

    QPushButton *opendoor = new QPushButton(QString("OPENDOOR"),this);
    QPushButton *closedoor = new QPushButton(QString("CLOSADOOR"),this);
    QPushButton *alarm = new QPushButton(QString("ALARM"),this);
    opendoor->setFont(QFont("Arial", 5)); // 设置字体为 Arial，大小为 12
    opendoor->setFixedSize(buttonWidth-5, buttonHeight);
    opendoor->setStyleSheet("QPushButton {"
                          "background-color: rgb(203,203,203);"
                          "color: rgb(13,13,13);"
                          "border-style: solid;"
                          "border-width: 0px;"
                          "border-radius: 5px;"
                          "padding: 5px 10px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: rgb(203,203,203);"
                          "color: rgb(10,10,10);"
                          "border-width: 2px;"
                          "border-color: gray;"
                          "}");
    opendoor->move(buttonX,buttonY + floor * (buttonHeight + 2));
    closedoor->setFont(QFont("Arial", 5)); // 设置字体为 Arial，大小为 12
    closedoor->setFixedSize(buttonWidth-5, buttonHeight);
    closedoor->setStyleSheet("QPushButton {"
                          "background-color: rgb(203,203,203);"
                          "color: rgb(13,13,13);"
                          "border-style: solid;"
                          "border-width: 0px;"
                          "border-radius: 5px;"
                          "padding: 5px 10px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: rgb(203,203,203);"
                          "color: rgb(10,10,10);"
                          "border-width: 2px;"
                          "border-color: gray;"
                          "}");
    closedoor->move(105,buttonY + floor * (buttonHeight + 2));
    alarm->setFont(QFont("Arial", 5)); // 设置字体为 Arial，大小为 12
    alarm->setFixedSize(buttonWidth-5, buttonHeight);
    alarm->setStyleSheet("QPushButton {"
                          "background-color: rgb(203,203,203);"
                          "color: rgb(13,13,13);"
                          "border-style: solid;"
                          "border-width: 0px;"
                          "border-radius: 5px;"
                          "padding: 5px 10px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: rgb(203,203,203);"
                          "color: rgb(10,10,10);"
                          "border-width: 2px;"
                          "border-color: gray;"
                          "}");
    alarm->move(200,buttonY + floor * (buttonHeight + 2));
    connect(opendoor, &QPushButton::clicked, this, [=] {
        if(this->state==2){
            open_door();
            state = 2;
            updata();
        }
        closed=0;
    });
    connect(closedoor, &QPushButton::clicked, this, [=] {
        if(state == 3)
        { // 如果门是打开状态, 调用关闭门的函数
            closed=1;
            close_door();
        }
    });
    connect(alarm, &QPushButton::clicked, this, [=] {
        tag=0;
        this->close();
    });


    QString context=updata_state();
    stateLable = new QLabel(context,this);
    stateLable->setFont(QFont("Arial", 12));
    stateLable->setStyleSheet("color: rgb(39,39,39);");
    stateLable->setFixedSize(150, labelHeight);
    stateLable->move(180, labelY);



    rectangle = new QFrame(this); // 父对象可以是窗口或者布局
    rectangle->setFixedSize(120, 37); // 设置长方形的大小
    rectangle->setLineWidth(0); // 设置边框宽度（可选）
    rectangle->setStyleSheet("background-color: rgb(35,167,210);");
    rectangle->move(150,buttonY + (floor-1) * (buttonHeight + 2));


    // 将标签添加到窗口
    label->show();
    //将开关门放入窗口
    opendoor->show();
    closedoor->show();
    // 将按钮添加到窗口
    for (auto &btn : bottom) {
        btn->show();
    }
    // 将状态标签添加到窗口
    stateLable->show();
    // 将长方形添加到窗口
    rectangle->show();


    //打开计时器，当计时器超时时，会调用相应的函数
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &elevator::Live_Update);
    timer->start(1000);//每隔1秒更新一次
}


//每个一段时间更新当前电梯状态
//如果有按钮信息则处理相应控制信息，如果没有按钮信息则等在下一个时钟周期再次检查
//重复上述过程
void elevator::Live_Update()
{
    //如果上升1，则当前楼层数+1
    //如果下降0，则当前楼层数-1
    if(state==0)
        current_floor--;
    if(state==1)
        current_floor++;

    state==2?Schedule_from_stop():Schedule_from_run();
    //Schedule_from_stop();
    //更新电梯状态和电梯位置
    updata();
}

void elevator::updata()
{
    //修改标签内容
    QString context=updata_state();
    stateLable->setText(context);

    //修改电梯位置
    rectangle->move(150,buttonY + (totle_floor-current_floor) * (buttonHeight + 2));
}

void elevator::Schedule_from_stop()
{
    std::vector<int> control_information;

    control_information.insert(control_information.end(), in_control_information.begin(), in_control_information.end());
    control_information.insert(control_information.end(), out_control_information.begin(), out_control_information.end());
    //当前没有按钮控制信息，无需执行任何操作
    if(control_information.empty())
        return ;

    //调整电梯状态
    int min_distance = INT_MAX;
    int target_floor = -1;
    for(auto floor : control_information){
        int distance = std::abs(current_floor - floor);
        if(distance < min_distance)
        {
            min_distance = distance;
            target_floor = floor;
        }
    }
    if(target_floor==current_floor)
    {
        open_door();
        state=2;
    }
    else if(target_floor>current_floor)
        state=1;
    else
        state=0;


    //在同一情况下，由于按下按钮后就禁用了该按钮，因此电梯内外最多各有一个使it！=end()
    //解除禁用意味着电梯到达该楼层
    auto it = std::find(in_control_information.begin(), in_control_information.end(), current_floor);
    if(it != in_control_information.end()){
        in_control_information.erase(it);
        bottom[current_floor]->setEnabled(true);
    }
    it = std::find(out_control_information.begin(), out_control_information.end(), current_floor);
    if(it != out_control_information.end()){
        out_control_information.erase(it);
        bottom[current_floor]->setEnabled(true);
    }
}

void elevator::Schedule_from_run()
{
    std::vector<int> control_information;

    control_information.insert(control_information.end(), in_control_information.begin(), in_control_information.end());
    control_information.insert(control_information.end(), out_control_information.begin(), out_control_information.end());
    if(control_information.empty())
        return ;

    bool haveUpFloor = false;
    bool haveDownFloor = false;
    for(auto i : control_information){
        if(i < current_floor)
            haveDownFloor = true;
        if(i > current_floor)
            haveUpFloor = true;
        if(i == current_floor){
            auto beforeStatus = state;
            state = 2;
            open_door();
            state = beforeStatus;
        }
    }
    auto it = std::find(in_control_information.begin(), in_control_information.end(), current_floor);
    if(it != in_control_information.end()){
        in_control_information.erase(it);
        bottom[current_floor]->setEnabled(true);
    }
    it = std::find(out_control_information.begin(), out_control_information.end(), current_floor);
    if(it != out_control_information.end()){
        out_control_information.erase(it);
        bottom[current_floor]->setEnabled(true);
    }

    //如果电梯正在上行，则没有上面按钮且有下面按钮时，才会变为下行
    //如果电梯正在下行，则没有下面按钮且有上面按钮时，才会变成上行
    //避免电梯突然转向的情况
    if(state == 1 && !haveUpFloor && haveDownFloor)
        state = 0;
    if(state == 0 && haveUpFloor && !haveDownFloor)
        state = 1;
    if(!haveUpFloor && !haveDownFloor)
        state = 2;
}

void elevator::open_door()
{
    state = 3;
    updata();
    QElapsedTimer t1;
    t1.start();
    while(t1.elapsed() < 3000) QCoreApplication::processEvents();

    if(0==closed)
    {
        state = 4; // 将状态设置为关闭
        updata(); // 更新界面
        QElapsedTimer t2;
        t2.start();
        while(t2.elapsed() < 3000) QCoreApplication::processEvents();
    }

}

void elevator::close_door()
{
    state = 4; // 将状态设置为关闭
    updata(); // 更新界面
    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed() < 3000) QCoreApplication::processEvents();
    state = 2;
    updata();
}

QString elevator::updata_state()
{
    switch(state)
    {
    case 0:
        return "DOWN";
    case 1:
        return "UP";
    case 2:
        return "STOP";
    case 3:
        return "OPEN";
    case 4:
        return "CLOSE";
    }
    return "FALSE";
}


bool elevator::out_receive(int isup,int target_floor)
{
    //电梯正在下行，电梯外按上行
    if((state==0&&isup==1&&current_floor>target_floor)||(state==1&&isup==0&&current_floor<target_floor))
        return false;

    //检查是否已经要在该层停靠
    bool hasIn = false;
    for(auto i : out_control_information)
        if(i == target_floor)
            hasIn = true;
    if(!hasIn)
        out_control_information.push_back(target_floor);

    state==2?Schedule_from_stop():Schedule_from_run();
    return true;
}


elevator::~elevator()
{
    delete ui;
    delete stateLable;
}

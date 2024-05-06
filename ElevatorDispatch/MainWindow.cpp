#include "MainWindow.h"
#include "ui_MainWindow.h"
#include"elevator.h"
#include"controller.h"
#include"QDesktopWidget"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_start_clicked()
{
    // 读取 QSpinBox 中的值
    int elevatornum = ui->input_elevator->value();
    int floor=ui->input_floor->value();
    //获取屏幕像素大小
    int screenWidth = QApplication::desktop()->width();
    int screenHeight = QApplication::desktop()->height();
    for(int i=0;i<elevatornum;i++)
    {
        elevator *elevatorclass = new elevator(i,floor);

        keep_elevator.push_back(elevatorclass);

        elevatorclass->move( 5 + i % (screenWidth / elevatorclass->width()) * (elevatorclass->width() + 5),
                           5 + ((i + 1) * elevatorclass->width() / screenWidth) * (elevatorclass->height() + 35));
        elevatorclass->show();


    }

    Controller *controller = new Controller(floor,keep_elevator);
    controller->move(screenHeight-controller->width(),100);
    controller->show();


    this->close();
}



void MainWindow::on_start_2_clicked()
{
    this->close();
}


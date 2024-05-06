#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include<vector>
#include"elevator.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_start_clicked();

    void on_start_2_clicked();

private:
    Ui::MainWindow *ui;

public:
    std::vector<elevator *> keep_elevator;

};
#endif // MAINWINDOW_H

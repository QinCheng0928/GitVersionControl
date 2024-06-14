#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <fstream>
#include<QTreeWidget>
#include <QCloseEvent>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QDir>
#include"Block.h"
#include"FAT.h"
#include"configure.h"
#include"FCB.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CreateFile;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_newfile_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_file_property_clicked();
    void deleteNodeAndChildren(QTreeWidgetItem *item);
    void on_delete_file_clicked();
    void on_open_file_clicked();
    void on_change_name_clicked();
    void on_save_clicked();
    void on_UpArrowButton_clicked();
    void on_Format_clicked();
    bool copyFile(const QString& sourcePath, const QString& destPath);
    void quit(QCloseEvent *event);
    void on_quit_clicked();

    void on_help_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

private:
    QTreeWidgetItem *curr;
    QTreeWidgetItem *root;

    Block** disk;
    FAT myFAT;
    QList<FCB> fcbList;



public:
    void read_file();
    void read_file_fat();
    void read_file_disk();
    void read_file_dir();
    void saveTreeWidgetItem(QTextStream &out, QTreeWidgetItem *item, int indentLevel);
    void update_path(QString p);
    void save();

};
#endif // MAINWINDOW_H

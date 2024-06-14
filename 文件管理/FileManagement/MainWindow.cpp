#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ui_createfile.h"

#include <QApplication>
#include <QCloseEvent>


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //返回上一级目录的图标
    ui->UpArrowButton->setIcon(QIcon(":/images/up_arrow.png"));
    //目录前小电脑的图标
    QPixmap pixmap(":/images/computer.png");
    int w = ui->computer->width()*0.8;
    int h = ui->computer->height()*0.85;
    ui->computer->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
    ui->computer->setAlignment(Qt::AlignCenter);

    //读文件
    read_file();
    this->show();
    ui->help->click();
    ui->file_content->setDisabled(true);
}


MainWindow::~MainWindow()
{
    delete ui;
    for(int i=0;i<blockNum;i++)
        delete disk[i];
    delete disk;
}

void MainWindow::quit(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "退出程序",
                                                               tr("是否将内容保存在磁盘?\n"),
                                                               QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        if (QFile::remove("copy_fat.txt"))
            qDebug() << "copy_fat.txt成功删除！";
        else
            qDebug() << "copy_fat.txt删除失败！";
        if (QFile::remove("copy_disk.txt"))
            qDebug() << "copy_disk.txt成功删除！";
        else
            qDebug() << "copy_disk.txt删除失败！";
        if (QFile::remove("copy_fcb_data.txt"))
            qDebug() << "copy_fcb_data.txt成功删除！";
        else
            qDebug() << "copy_fcb_data.txt删除失败！";
        event->accept();
    }
    else if(resBtn == QMessageBox::No)
    {
        copyFile("copy_fat.txt","fat.txt");
        copyFile("copy_disk.txt","disk.txt");
        copyFile("copy_fcb_data.txt","fcb_data.txt");
        if (QFile::remove("copy_fat.txt"))
            qDebug() << "copy_fat.txt成功删除！";
        else
            qDebug() << "copy_fat.txt删除失败！";
        if (QFile::remove("copy_disk.txt"))
            qDebug() << "copy_disk.txt成功删除！";
        else
            qDebug() << "copy_disk.txt删除失败！";
        if (QFile::remove("copy_fcb_data.txt"))
            qDebug() << "copy_fcb_data.txt成功删除！";
        else
            qDebug() << "copy_fcb_data.txt删除失败！";
        event->accept();
    }
    else
    {
        event->ignore();
    }

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->quit(event);
}

void MainWindow::read_file()
{

    read_file_fat();
    read_file_disk();
    read_file_dir();
    copyFile("fat.txt", "copy_fat.txt");
    copyFile("disk.txt", "copy_disk.txt");
    copyFile("fcb_data.txt", "copy_fcb_data.txt");
}

void MainWindow::read_file_fat()
{
    //读取fat表
    QFile file_fat("fat.txt");
    if (file_fat.exists())
    {
        if (!file_fat.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "fat表文件不存在或无法打开";
            return;
        }
        else
        {
            qDebug() << "fat表文件存在并已成功打开";
            QTextStream in(&file_fat);

            // 读取文件内容到 QStringList
            QString line = in.readLine();
            QStringList values = line.split(" ", QString::SkipEmptyParts);

            // 关闭文件
            file_fat.close();

            // 根据读取到的数据更新数组
            int count = qMin(values.size(), blockNum); // 确保不超出数组大小
            for (int i = 0; i < count; ++i) {
                myFAT.fat[i] = values[i].toInt();
            }

            // 打印读取的数组内容
            qDebug() << "fat元素个数:" << count;
        }
    }
    else
    {
        if (!file_fat.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(nullptr, "警告", "无法打开文件！");
            return;
        }
        // 创建文本流
        QTextStream out(&file_fat);
        for (int i = 0; i < blockNum; i++)
            out << myFAT.fat[i] << " ";
        file_fat.close();
        qDebug() << "fat表文件成功保存！";
    }

}

void MainWindow::read_file_disk()
{
    // 读取disk表
    QFile file_disk("disk.txt");
    disk = new Block*[blockNum];
    for (int i = 0; i < blockNum; i++)
        disk[i] = new Block(i);

    if (file_disk.exists())
    {
        if (!file_disk.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "disk表文件不存在或无法打开";
            return;
        }
        else
        {
            qDebug() << "disk表文件存在并已成功打开";
            QTextStream in(&file_disk);

            int i = 0;
            while (!in.atEnd() && i < blockNum) {
                QString line = in.readLine();
                if (!line.isEmpty()) {
                    disk[i] = new Block(Block::deserialize(line));
                    i++;
                }
            }

            file_disk.close();

            // 打印读取的数组内容
            qDebug() << "读取的Block个数:" << i;
            for (int j = 0; j < 1; j+=50) {
                qDebug() << j << ":" << disk[j]->Block_read();
            }
        }
    }
    else
    {
        if (!file_disk.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(nullptr, "警告", "无法打开文件！");
            return;
        }

        // 创建文本流
        QTextStream out(&file_disk);
        for (int i = 0; i < blockNum; i++) {
            out << disk[i]->serialize() << "\n";
        }

        file_disk.close();
        qDebug() << "disk表文件成功保存！";
    }

}

void MainWindow::read_file_dir()
{
    //读取目录
    QFile file_dir("fcb_data.txt");
    if (file_dir.exists())
    {
        if (!file_dir.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "目录文件不存在或无法打开";
            return;
        }
        else
        {
            qDebug() << "目录文件存在并已成功打开";
            QTextStream in(&file_dir);
            fcbList.clear();
            ui->treeWidget->clear();
            ui->treeWidget->setColumnCount(1);

            QMap<int, QTreeWidgetItem*> indentItemMap;
            indentItemMap[0] = ui->treeWidget->invisibleRootItem();
            curr=ui->treeWidget->invisibleRootItem();

            while (!in.atEnd()) {
                QString line = in.readLine();
                int indentLevel = 0;
                while (indentLevel < line.size() && line.at(indentLevel) == ' ') {
                    ++indentLevel;
                }
                line = line.mid(indentLevel);
                QStringList parts = line.split(",");
                if (parts.size() == 5) {
                    QString name = parts[0];
                    int start = parts[1].toInt();
                    int isFile=parts[2].toInt();
                    QDateTime createTime = QDateTime::fromString(parts[3], Qt::ISODate);
                    QDateTime updateTime = QDateTime::fromString(parts[4], Qt::ISODate);
                    FCB fcb(name, start, isFile,createTime, updateTime);
                    fcbList.append(fcb);


                    QTreeWidgetItem *newItem = new QTreeWidgetItem();
                    qDebug() <<name;
                    newItem->setText(0, name);


                    QVariant fcbVariant;
                    fcbVariant.setValue(fcb);
                    newItem->setData(0, Qt::UserRole, fcbVariant);


                    if (indentItemMap.contains(indentLevel)) {
                        indentItemMap[indentLevel]->addChild(newItem);
                    }
                    if(0==indentLevel)
                        root=newItem;

                    indentItemMap[indentLevel+1] = newItem;
                    qDebug() << "缩进:" << indentLevel<<"名称"<<newItem->text(0);
                }
            }
            file_dir.close();
        }
    }
    else
    {
        QTreeWidgetItem *temp_root = new QTreeWidgetItem(ui->treeWidget);
        temp_root->setText(0, "E");
        FCB fcb("E",0);
        curr=temp_root;
        this->root=temp_root;

        QVariant fcbVariant;
        fcbVariant.setValue(fcb);
        temp_root->setData(0, Qt::UserRole, fcbVariant);
        fcbList.append(fcb);


        qDebug() << "目录初始化";

        if (file_dir.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file_dir);
            saveTreeWidgetItem(out, ui->treeWidget->invisibleRootItem(), 0);
            file_dir.close();
            qDebug() << "目录初始化完成";
        }
        else {
            QMessageBox::warning(this, "Save to File", "Failed to open file for writing.");
        }
    }

}

void MainWindow::saveTreeWidgetItem(QTextStream &out, QTreeWidgetItem *item, int indentLevel) {
    qDebug()<<item->text(0)<<item->childCount();
    for (int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem *child = item->child(i);
        FCB fcb = child->data(0, Qt::UserRole).value<FCB>();
        out << QString(indentLevel, ' ') << fcb.name << "," << fcb.start << ","<<fcb.isFile<< ","
            << fcb.createTime.toString(Qt::ISODate) << ","
            << fcb.updateTime.toString(Qt::ISODate) << "\n";
        qDebug() << QString(indentLevel, ' ') << fcb.name << "," << fcb.start << ","<<fcb.isFile<< ","
            << fcb.createTime.toString(Qt::ISODate) << ","
            << fcb.updateTime.toString(Qt::ISODate);
        saveTreeWidgetItem(out, child, indentLevel + 1);
    }
}

void MainWindow::on_newfile_clicked()
{
    FCB temp_fcb = curr->data(0, Qt::UserRole).value<FCB>();
    if(0==ui->File->isChecked()&&0==ui->Floder->isChecked())
    {
        QMessageBox::warning(this, "警告", "请选择新建文件类型！\n\n文件/文件夹");
        return;
    }
    else if(ui->name->text()=="")
    {
        QMessageBox::warning(this, "警告", "新建文件名称不能为空！\n\n请输入文件名称！");
        return;
    }
    else if(ui->name->text()=="E")
    {
        QMessageBox::warning(this, "警告", "不能建立与E同级的目录！\n\n请切换目录再次建立！");
        return;
    }
    else if(1==temp_fcb.isFile)
    {
        QMessageBox::warning(this, "警告", "当前所在为文件，不能新建目录！\n\n请选择文件夹添加目录！");
        return;
    }
    else
    {
        int checked=ui->File->isChecked();
        QString name=ui->name->text();
        qDebug()<<curr->text(0);

        QTreeWidgetItem *temp = new QTreeWidgetItem(curr);
        temp->setText(0, name);
        FCB fcb(name,checked);
        fcbList.append(fcb);
        if(checked)
            fcb.FCB_updata("请输入内容。",myFAT,disk);
        else
            fcb.FCB_updata("",myFAT,disk);
        QVariant fcbVariant;
        fcbVariant.setValue(fcb);
        temp->setData(0, Qt::UserRole, fcbVariant);
        fcbList.append(fcb);
        save();
    }
}

void MainWindow::save()
{
    QFile file_fat("fat.txt");
    if (!file_fat.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "警告", "无法打开文件！");
        return;
    }
    // 创建文本流
    QTextStream out(&file_fat);
    for (int i = 0; i < blockNum; i++)
        out << myFAT.fat[i] << " ";
    file_fat.close();
    qDebug() << "fat表文件成功保存！";


    QFile file_disk("disk.txt");
    if (!file_disk.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "警告", "无法打开文件！");
        return;
    }
    QTextStream out_disk(&file_disk);
    for (int i = 0; i < blockNum; i++) {
        out_disk << disk[i]->serialize() << "\n";
    }
    file_disk.close();
    qDebug() << "disk表文件成功保存！";


    QFile file_dir("fcb_data.txt");
    if (file_dir.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file_dir);
        saveTreeWidgetItem(out, ui->treeWidget->invisibleRootItem(), 0);
        file_dir.close();
    }
    else {
        QMessageBox::warning(this, "Save to File", "Failed to open file for writing.");
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    //关闭文件
    QVariant variant = curr->data(0, Qt::UserRole);
    FCB last_fcb = variant.value<FCB>();
    last_fcb.state=0;
    curr->setData(0, Qt::UserRole, QVariant::fromValue(last_fcb));

    ui->file_content->setText("");
    // 获取节点的完整路径
    QString path = item->text(column);
    curr=item;

    //如果是文件夹时，禁用文本输入框
    FCB fcb=curr->data(0, Qt::UserRole).value<FCB>();
    if(0==fcb.isFile||0==fcb.state)
        ui->file_content->setDisabled(true);
    else
        ui->file_content->setDisabled(false);

    QTreeWidgetItem *parent = item->parent();
    while (parent) {
        path = parent->text(column) + " -> " + path;
        parent = parent->parent();

    }
    update_path(path);
}

void MainWindow::update_path(QString p)
{
    ui->path->setText(p);
}

void MainWindow::on_file_property_clicked()
{
    FCB fcb = curr->data(0, Qt::UserRole).value<FCB>();
    QString temp="名称："+fcb.name+"\n文件类型："+(fcb.isFile?"文件":"文件夹")+"\n";
    temp+="创建时间："+fcb.createTime.toString(Qt::ISODate)+'\n';
    temp+="修改时间："+fcb.updateTime.toString(Qt::ISODate);
    ui->file_content->setText(temp);
}

void MainWindow::deleteNodeAndChildren(QTreeWidgetItem *item)
{
    if (!item) return;

    // 递归删除子节点，并获取每一个删除的节点指针
    while (item->childCount() > 0) {
        QTreeWidgetItem *child = item->takeChild(0); // 获取并移除第一个子节点
        deleteNodeAndChildren(child); // 递归删除子节点
    }

    // 在这里可以获取被删除节点的指针，进行相关操作
    qDebug() << "Deleting node:" << item->text(0);

    QVariant variant = curr->data(0, Qt::UserRole);
    FCB time_fcb = variant.value<FCB>();
    time_fcb.FCB_delete(myFAT,disk);
    curr->setData(0, Qt::UserRole, QVariant::fromValue(time_fcb));

    delete item; // 删除当前节点
}

void MainWindow::on_delete_file_clicked()
{
    if (curr->text(0)!="E") {
        QTreeWidgetItem *parent = curr->parent();
        if (parent)
        {
            parent->removeChild(curr);
        } else
        {
            ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(curr));
        }

        deleteNodeAndChildren(curr);  // 确保释放内存和删除所有子节点
    }
    curr=root;
    ui->treeWidget->update();
    save();

}

void MainWindow::on_open_file_clicked()
{
    FCB fcb = curr->data(0, Qt::UserRole).value<FCB>();
    if(fcb.isFile==0)
    {
        QMessageBox::warning(this, "警告", "文件夹内的信息在左侧目录显示！\n\n请选择一个文件打开，查看保存在文件中的内容！");
        return;
    }

    QString con=fcb.FCB_read(myFAT,disk);
    qDebug()<<con;
    ui->file_content->setText(con);
    ui->file_content->update();

    QVariant variant = curr->data(0, Qt::UserRole);
    FCB time_fcb = variant.value<FCB>();
    time_fcb.updateTime = QDateTime::currentDateTime();
    time_fcb.state=1;
    curr->setData(0, Qt::UserRole, QVariant::fromValue(time_fcb));
    ui->file_content->setDisabled(false);
    qDebug()<<"状态："<<curr->data(0, Qt::UserRole).value<FCB>().state;
    save();
}

void MainWindow::on_change_name_clicked()
{
    QString ch_name=ui->get_change_name->text();
    if(curr->text(0)!="E"&&!ch_name.isEmpty())
    {
        QVariant variant = curr->data(0, Qt::UserRole);
        FCB fcb = variant.value<FCB>();
        fcb.name = ch_name;
        fcb.updateTime = QDateTime::currentDateTime();
        curr->setData(0, Qt::UserRole, QVariant::fromValue(fcb));
        curr->setText(0, ch_name);
    }
    else if(curr->text(0)=="E")
        QMessageBox::warning(this, "警告", "无法修改根目录的名称！");
    else
        QMessageBox::warning(this, "警告", "修改后的名称不能为空！\n\n请输入修改后的名称！");
    ui->treeWidget->update();
    save();
}

void MainWindow::on_save_clicked()
{
    QVariant variant = curr->data(0, Qt::UserRole);
    FCB fcb = variant.value<FCB>();
    if(1==fcb.isFile)
    {
        fcb.updateTime = QDateTime::currentDateTime();
        fcb.FCB_updata(ui->file_content->toPlainText(),myFAT,disk);
        fcb.state=0;
        curr->setData(0, Qt::UserRole, QVariant::fromValue(fcb));
        ui->file_content->setDisabled(true);
        qDebug()<<"状态："<<curr->data(0, Qt::UserRole).value<FCB>().state;
        save();
    }
}

void MainWindow::on_UpArrowButton_clicked()
{
    if(!curr->parent())
        return;
    on_treeWidget_itemClicked(curr->parent(), 0);
}

void MainWindow::on_Format_clicked()
{
    ui->treeWidget->clear();
    QFile::remove("fat.txt");
    QFile::remove("disk.txt");
    QFile::remove("fcb_data.txt");
    read_file_fat();
    read_file_disk();
    read_file_dir();
    ui->treeWidget->update();

}

bool MainWindow::copyFile(const QString& sourcePath, const QString& destPath)
{
    //打开源文件
    QFile sourceFile(sourcePath);
    if (!sourceFile.exists()) {
        qDebug() << "源文件不存在！" << sourcePath;
        return false;
    }
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        qDebug() << "源文件打开失败！" << sourcePath;
        return false;
    }

    //打开目标文件
    QFile destFile(destPath);
    if (!destFile.open(QIODevice::WriteOnly)) {
        qDebug() << "目标文件打开失败！" << destPath;
        sourceFile.close();
        return false;
    }

    QByteArray fileData = sourceFile.readAll();
    destFile.write(fileData);

    sourceFile.close();
    destFile.close();

    qDebug() << "文件内容从" << sourcePath << "复制到" << destPath;
    return true;
}

void MainWindow::on_quit_clicked()
{
    this->close();
}

void MainWindow::on_help_clicked()
{
    QString text;
    QMessageBox msgBox;

    msgBox.setWindowTitle("帮助");
    text="2250397秦成项目3文件管理系统\n\n";
    text+="（！！！写入文件内容之前请先打开文件！！！）\n\n";
    text+="界面展示内容说明：\n";
    text+="1、左侧目录展示所有创建的文件 和 文件系统的层次关系。\n";
    text+="2、右侧区域显示文件内容 或 写入内容 或 显示属性。\n\n";
    text+="实现功能及使用方式介绍：\n";
    text+="1、新建功能：可以新建文件或文件夹。\n";
    text+="          (1)选择文件建立位置（在目录模块点击文件建立位置）\n";
    text+="          (2)选择文件类型（文件/文件夹）\n";
    text+="          (3)输入名称\n";
    text+="          (4)点击新建按钮\n";
    text+="2、打开功能：可以打开文件并显示内容。\n";
    text+="3、属性功能：可以显示文件属性。\n";
    text+="4、删除功能：删除文件或文件夹（如果是文件夹，里面所有内容均删除）。\n";
    text+="          (1)选择需要操作的文件（在目录模块点击文件）\n";
    text+="          (2)点击相应的按钮（打开、属性、删除）\n";
    text+="5、重命名功能：可以更改文件或文件夹的名称。\n";
    text+="          (1)选择需要重命名的文件（在目录模块点击文件）\n";
    text+="          (2)输入更改后名称\n";
    text+="          (3)点击相应的按钮\n";
    text+="6、保存功能：保存输入文件的内容（不写入磁盘）。\n";
    text+="          (1)首先打开文件\n";
    text+="          (2)在右下角模块写入内容\n";
    text+="          (3)点击保存按钮\n";
    text+="7、格式化：删除所有文件（根目录除外）。\n";
    text+="8、写磁盘功能：关闭页面或点击退出时进行选择。\n";
    msgBox.setInformativeText(text);
    msgBox.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();

}


#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initkey();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initkey()
{
    //点击左边按钮
    connect(ui->pushButton_file,&QAbstractButton::clicked,
            ui->plainTextEdit,[&]{
        //QString expression = ui->plainTextEdit->toPlainText();
        QFileDialog* f = new QFileDialog(this);
        f->setWindowTitle("Select File*.*");
        f->setNameFilter("*.*");
        f->setViewMode(QFileDialog::Detail);

        QString filePath;
        if(f->exec() == QDialog::Accepted)
            filePath = f->selectedFiles()[0];
        std::string str;
        str = filePath.toLocal8Bit().data();
        //std::cout << str;
        ui->plainTextEdit->clear();
        ui->plainTextEdit->appendPlainText("待压缩文件：" + filePath);
        string* Code = Compress(str);  //调用压缩函数
        ui->plainTextEdit->appendPlainText("字符编码如下：");
        //输出每个字符的编码
        for(int i = 0; i < 256; i++){
            QString num = QString::number(i - 128);
            QString string = QString::fromStdString(Code[i]);
            ui->plainTextEdit->appendPlainText(num + ": " + string);
        }
        ui->plainTextEdit->appendPlainText("压缩完成！压缩后文件名： " + filePath + ".huf");

    });

    //点击右边按钮
    connect(ui->pushButton_decompress,&QAbstractButton::clicked,
            ui->plainTextEdit,[&]{
        //QString expression = ui->plainTextEdit->toPlainText();
        QFileDialog* f = new QFileDialog(this);
        f->setWindowTitle("Select File*.huf");
        f->setNameFilter("*.huf");
        f->setViewMode(QFileDialog::Detail);

        QString filePath;
        if(f->exec() == QDialog::Accepted)
            filePath = f->selectedFiles()[0];
        std::string str;
        str = filePath.toLocal8Bit().data();
        ui->plainTextEdit->clear();
        ui->plainTextEdit->appendPlainText("待解压文件：" + filePath);
        Decompress(str); //调用解压函数
        ui->plainTextEdit->appendPlainText("解压完成！解压后文件名： " + filePath.replace(".huf", ""));
    });



}


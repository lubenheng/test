#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "USB2UARTSPIIICDLL.h"
#include "qmessagebox.h"
#include "QString"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    unsigned char UIDBuf[12];
    unsigned int  isHsUSB;
    unsigned int  fwv;
    int rtn;

    if(OpenUsb(0)>=0)
    {
        memset(UIDBuf,0,sizeof(UIDBuf));
        rtn = GetBoardInformation(UIDBuf,&isHsUSB,&fwv,0);
        if(rtn < 0)
        {
            QMessageBox msgBox;
            msgBox.setText("没有获取到UID");
            msgBox.exec();
            return;
        }

    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("打开USB失败！");
        msgBox.exec();
        return;
    }


    unsigned char sendBuf[5] = {0x01,0x02,0x03,0x04,0x05};
    unsigned char rcvBuf[5] = {0,0,0,0,0};

    if(ConfigSPIParam(0,0,0,0) < 0)
    {
        QMessageBox msgBox;
        msgBox.setText("设置SPI主模式参参数错误！");
        msgBox.exec();
        CloseUsb(0);
        return;
    }

    if(SPISendAndRcvData(0,0,1,50,0,0xAA,sendBuf,rcvBuf,5,5,0) < 0)
    {
        QMessageBox msgBox;
        msgBox.setText("SPI发送数据错误！");
        msgBox.exec();
        CloseUsb(0);
        return;
    }

    CloseUsb(0);

}

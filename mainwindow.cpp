#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <iostream>
#include <cstdio>
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

void MainWindow::parseVideoData()
{


}

void MainWindow::parseFlvSHeader(QDataStream &in)
{
    bsbuf  = new unsigned char[6<<20];
    ptr = bsbuf;
    in.readRawData((char*)bsbuf, 6<<20);
    rptr = 0;
    wptr = 6<<20;

    // FLV
    if(bsbuf[0] == 0x46 && bsbuf[1] == 0x4C && bsbuf[2] == 0x56)
        ui->textBrowser->append("I'm FLV!!");
    else
        ui->textBrowser->append("NOT FLV GG!!");

    // Header Size
    unsigned int DataOffset = (bsbuf[5]<<24)|(bsbuf[6]<<16)|(bsbuf[7]<<8)|(bsbuf[8]);
    QString str = QString::number(DataOffset);
    ui->textBrowser->append("DataOffset = "+str);

    ptr += 9; // Header
    ptr += 4; // PreviousTagSize0 always be 0

    int n = 0;
    while(n < 50)
    {
        if(*ptr == 18)
        {
            ui->textBrowser->append("\nScript Data!");
            unsigned int scriptDataSz = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("Data size: " + QString::number(scriptDataSz));
            ptr += 8 + scriptDataSz;
        }
        else if(*ptr == 9)
        {
            ui->textBrowser->append("\nVideo Data!");
            unsigned int DataSz = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("Data size: " + QString::number(DataSz));

            unsigned int Timestamp = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr))|((*(++ptr))<<24);
            ui->textBrowser->append("Timestamp: " + QString::number(Timestamp));

            unsigned int StreamID = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("StreamID: " + QString::number(StreamID));

            unsigned int FrameType = (*(++ptr))&0xF0;
            unsigned int CodecID = (*ptr)&0xF;
            unsigned int AvcPacketType = *(++ptr);
            if(AvcPacketType == 0) // sequence
                ptr += (DataSz - 2);
            else if(AvcPacketType == 1) // NALU
                parseVideoData();
            //ptr += 8 + DataSz;
        }
        else if(*ptr == 8)
        {
            ui->textBrowser->append("\nAudio Data!");
            unsigned int DataSz = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("Data size: " + QString::number(DataSz));
            ptr += 8 + DataSz;
        }
        ptr += 4;
        n++;
    }


}

void MainWindow::open(QString& FileName)
{
    QFile file(FileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    parseFlvSHeader(in);

}

void MainWindow::on_pushButton_clicked()
{
    QString FileName;
    FileName = QFileDialog::getOpenFileName(this, "Open a Transport Stream File");
    if(!FileName.isEmpty())
    {
        ui->lineEdit->setText(FileName);
        ui->lineEdit_2->setText(FileName+".ts");
    }

    open(FileName);

}

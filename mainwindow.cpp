#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

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

void MainWindow::parseVideoData(int dataSz)
{
    char pts[5];
    while(dataSz > 0)
    {
        unsigned int PayloadSz = ((*(++ptr))<<24)|((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
        std::cout<<"Payload Size: "<<PayloadSz<<std::endl;
        out.writeRawData((const char*)pes_header,9);
        {
            pts[0] = (0x3<<4|(_Timestamp&0xC0000000)>>27|0x1)&(0x37);
        }
        ++ptr;
        out.writeRawData((const char*)ptr,PayloadSz);
        ptr += (PayloadSz-1);

        dataSz = dataSz - PayloadSz - 4;
        std::cout<<"Data Remain Size: "<<dataSz<<std::endl;
    }
    ++ptr;
}

void MainWindow::parseAVCDecoderConfigurationRecord()
{
    unsigned int Version = *(++ptr);
    unsigned int Profile = *(++ptr);
    unsigned int ProfileCompact = *(++ptr);
    unsigned int Level = *(++ptr);
    unsigned int NumBytePresentNaluSize = *(++ptr);
    NumBytePresentNaluSize &= 0x3;
    NumBytePresentNaluSize++;

    const unsigned char seq_pes_header[13]={0x0,0x0,0x01,0xE0,0x0,0x0,0x8B,0x0,0x0,0x0,0x0,0x0,0x01};
    unsigned int SpsNum = *(++ptr);
    SpsNum &= 0x1F;
    unsigned int SpsSize = (*(++ptr))<<8|(*(++ptr));
    std::cout<<"SPS Size: "<<SpsSize<<std::endl;
    out.writeRawData((const char*)seq_pes_header,13);
    ++ptr; //point to SPS NALU Type
    out.writeRawData((const char*)ptr,SpsSize);
    ptr += SpsSize;

    unsigned int PpsNum = *ptr;
    unsigned int PpsSize = (*(++ptr))<<8|(*(++ptr));
    std::cout<<"PPS Size: "<<PpsSize<<std::endl;
    out.writeRawData((const char*)seq_pes_header,13);
    ++ptr; //point to PPS NALU Type
    out.writeRawData((const char*)ptr,PpsSize);
    ptr += PpsSize;
    //outFile.close();
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
    while(n < 5000)
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

            _Timestamp = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr))|((*(++ptr))<<24);
            ui->textBrowser->append("Timestamp: " + QString::number(Timestamp));

            unsigned int StreamID = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("StreamID: " + QString::number(StreamID));

            unsigned int FrameType = (*(++ptr))&0xF0;
            unsigned int CodecID = (*ptr)&0xF;
            unsigned int AvcPacketType = *(++ptr);
            unsigned int CompositionTime = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            if(AvcPacketType == 0) // sequence
                parseAVCDecoderConfigurationRecord();
            else if(AvcPacketType == 1) // NALU
                parseVideoData(DataSz-5);
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

    outFile.setFileName(FileName+".mpg");
    outFile.open(QIODevice::WriteOnly);
    out.setDevice(&outFile);


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

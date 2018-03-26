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
    char stcode[4] = {0x0, 0x0, 0x0, 0x1};
    while(dataSz > 0)
    {
        unsigned int PayloadSz = ((*(++ptr))<<24)|((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
        std::cout<<"Payload Size: "<<PayloadSz<<std::endl;
        out.writeRawData((const char*)pes_header,9);
        {
            pts[0] = (0x2<<4|((_Timestamp&0xC0000000)>>29)|0x1)&(0xF7);
            pts[1] = (_Timestamp&0x3FC00000)>>22;
            pts[2] = ((_Timestamp&0x3F8000)>>15<<1)|0x1;
            pts[3] = (_Timestamp&0x7F80)>>14;
            pts[4] = ((_Timestamp&0x7F)<<1)|0x1;
            out.writeRawData((const char*)pts,5);
        }
        out.writeRawData((const char*)stcode,4);
        ++ptr;
        out.writeRawData((const char*)ptr,PayloadSz);
        ptr += (PayloadSz-1);

        dataSz = dataSz - PayloadSz - 4;
        std::cout<<"Data Remain Size: "<<dataSz<<std::endl;
    }
    ++ptr;
}

void MainWindow::generatePSI()
{
    //PAT
    char patHeader[4] = {0x47, 0x40, 0x0, 0x10};
    out.writeRawData((const char*)patHeader,5);
    char patSection[16] = {0x0, 0xB0, 0x0D, 0x0, 0x01, 0xC1, 0x0, 0x0, 0x0, 0x01, 0xE0, 0x30, 0xEE, 0xD2, 0xF2, 0x31};
    out.writeRawData((const char*)patSection,16);
    int i;
    for(i=0; i<(188-4-16); i++)
        out<<(quint8)0xFF;

    //PMT
    char pmtHeader[4] = {0x47, 0x40, 0x30, 0x10};
    out.writeRawData((const char*)patHeader,5);
    char pmtSection[26] = {0x02, 0xB0, 0x17, 0x0, 0x01, 0xC1, 0x0, 0x0, 0xE0, 0x31, 0xF0, 0x0, 0x02, 0xE0, 0x31, 0xF0, 0x0, 0x03, 0xE0, 0x34, 0xF0, 0x0, 0x21, 0xC5, 0x99, 0x32};
    out.writeRawData((const char*)patSection,26);
    for(i=0; i<(188-4-26); i++)
        out<<(quint8)0xFF;

}

void MainWindow::parseAVCDecoderConfigurationRecord()
{
    generatePSI();

    unsigned int Version = *(++ptr);
    unsigned int Profile = *(++ptr);
    unsigned int ProfileCompact = *(++ptr);
    unsigned int Level = *(++ptr);
    unsigned int NumBytePresentNaluSize = *(++ptr);
    NumBytePresentNaluSize &= 0x3;
    NumBytePresentNaluSize++;

    char tsHeader[4] = {0x47, 0x40, 0x31, 0x10};
    out.writeRawData((const char*)tsHeader,4);
    const unsigned char seq_pes_header[13]={0x0,0x0,0x01,0xE0,0x0,0x0,0x8B,0x0,0x0,0x0,0x0,0x0,0x01};
    unsigned int SpsNum = *(++ptr);
    SpsNum &= 0x1F;
    unsigned int SpsSize = (*(++ptr))<<8|(*(++ptr));
    std::cout<<"SPS Size: "<<SpsSize<<std::endl;
    out.writeRawData((const char*)seq_pes_header,13);
    ++ptr; //point to SPS NALU Type
    out.writeRawData((const char*)ptr,SpsSize);
    ptr += SpsSize;
    int i;
    for(i=0; i<(188-4-13-SpsSize); i++)
        out<<(quint8)0xFF;

    unsigned int PpsNum = *ptr;
    unsigned int PpsSize = (*(++ptr))<<8|(*(++ptr));
    out.writeRawData((const char*)tsHeader,4);
    std::cout<<"PPS Size: "<<PpsSize<<std::endl;
    out.writeRawData((const char*)seq_pes_header,13);
    ++ptr; //point to PPS NALU Type
    out.writeRawData((const char*)ptr,PpsSize);
    ptr += PpsSize;
    for(i=0; i<(188-4-13-PpsSize); i++)
        out<<(quint8)0xFF;
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
    while(n < 500)
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
            ui->textBrowser->append("Timestamp: " + QString::number(_Timestamp));

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

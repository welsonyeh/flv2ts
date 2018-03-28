#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void open(QString&);
    void parseFlvHeader(QDataStream&);
    void parseVideoData(int);
    void parseAudioData(int);
    void parseAVCDecoderConfigurationRecord();
    void generatePSI();
    void xEs2TsPacketizeEx(unsigned int);

private:
    Ui::MainWindow *ui;
    unsigned char* bsbuf;
    unsigned char* ptr;
    unsigned char* outbuf_head;
    unsigned char* outbuf_cur;
    unsigned char  CCa;
    unsigned char  CCv;
    unsigned char* pCC;
    unsigned char pbSPS[256];
    unsigned char pbPPS[256];
    unsigned char* pExtHdr;
    unsigned int dExtHdrSz;
    unsigned char* pData;
    unsigned long long dPTS;
    unsigned long long old_PTS;
    unsigned char bAVStreamType;
    QFile outFile;
};

#endif // MAINWINDOW_H

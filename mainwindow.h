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
    void parseFlvSHeader(QDataStream&);
    void parseVideoData(int);
    void parseAVCDecoderConfigurationRecord();
    void generatePSI();

private:
    Ui::MainWindow *ui;
    unsigned char* bsbuf;
    unsigned char* ptr;
    unsigned int wptr;
    unsigned int rptr;
    QDataStream out;
    QFile outFile;
    const unsigned char pes_header[9]={0x0,0x0,0x01,0xE0,0x0,0x0,0x8B,0x80,0x05};
    unsigned int _Timestamp;
};

#endif // MAINWINDOW_H

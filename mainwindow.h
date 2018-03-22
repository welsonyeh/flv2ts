#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    unsigned char* bsbuf;
    unsigned int wptr;
    unsigned int rptr;
};

#endif // MAINWINDOW_H

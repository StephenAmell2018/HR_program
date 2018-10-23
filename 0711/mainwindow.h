#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPaintEvent>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QImage>
#include <QDebug>
#include <opencv2/opencv.hpp>

#include <QMainWindow>
using namespace cv;
void mouseWrapper( int event, int x, int y, int flags, void* param );

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void onMouse(int EVENT, int x, int y, int flags, void* userdata);//注意要放到public中；

private slots:
    void on_pushButton_clicked();
    void getFrame();




private:
    Ui::MainWindow *ui;
    QImage *imag;
    CvCapture *capture;//highgui里提供的一个专门处理摄像头图像的结构体
    IplImage *frame;//摄像头每次抓取的图像为一帧，使用该指针指向一帧图像的内存空间
    QTimer *timer;//定时器用于定时取帧，上面说的隔一段时间就去取就是用这个实现




};

#endif // MAINWINDOW_H

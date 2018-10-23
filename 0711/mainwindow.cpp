#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"MatToQImage.h"
#include <opencv2/opencv.hpp>



#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
char string[10];
double t=0;
double fps=0;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    capture = NULL;
    timer = new QTimer(this);
    imag = new QImage();

    connect(ui->pushButton,SIGNAL(click()),this,SLOT(on_pushButton_clicked()));//打开摄像头按钮
   // connect(timer, SIGNAL(timeout()), this, SLOT(getFrame()));//超时就读取当前摄像头信息,通过此可以控制采样帧率
}

void MainWindow::onMouse(int event, int x, int y, int flags, void *param)
{
    bool draw;
    Mat src;//原始图像
    Mat roi;//ROI图像
    Point cursor;//初始坐标
    Rect rect;//标记ROI的矩形框
    Mat img = src.clone();
    switch (event)
    {
        //按下鼠标左键
    case CV_EVENT_LBUTTONDOWN:
        //点击鼠标图像时，清除之前ROI图像的显示窗口
        cvDestroyWindow("ROI");
        //存放起始坐标
        cursor = Point(x, y);
        //初始化起始矩形框
        rect = Rect(x, y, 0, 0);
        draw = true;
        break;

        //松开鼠标左键
    case CV_EVENT_LBUTTONUP:
        if (rect.height > 0 && rect.width > 0)
        {
            //将img中的矩形区域复制给roi，并显示在SignROI窗口
            roi = img(Rect(rect.x, rect.y, rect.width, rect.height));

            rectangle(img, rect, Scalar(0, 0, 255), 2);
            QImage image2 = MatToQImage(img);
            ui->label->setScaledContents(true);//很重要，通过这个设置可以使label自适应显示图片
            ui->label->setPixmap(QPixmap::fromImage(image2));//将视频显示到label上


            //将画过矩形框的图像用原图像还原
            src.copyTo(img);
            QImage image3 = MatToQImage(img);
            ui->label->setScaledContents(true);//很重要，通过这个设置可以使label自适应显示图片
            ui->label_2->setPixmap(QPixmap::fromImage(image3));//将视频显示到label上

            //显示ROI图像
            namedWindow("ROI");
            imshow("ROI", roi);
            waitKey(0);
        }
        draw = false;
        break;

        //移动光标
    case CV_EVENT_MOUSEMOVE:
        if (draw)
        {
            //用MIN得到左上点作为矩形框的起始坐标，如果不加这个，画矩形时只能向一个方向进行
            rect.x = MIN(x, cursor.x);
            rect.y = MIN(y, cursor.y);
            rect.width = abs(cursor.x - x);
            rect.height = abs(cursor.y - y);
            //防止矩形区域超出图像的范围
            rect &= Rect(0, 0, src.cols, src.rows);
        }
        break;
    }
}

void mouseWrapper( int event, int x, int y, int flags, void* param )
{
    MainWindow * mainWin = (MainWindow *)(param);
    mainWin->onMouse(event,x,y,flags,param);
}

/******************************
 ***********打开摄像头***********
 *****************************/
void MainWindow::on_pushButton_clicked()
{

    VideoCapture  cap;
    cap.open(0);
    if(!cap.isOpened())
    {
        qDebug()<<"error!";
    }
    //timer->start(50);//开始计时，超时则发出timeout()信号,1000为1秒，50毫秒取一帧，20帧/秒
    while (1)
    {
   //获取系统时间
    double t = (double)cv::getTickCount();
    if (cap.isOpened()){
    Mat frame;
    cap >> frame;//debug frame的格式是720*1080
    Rect rect(480,240,360,240);
    Mat image_roi = frame(rect);
    //再次获取系统时间
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    fps = 1.0 / t;
    sprintf(string, "%.2f", fps);  // 帧率保留两位小数
    std::string fpsString("FPS:");
    fpsString += string;                    // 在"FPS:"后加入帧率数值字符串
    printf("fps: %.2f,width:%d,height:%d,fps:%.2f\n", fps, frame.cols, frame.rows, cap.get(CV_CAP_PROP_FPS));
               // 将帧率信息写在输出帧上
    cv::putText(image_roi, // 图像矩阵
                   fpsString,        // string型文字内容
                   cv::Point(50,200),           // 文字坐标，以左下角为原点
                   cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
                   0.5, // 字体大小
                   cv::Scalar(0, 0, 0));       // 字体颜色
    cv::putText(frame, // 图像矩阵
                   fpsString,        // string型文字内容
                   cv::Point(50,200),           // 文字坐标，以左下角为原点
                   cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
                   0.5, // 字体大小
                   cv::Scalar(0, 0, 0));       // 字体颜色
     //使用了某大神的调用函数以求在控件lable上显示图像
    //重点！可以把图片显示在里面了
   QImage image = MatToQImage(image_roi);
   ui->label->setScaledContents(true);//很重要，通过这个设置可以使label自适应显示图片
   ui->label->setPixmap(QPixmap::fromImage(image));//将视频显示到label上
   //同理 可以显示预览的图片在标签2 以后调换位置就好了 接下来考虑如何裁剪，然后就是相关函数了
   QImage image1 = MatToQImage(frame);
   ui->label_2->setScaledContents(true);//很重要，通过这个设置可以使label自适应显示图片
   ui->label_2->setPixmap(QPixmap::fromImage(image1));//将视频显示到label


    cv::imshow("img", frame);
    setMouseCallback("img", mouseWrapper, 0);
    //setMouseCallback("img", B_on_Mouse, 0);
    char c = cv::waitKey(30); //延时30毫秒
                // 注：waitKey延时越长 fps越大 出现跳帧 摄像头显示变卡
                if (c == 27) //按ESC键退出
                    break;
    }
                else
                    {
                               std::cout << "No Camera Input!" << std::endl;
                               break;
                }
    }
}



void MainWindow::getFrame()
{
    frame = cvQueryFrame(capture);//从摄像头中抓取并返回每一帧
    // 将抓取到的帧，从IplImage格式转换为QImage格式,rgbSwapped是为了显示效果色彩好一些
    //QImage::Format_RGB888不同的摄像头用不同的格式。
   QImage image = QImage((const uchar*)frame->imageData,
                         frame->width, frame->height,
                         QImage::Format_RGB888).rgbSwapped();

    ui->label->setScaledContents(true);//很重要，通过这个设置可以使label自适应显示图片
    ui->label->setPixmap(QPixmap::fromImage(image));//将视频显示到label上
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "widget.h"
#include "ui_widget.h"
#include "main.h"
#include <QWidget>
#include <QApplication>
#include "CSpiDev.h"
#include <fcntl.h>
#include <QLabel>
#include <FFT.h>

#include <QDebug>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>


quint8 a[800];
quint8 b[800];
int timeupdate=0;

int spi_draw[400];

char spi_ready[1];
int spi_data_ready;
char str_ready[30];

char force_trig[1];
int force_data_trig;
char str_ftrig[30];

char change_flag[1];
int change_data_flag;
char str_cflag[30];

char time_4[1];
int time_data_4;
char time_3[1];
int time_data_3;
char time_2[1];
int time_data_2;
char time_1[1];
int time_data_1;
char time_0[1];
int time_data_0;
int time_data = 5;
char str_time[30];

char measure0[1];
int measure_data_0;
char measure1[1];
int measure_data_1;
int measure_data;
char str_meas[30];

char ampl0[1];
int ampl_data_0;
char ampl1[1];
int ampl_data_1;
char ampl2[1];
int ampl_data_2;
char ampl3[1];
int ampl_data_3;
int ampl_data;
char str_ampl[30];

int time_flag;
int k_ampl;
int k_widget;
int k_time;

float k_expand_case0=1*1.3;
float k_expand_case1=0.5*1.3;
float k_expand_case2=2*1.3;
float k_expand_case3=4*1.3;

double SampleRate;
int k_break;

int maxdata=0;
int mindata=1023;
int point[400];
int s;
double d;
double amplt;

double point_frequency;
double average_circle;
double circle1;
int decimalValue;

//光标区*********************************光标区*//
//X轴坐标
// double X_Range = 0.0;
// double Y_Range = 0.0;
//Y轴坐标



int xData[400];
int yData[1024];


//X光标
// QString X_styleSheet;
static bool X1_flag = false;
static bool X2_flag = false;

// QString Y_styleSheet;
static bool Y1_flag = false;
static bool Y2_flag = false;
//Y光标

static bool cursors_flag = false;
static bool fft_flag = false;
static bool rst_flag = false;
static bool pause_flag = false;

//光标区*********************************光标区*//


#define GPIO_SYSFS_PATH "/sys/class/gpio"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    InitGPIO();


    ui->setupUi(this);


    // QCustomPlot *pCustomPlot = new QCustomPlot(this);
    // pCustomPlot->resize(300, 300);

    // 创建定时器，每秒触发一次 updateData 槽函数
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::updateData);
    timer->start(16); // 定时器间隔为16ms

    for(int j=0; j<=799;j++){
        a[j]=0;
    }
    openSpi();
    // 初始化数据索引
    dataStartIndex[0] = 120;
    dataEndIndex[0] = 279;

    dataStartIndex[1] = 120;
    dataEndIndex[1] = 279;

    dataStartIndex[2] = 120;
    dataEndIndex[2] = 279;

    dataStartIndex[3] = 140;
    dataEndIndex[3] = 299;

    dataStartIndex[4] = 180;
    dataEndIndex[4] = 339;

    dataStartIndex[5] = 220;
    dataEndIndex[5] = 379;

    dataStartIndex[6] = 180;
    dataEndIndex[6] = 339;

    dataStartIndex[7] = 220;
    dataEndIndex[7] = 379;


    // 初始化 QCustomPlot
    ui->widget->addGraph();
    origionWindows();


    ui->stackedWidget->setCurrentWidget(ui->page);
    ui->stackedWidget->setVisible(false);
    ui->stackedWidget->setWindowFlags(Qt::FramelessWindowHint);//设置无窗口框架边界
    ui->stackedWidget->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    //光标
    Ycursor();
    Xcursor();

    Midcursor();

    // 设置窗口大小，使波形完整填充窗口
    this->resize(1220, 650); // 设置窗口大小为 800x600，根据需要调整大小


    for(int i=0;i<250;i++){
        b[i]=19;
    }
    for(int i=250;i<280;i++){
        b[i]=19;
    }
    for(int i=280;i<300;i++){
        b[i]=19;
    }
    for(int i=300;i<320;i++){
        b[i]=19;
    }
    for(int i=320;i<400;i++){
        b[i]=11;
    }
    for(int i=400;i<480;i++){
        b[i]=19;
    }
    for(int i=480;i<800;i++){
        b[i]=11;
    }



    RightLayoutInit();

}

void Widget::Midcursor(){
    QPen ReferLinePen(Qt::white);
    ReferLinePen.setWidth(4);
    ReferLinePen.setStyle(Qt::PenStyle::SolidLine);

    //中心参考线
    m_refer_lineY0 = new QCPItemStraightLine(ui->widget);
    m_refer_lineY0->point1->setCoords(X0, 100);               //设置点1的值
    m_refer_lineY0->point2->setCoords(X0, 300);              //设置点2的值
    m_refer_lineY0->setPen(ReferLinePen);

    m_refer_lineX0 = new QCPItemStraightLine(ui->widget);
    m_refer_lineX0->point1->setCoords(100, 512);               //设置点1的值
    m_refer_lineX0->point2->setCoords(300, 512);              //设置点2的值
    m_refer_lineX0->setPen(ReferLinePen);

    QColor clrDefault(Qt::white);
    QPen penDefault(clrDefault);
    m_arrowY0 = new QCPItemLine(ui->widget);
    m_arrowY0->setPen(penDefault);
    m_arrowY0->setLayer("overlay");
    m_arrowY0->setClipToAxisRect(false);
    m_arrowY0->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowY0->start->setCoords(X0, 995);
    m_arrowY0->end->setCoords(X0, 1023);

    m_arrowX0 = new QCPItemLine(ui->widget);
    m_arrowX0->setPen(penDefault);
    m_arrowX0->setLayer("overlay");
    m_arrowX0->setClipToAxisRect(false);
    m_arrowX0->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowX0->start->setCoords(dataEndIndex[time_flag]-20, 512);
    m_arrowX0->end->setCoords(dataEndIndex[time_flag], 512);
}

void Widget::Ycursor()
{
    QPen ReferLinePen(Qt::red);
    ReferLinePen.setWidth(2);
    ReferLinePen.setStyle(Qt::PenStyle::DotLine);

    //水平参考线Y1
    m_refer_lineY1 = new QCPItemStraightLine(ui->widget);
    m_refer_lineY1->point1->setCoords(100, Y1);               //设置点1的值
    m_refer_lineY1->point2->setCoords(300, Y1);              //设置点2的值
    m_refer_lineY1->setPen(ReferLinePen);

    //水平参考线Y2
    m_refer_lineY2 = new QCPItemStraightLine(ui->widget);
    m_refer_lineY2->point1->setCoords(100, Y2);               //设置点1的值
    m_refer_lineY2->point2->setCoords(300, Y2);              //设置点2的值
    m_refer_lineY2->setPen(ReferLinePen);

    //Y1箭头Y2箭头
    QColor clrDefault(Qt::red);
    QPen penDefault(clrDefault);
    m_arrowY1 = new QCPItemLine(ui->widget);
    m_arrowY1->setPen(penDefault);
    m_arrowY1->setLayer("overlay");
    m_arrowY1->setClipToAxisRect(false);
    m_arrowY1->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowY1->start->setCoords(120, Y1);
    m_arrowY1->end->setCoords(122, Y1);


    m_arrowY2 = new QCPItemLine(ui->widget);
    m_arrowY2->setPen(penDefault);
    m_arrowY2->setLayer("overlay");
    m_arrowY2->setClipToAxisRect(false);
    m_arrowY2->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowY2->start->setCoords(120, Y2);
    m_arrowY2->end->setCoords(122, Y2);

    //初始化后隐藏
    m_refer_lineY1->setVisible(false);
    m_refer_lineY2->setVisible(false);

    //箭头
    m_arrowY1->setVisible(false);
    m_arrowY2->setVisible(false);

}

void Widget::Xcursor()
{
    QPen ReferLinePen(Qt::red);
    ReferLinePen.setWidth(2);
    ReferLinePen.setStyle(Qt::PenStyle::DotLine);
    //垂直参考线X1
    m_refer_lineV = new QCPItemStraightLine(ui->widget);
    m_refer_lineV->point1->setCoords(X1, 800);               //设置点1的值
    m_refer_lineV->point2->setCoords(X1, 700);              //设置点2的值
    m_refer_lineV->setPen(ReferLinePen);

    //垂直参考线X2
    m_refer_lineV2 = new QCPItemStraightLine(ui->widget);
    m_refer_lineV2->point1->setCoords(X2, 800);               //设置点1的值
    m_refer_lineV2->point2->setCoords(X2, 700);              //设置点2的值
    m_refer_lineV2->setPen(ReferLinePen);

    //X1箭头X2箭头
    QColor clrDefault(Qt::red);
    QPen penDefault(clrDefault);
    m_arrow = new QCPItemLine(ui->widget);
    m_arrow->setPen(penDefault);
    m_arrow->setLayer("overlay");
    m_arrow->setClipToAxisRect(false);
    m_arrow->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrow->start->setCoords(X1, 1023);
    m_arrow->end->setCoords(X1, 1018);

    m_arrowX2 = new QCPItemLine(ui->widget);
    m_arrowX2->setPen(penDefault);
    m_arrowX2->setLayer("overlay");
    m_arrowX2->setClipToAxisRect(false);
    m_arrowX2->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowX2->start->setCoords(X2, 1023);
    m_arrowX2->end->setCoords(X2, 1018);

    //初始化后隐藏
    //参考线
    m_refer_lineV2->setVisible(false);
    m_refer_lineV->setVisible(false);

    //箭头
    m_arrow->setVisible(false);
    m_arrowX2->setVisible(false);

}

Widget::~Widget()
{
    delete ui;
}


void Widget::updateData()
{
    dataArray.clear();
    //spiDataReady
        fdd = open(GPIO_SYSFS_PATH "/gpio53/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, spi_ready, 1);
         ::close(fdd);

        spi_data_ready=int(spi_ready[0]);
        spi_data_ready=spi_data_ready-48;
        sprintf(str_ready,"Ready=%d",spi_data_ready);
        ui->label_spird->setText(str_ready);



     //force_trig
        fdd = open(GPIO_SYSFS_PATH "/gpio52/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, force_trig, 1);
        ::close(fdd);

        force_data_trig=int(force_trig[0]);
        force_data_trig=force_data_trig-48;
        sprintf(str_ftrig,"ForceTrig=%d",force_data_trig);
        ui->label_ftrig->setText(str_ftrig);

    //change_flag
        fdd = open(GPIO_SYSFS_PATH "/gpio54/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, change_flag, 1);
        ::close(fdd);

        // change_data_flag=int(change_flag[0]);
        // change_data_flag =change_data_flag-48;
        // sprintf(str_cflag,"ChangeFlag=%d",change_data_flag);
        // ui->label_cflag->setText(str_cflag);



    //measure_state0&1
    //00run,    01single,   10pause
        fdd = open(GPIO_SYSFS_PATH "/gpio47/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, measure0, 1);
        ::close(fdd);

        measure_data_0=int(measure0[0]);
        measure_data_0 =measure_data_0-48;

        fdd = open(GPIO_SYSFS_PATH "/gpio46/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, measure1, 1);
        ::close(fdd);

        measure_data_1=int(measure1[0]);
        measure_data_1 =measure_data_1-48;
        measure_data = (measure_data_1 << 1 | measure_data_0);


     //amplitudeState0&1&2&3
        fdd = open(GPIO_SYSFS_PATH "/gpio45/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, ampl0, 1);
        ::close(fdd);

        fdd = open(GPIO_SYSFS_PATH "/gpio44/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, ampl1, 1);
        ::close(fdd);

        fdd = open(GPIO_SYSFS_PATH "/gpio51/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, ampl2, 1);
        ::close(fdd);

        // fdd = open(GPIO_SYSFS_PATH "/gpio50/value", O_RDONLY);
        // if (fdd < 0) {
        //     //printf("open direction failed\n");
        // }
        // ret = read(fdd, ampl3, 1);
        // ::close(fdd);
        //ampl3[0]=0;
        ampl_data_0=int(ampl0[0]);
        ampl_data_1=int(ampl1[0]);
        ampl_data_2=int(ampl2[0]);
        ampl_data_3=int(ampl3[0]);
        ampl_data_0 = ampl_data_0-48;
        ampl_data_1 = ampl_data_1-48;
        ampl_data_2 = ampl_data_2-48;
        ampl_data_3 = 0;//ampl_data_3-48;
        ampl_data = (ampl_data_3<<3|ampl_data_2<<2|ampl_data_1<<1|ampl_data_0<<0);



    //TimeState01234
        fdd = open(GPIO_SYSFS_PATH "/gpio59/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, time_4, 1);
        ::close(fdd);

        fdd = open(GPIO_SYSFS_PATH "/gpio58/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, time_3, 1);
        ::close(fdd);

        fdd = open(GPIO_SYSFS_PATH "/gpio57/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, time_2, 1);
        ::close(fdd);

        fdd = open(GPIO_SYSFS_PATH "/gpio56/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, time_1, 1);
        ::close(fdd);

        fdd = open(GPIO_SYSFS_PATH "/gpio55/value", O_RDONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        ret = read(fdd, time_0, 1);
        ::close(fdd);

        time_data_4=int(time_4[0]);
        time_data_3=int(time_3[0]);
        time_data_2=int(time_2[0]);
        time_data_1=int(time_1[0]);
        time_data_0=int(time_0[0]);
        time_data_4 = time_data_4-48;
        time_data_3 = time_data_3-48;
        time_data_2 = time_data_2-48;
        time_data_1 = time_data_1-48;
        time_data_0 = time_data_0-48;
        time_data = (time_data_4<<4|time_data_3<<3|time_data_2<<2|time_data_1<<1|time_data_0<<0);



        //int time_data = 3;

        switch(time_data)
        {
        case 20:
            ui->label_time->setText("100 ns/div \n200 MSa/s \n 等效采样");
            time_flag = 7;
            SampleRate = 200000000;
            k_time=100;
            break;
        case 19:
            ui->label_time->setText("200 ns/div \n100 MSa/s \n 等效采样");
            time_flag = 6;
            SampleRate = 100000000;
            k_time=200;
            break;
        case 18:
            ui->label_time->setText("500 ns/div \n40 MSa/s \n 等效采样");
            time_flag = 5;
            SampleRate = 40000000;
            k_time=500;
            break;
        case 17:
            ui->label_time->setText("1 us/div \n20 MSa/s \n 等效采样");
            time_flag = 4;
            SampleRate = 20000000;
            k_time=1000;
            break;
        case 16:
            ui->label_time->setText("2 us/div \n10 MSa/s \n 等效采样");
            time_flag = 3;
            SampleRate = 10000000;
            k_time=2000;
            break;
        case 15:
            ui->label_time->setText("5 us/div \n4 MSa/s \n 等效采样");
            time_flag = 2;
            SampleRate = 4000000;
            k_time=5000;
            break;
        case 14:
            ui->label_time->setText("10 us/div \n2 MSa/s \n 等效采样");
            time_flag = 1;
            SampleRate = 2000000;
            k_time=10000;
            break;
        case 13:
            ui->label_time->setText("20 us/div \n1 MSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 1000000;
            k_time=20000;
            break;
        case 12:
            ui->label_time->setText("50 us/div \n400 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 400000;
            k_time=50000;
            break;
        case 11:
            ui->label_time->setText("100 us/div \n200 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 200000;
            k_time=100000;
            break;
        case 10:
            ui->label_time->setText("200 us/div \n100 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 100000;
            k_time=200000;
            break;
        case 9:
            ui->label_time->setText("500 us/div \n40 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 40000;
            k_time=500000;
            break;
        case 8:
            ui->label_time->setText("1 ms/div \n20 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 20000;
            k_time=1000000;
            break;
        case 7:
            ui->label_time->setText("2 ms/div \n10 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 10000;
            k_time=2000000;
            break;
        case 6:
            ui->label_time->setText("5 ms/div \n4 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 4000;
            k_time=5000000;
            break;
        case 5:
            ui->label_time->setText("10 ms/div \n2 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 2000;
            k_time=10000000;
            break;
        case 4:
            ui->label_time->setText("20 ms/div \n1 KSa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 1000;
            k_time=20000000;
            break;
        case 3:
            ui->label_time->setText("50 ms/div \n400 Sa/s \n 实时采样");
            time_flag = 0;
            SampleRate = 400;
            k_time=50000000;
            break;
        default:
            ui->label_time->setText("-- s/div \n-- Sa/s \n --采样");
            time_flag = 0;
            SampleRate = 5000;
            k_time=0;
    }


    if(ampl_data==0){
        fdd = open(GPIO_SYSFS_PATH "/gpio60/value", O_WRONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        //printf("###\n");
        ret = write(fdd, "1", 1);
        ::close(fdd);
    }else{
        fdd = open(GPIO_SYSFS_PATH "/gpio60/value", O_WRONLY);
        if (fdd < 0) {
            //printf("open direction failed\n");
        }
        //printf("???\n");
        ret = write(fdd, "0", 1);
        ::close(fdd);
    }


    //int ampl_data = 2;
    switch(ampl_data)
    {
        case 0:
            ui->label_ampl->setText("2 mV/div");
            k_ampl=2;
            k_widget=0;
            break;
        case 1:
            ui->label_ampl->setText("2 V/div");
            k_ampl=2000;
            k_widget=1;
            break;
        case 2:
            ui->label_ampl->setText("1 V/div");
            k_ampl=1000;
            k_widget=0;
            break;
        case 3:
            ui->label_ampl->setText("500 mV/div");
            k_ampl=500;
            k_widget=2;
            break;
        case 4:
            ui->label_ampl->setText("200 mV/div");
            k_ampl=200;
            k_widget=1;
            break;
        case 5:
            ui->label_ampl->setText("100 mV/div");
            k_ampl=100;
            k_widget=0;
            break;
        case 6:
            ui->label_ampl->setText("50 mV/div");
            k_ampl=50;
            k_widget=2;
            break;
        case 7:
            ui->label_ampl->setText("20 mV/div");
            k_ampl=20;
            k_widget=3;
            break;
        case 8:
            ui->label_ampl->setText("2 mV/div");
            k_ampl=2;
            k_widget=0;
            break;
        default:
            ui->label_ampl->setText("- V/div");
            k_widget=0;
            k_ampl=0;
    }


    //int measure_data = 0;
    switch(measure_data)
    {
        case 0:
            ui->label_meas->setText("RUN");
            break;
        case 1:
            ui->label_meas->setText("SINGLE");
            break;
        case 2:
            ui->label_meas->setText("PAUSE");
            break;
        default:
            ui->label_meas->setText("ERROR");
    }

    // printf("%d",spi_data_ready);

/*------------------------------------------------------------------------------*/
    //  spi_data_ready=1; //test point
        if(spi_data_ready==0 || spi_data_ready==-48)
       {
        return;
        }
/*------------------------------------------------------------------------------*/

    transferSpi(a,b,800);
    s=0;


    // for(int i=0;i<=799;i++){
    //      printf("%04d:%03d ",i,b[i]);
    //      //printf("%03d ",b[i]);
    //  }



    //time_flag = 1; //test point
    switch (time_flag) {
    case 0:
        for(int i=0;i<=399;i++){
            int c = ((b[2*i] & 0x1F) << 5) | (b[2*i+1] & 0x1F);
            switch (k_widget) {
            case 0:
                c=c*k_expand_case0+512*(1-k_expand_case0);
                break;
            case 1:
                c=c*k_expand_case1+512*(1-k_expand_case1);
                break;
            case 2:
                c=c*k_expand_case2+512*(1-k_expand_case2);
                break;
            case 3:
                c=c*k_expand_case3+512*(1-k_expand_case3);
                break;
            default:
                break;
            }
            if(c>1023){
                c=1023;
            }
            if(s<=399){
                point[s]=c;
                s++;}
        }
        break;

    case 1:
        for(int i=0;i<=398;i=i+2){
            for(int j=0;j<=1;j++){
                int c = ((b[400*j+i] & 0x1F) << 5) | (b[400*j+i+1] & 0x1F);
                switch (k_widget) {
                case 0:
                    c=c*k_expand_case0+512*(1-k_expand_case0);
                    break;
                case 1:
                    c=c*k_expand_case1+512*(1-k_expand_case1);
                    break;
                case 2:
                    c=c*k_expand_case2+512*(1-k_expand_case2);
                    break;
                case 3:
                    c=c*k_expand_case3+512*(1-k_expand_case3);
                    break;
                default:
                    break;
                }
                if(c>1023){
                    c=1023;
                }
                if(s<=399){
                    point[s]=c;
                    s++;}

            }
        }
        break;
    case 2:
        for(int i=0;i<=198;i=i+2){
            for(int j=0;j<=3;j++){
                int c = ((b[200*j+i] & 0x1F) << 5) | (b[200*j+i+1] & 0x1F);
                switch (k_widget) {
                case 0:
                    c=c*k_expand_case0+512*(1-k_expand_case0);
                    break;
                case 1:
                    c=c*k_expand_case1+512*(1-k_expand_case1);
                    break;
                case 2:
                    c=c*k_expand_case2+512*(1-k_expand_case2);
                    break;
                case 3:
                    c=c*k_expand_case3+512*(1-k_expand_case3);
                    break;
                default:
                    break;
                }
                if(c>1023){
                    c=1023;
                }
                if(s<=399){
                    point[s]=c;
                    s++;}
            }
        }
        break;
    case 3:
        for(int i=0;i<=78;i=i+2){
            for(int j=0;j<=9;j++){
                int c = ((b[80*j+i] & 0x1F) << 5) | (b[80*j+i+1] & 0x1F);
                switch (k_widget) {
                case 0:
                    c=c*k_expand_case0+512*(1-k_expand_case0);
                    break;
                case 1:
                    c=c*k_expand_case1+512*(1-k_expand_case1);
                    break;
                case 2:
                    c=c*k_expand_case2+512*(1-k_expand_case2);
                    break;
                case 3:
                    c=c*k_expand_case3+512*(1-k_expand_case3);
                    break;
                default:
                    break;
                }
                if(c>1023){
                    c=1023;
                }
                if(s<=399){
                    point[s]=c;
                    s++;}
            }
        }
        break;

    case 4:
        for(int i=0;i<=38;i=i+2){
            for(int j=0;j<=19;j++){
                int c = ((b[40*j+i] & 0x1F) << 5) | (b[40*j+i+1] & 0x1F);
                switch (k_widget) {
                case 0:
                    c=c*k_expand_case0+512*(1-k_expand_case0);
                    break;
                case 1:
                    c=c*k_expand_case1+512*(1-k_expand_case1);
                    break;
                case 2:
                    c=c*k_expand_case2+512*(1-k_expand_case2);
                    break;
                case 3:
                    c=c*k_expand_case3+512*(1-k_expand_case3);
                    break;
                default:
                    break;
                }
                if(c>1023){
                    c=1023;
                }
                if(s<=399){
                    point[s]=c;
                    s++;}

            }
        }
        break;

    case 5:
        for(int i=0;i<=19;i=i+2){
            for(int j=0;j<=39;j++){
                int c = ((b[20*j+i] & 0x1F) << 5) | (b[20*j+i+1] & 0x1F);
                switch (k_widget) {
                case 0:
                    c=c*k_expand_case0+512*(1-k_expand_case0);
                    break;
                case 1:
                    c=c*k_expand_case1+512*(1-k_expand_case1);
                    break;
                case 2:
                    c=c*k_expand_case2+512*(1-k_expand_case2);
                    break;
                case 3:
                    c=c*k_expand_case3+512*(1-k_expand_case3);
                    break;
                default:
                    break;
                }
                if(c>1023){
                    c=1023;
                }
                if(s<=399){
                    point[s]=c;
                    s++;}

            }
        }
        break;

    case 6:
        for(int i=0;i<=6;i=i+2){
            for(int j=0;j<=99;j++){
                int c = ((b[8*j+i] & 0x1F) << 5) | (b[8*j+i+1] & 0x1F);
                switch (k_widget) {
                case 0:
                    c=c*k_expand_case0+512*(1-k_expand_case0);
                    break;
                case 1:
                    c=c*k_expand_case1+512*(1-k_expand_case1);
                    break;
                case 2:
                    c=c*k_expand_case2+512*(1-k_expand_case2);
                    break;
                case 3:
                    c=c*k_expand_case3+512*(1-k_expand_case3);
                    break;
                default:
                    break;
                }
                if(c>1023){
                    c=1023;
                }
                if(s<=399){
                    point[s]=c;
                    s++;}
            }
        }
        break;

    case 7:
        for(int i=0;i<=2;i=i+2){
            for(int j=0;j<=199;j++){
                int c = ((b[4*j+i] & 0x1F) << 5) | (b[4*j+i+1] & 0x1F);
                switch (k_widget) {
                case 0:
                    c=c*k_expand_case0+512*(1-k_expand_case0);
                    break;
                case 1:
                    c=c*k_expand_case1+512*(1-k_expand_case1);
                    break;
                case 2:
                    c=c*k_expand_case2+512*(1-k_expand_case2);
                    break;
                case 3:
                    c=c*k_expand_case3+512*(1-k_expand_case3);
                    break;
                default:
                    break;
                }
                if(c>1023){
                    c=1023;
                }
                if(s<=399){
                point[s]=c;
                    s++;}
            }
        }
        break;

    default:
        return;
    }


    if(time_data==19){
        point[200]=(point[199]+point[201])/2;
        point[300]=(point[299]+point[301])/2;
    }

    circle1 = frequency_value(512);
        // circle = f0 * k_time/20   ns;



    average_circle = /*((circle1+circle2+circle3)/3)*/circle1/(1000*1000*1000);

    //printf("%lf\n",average_circle);

    point_frequency = 1/average_circle;


    if(point_frequency>2.5){
    switch(time_data)
    {
    case 20:
        if(point_frequency>21000000){
            return;
        }
        break;
    case 19:
        if(point_frequency>10500000){
            return;
        }
        break;
    case 18:
        if(point_frequency>4200000){
            return;
        }
        break;
    case 17:
        if(point_frequency>2100000){
            return;
        }
        break;
    case 16:
        if(point_frequency>1050000){
            return;
        }
        break;
    case 15:
        if(point_frequency>420000){
            return;
        }
        break;
    case 14:
        if(point_frequency>210000){
            return;
        }
        break;
    case 13:
        if(point_frequency>105000){
            return;
        }
        break;
    case 12:
        if(point_frequency>42000){
            return;
        }
        break;
    case 11:
        if(point_frequency>21000){
            return;
        }
        break;
    case 10:
        if(point_frequency>10500){
            return;
        }
        break;
    case 9:
        if(point_frequency>4200){
            return;
        }
        break;
    case 8:
        if(point_frequency>2100){
            return;
        }
        break;
    case 7:
        if(point_frequency>1050){
            return;
        }
        break;
    case 6:
        if(point_frequency>420){
            return;
        }
        break;
    case 5:
        if(point_frequency>210){
            return;
        }
        break;
    case 4:
        if(point_frequency>105){
            return;
        }
        break;
    case 3:
        if(point_frequency>42){
            return;
        }
        break;
    default:
        if(point_frequency>21000000){
            return;
        }
    }
    }

    if(point_frequency<20000000){
        //amplt=(point_frequency*point_frequency)*8/10000000000000000+(3.4783*point_frequency)/1000000000+1.014969;
    }else{
        //amplt=1.014969;
    }
    amplt=1;
    //printf("%lf \n",amplt);

    maxdata=0;
    mindata=1023;
    for(int i=0;i<=399;i++){
        point[i]=1024-(point[i]*amplt+512*(1-amplt));
        //printf("%d:%d ",i,point[i]);
    }


    for(int i=0;i<=399;i++){
        QString binaryString;
        d=point[i];
        binaryString += QString::number(uint(d));
        decimalValue =int(d);
        //printf("%d ",decimalValue);
        if(i>=dataStartIndex[time_flag] && i<=dataEndIndex[time_flag]){
        if(maxdata < decimalValue){
            maxdata = decimalValue;
        }
        if(mindata > decimalValue){
            mindata = decimalValue;
        }
        }
        dataArray.append(QString::number(decimalValue));
    }

    if(average_circle>0.4)
        ui->label_circle->setText("Circle= -- ns");
    else if(average_circle*1000*1000*1000<1000)
        ui->label_circle->setText("Circle= "+QString::number((average_circle)*1000*1000*1000)+" ns");
    else if(average_circle*1000*1000*1000>=1000 && average_circle*1000*1000*1000<1000000)
        ui->label_circle->setText("Circle= "+QString::number((average_circle)*1000*1000)+" us");
    else if(average_circle*1000*1000*1000>=1000000 && average_circle*1000*1000*1000<1000000000)
        ui->label_circle->setText("Circle= "+QString::number((average_circle*1000))+"ms");

    if(point_frequency<2.5)
        ui->label_frequ->setText("Freq= -- Hz");
    else if(point_frequency<1000)
        ui->label_frequ->setText("Freq= "+QString::number(point_frequency)+" Hz");
    else if(point_frequency>=1000 && point_frequency<1000000)
        ui->label_frequ->setText("Freq= "+QString::number(point_frequency/1000)+" KHz");
    else if(point_frequency>=1000000 && point_frequency<1000000000)
        ui->label_frequ->setText("Freq= "+QString::number(point_frequency/1000/1000)+" MHz");

    if(!fft_flag)
    {
    origionWindows();

    X0 = (dataStartIndex[time_flag]+dataEndIndex[time_flag]+1)/2;
    QPen ReferLinePen(Qt::white);
    ReferLinePen.setWidth(4);
    ReferLinePen.setStyle(Qt::PenStyle::SolidLine);

    //中心参考线
    // m_refer_lineY0 = new QCPItemStraightLine(ui->widget);
    m_refer_lineY0->point1->setCoords(X0, 100);               //设置点1的值
    m_refer_lineY0->point2->setCoords(X0, 300);              //设置点2的值
    m_refer_lineY0->setPen(ReferLinePen);

    m_refer_lineX0->point1->setCoords(200, 512);               //设置点1的值
    m_refer_lineX0->point2->setCoords(220, 512);              //设置点2的值
    m_refer_lineX0->setPen(ReferLinePen);

    QColor clrDefault(Qt::white);
    QPen penDefault(clrDefault);
    // m_arrowY0 = new QCPItemLine(ui->widget);
    m_arrowY0->setPen(penDefault);
    m_arrowY0->setLayer("overlay");
    m_arrowY0->setClipToAxisRect(false);
    m_arrowY0->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowY0->start->setCoords(X0, 995);
    m_arrowY0->end->setCoords(X0, 1023);

    // m_arrowX0 = new QCPItemLine(ui->widget);
    m_arrowX0->setPen(penDefault);
    m_arrowX0->setLayer("overlay");
    m_arrowX0->setClipToAxisRect(false);
    m_arrowX0->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowX0->start->setCoords(dataEndIndex[time_flag]-20, 512);
    m_arrowX0->end->setCoords(dataEndIndex[time_flag], 512);

    QVector<double> displayData; //y
    for(int i=dataStartIndex[time_flag];i<=dataEndIndex[time_flag];i++)
        displayData.append(dataArray[i].toInt());
    // 更新曲线显示
    QVector<double> xData; //x
    for (int i = dataStartIndex[time_flag]; i <= dataEndIndex[time_flag]; ++i) {
        xData.append(i);
    }

    ui->widget->graph(0)->setData(xData, displayData);
    ui->widget->xAxis->setRange(dataStartIndex[time_flag], dataEndIndex[time_flag]);
    ui->widget->yAxis->setRange(0,1023); // 10 位二进制数的十进制值的最大值为 1023
    ui->widget->replot();
    }
    else{
        fftWindows();
        QVector<double> x_,y_;
        int h=0;
        for(int i = dataStartIndex[time_flag]; i <= dataEndIndex[time_flag]-32; i++)
        {
            x_ << i;
            y_ << point[i];
            //printf(" %lf\n",y_[h]);
            h++;
        }
        double N= (dataEndIndex[time_flag]-dataStartIndex[time_flag]+1);
        FFTCount(SampleRate,x_,y_,N);
        //printf(" %lf\n",SampleRate);
    }

    if(((maxdata-512)*k_ampl)/102.4>-1000&&((maxdata-511.5)*k_ampl)/102.4<1000)
        ui->label_max->setText("Max="+QString::number((floor(((maxdata*10-5115)*k_ampl)/1.024))/1000)+"mV");
    else
        ui->label_max->setText("Max="+QString::number((floor(((maxdata*10-5115)*k_ampl)/1024))/1000)+"V");

    if(((mindata-512)*k_ampl)/102.4>-1000&&((mindata-511.5)*k_ampl)/102.4<1000)
        ui->label_min->setText("Min="+QString::number((floor(((mindata*10-5115)*k_ampl)/1.024))/1000)+"mV");
    else
        ui->label_min->setText("Min="+QString::number((floor(((mindata*10-5115)*k_ampl)/1024))/1000)+"V");

    if(((maxdata-mindata)*k_ampl)/102.4<1000)
        ui->label_Vpp->setText("Vpp="+QString::number((floor(((maxdata*10-mindata*10)*k_ampl)/1.024))/1000)+"mV");
    else
        ui->label_Vpp->setText("Vpp="+QString::number((floor(((maxdata-mindata)*k_ampl)/102.4))/1000)+"V");
}

void Widget::FFTCount(double SampleRate,QVector<double> x_,QVector<double>y_,double N)
{

    fft* m_fft = new fft();
    //用上面的原始数据的初始化这个结构体类
    QVector<Complex> in_(x_.size());
    QVector<Complex> out_(x_.size());
    for(auto i = 0; i< x_.size();i++)
    {
        in_[i].rl = y_[i];
        // printf(" %d\n",i);
        in_[i].im = 0.0;
    }
    //printf("%d",in_.size());
    //调用接口 生成频域的 out_ 数据
    m_fft->fft1(in_,in_.size(),out_);
    //qDebug()<<"in_.size()="<<in_.size()<<'\n';
    //qDebug()<<"out_.size()="<<out_.size()<<'\n';
    QVector<double> x1,y1;

    //直流成分
    x1 << 0.0;
    y1 << 0.0;//(sqrt(out_[0].rl*out_[0].rl + out_[0].im*out_[0].im))/N;
    //交流成分
    for(auto i = 0;i<out_.size()/2;i++)
    {
        x1<<log10(i * (SampleRate/N));
        //用计算出的实部当做y轴画图
        y1<<log2((sqrt(out_[i].rl*out_[i].rl + out_[i].im*out_[i].im))*2/N);
        //printf("%lf:%lf ",x1[i],y1[i]);
    }
    //画图
    ui->widget->graph(0)->setData(x1,y1);
    ui->widget->replot();
}

// double Widget::frequency_value(int horizon_value){
//     double circle;
//     int f1,/*f2,*/f3,f0;
//     //printf("nmsl");
//     for(int i = dataStartIndex[time_flag]; i <= dataEndIndex[time_flag]-1; i++)
//     {
//         if((point[i]-horizon_value)*(point[i+1]-horizon_value)<=0)
//         {
//             f1 = i;
//             //printf("%d ",f1);
//             //printf("%d %d %d/n",point[f1-1],point[f1],point[f1+1]);
//             for(int j = i+2;j<=dataEndIndex[time_flag]-1;j++)
//             {
//                 if((point[j]-horizon_value)*(point[j+1]-horizon_value)<=0)
//                 {
//                     //f2 = j;
//                     //printf("%d ",f2);
//                     //printf("%d %d %d/n",point[f2-1],point[f2],point[f2+1]);
//                     for(int l = j+2;l<=dataEndIndex[time_flag]-1;l++)
//                     {
//                         //printf("%d ",l);
//                         if((point[l]-horizon_value)*(point[l+1]-horizon_value)<=0)
//                         {
//                             f3 = l;
//                             //printf("%d ",f3);
//                             //printf("%d %d %d/n",point[f3-1],point[f3],point[f3+1]);
//                             f0 = f3 - f1;
//                             //printf(" %d",f0);
//                             circle = f0 * (k_time/20);
//                             //printf(" %ld\n", f0 * (k_time/20));
//                             return circle;
//                             //break;
//                         }
//                         // else
//                         // {
//                         //     circle = 500000000;
//                         //     //break;
//                         // }
//                     }
//                 }
//                 // else
//                 // {
//                 //     circle = 500000000;
//                 //     //break;
//                 // }
//             }
//         }
//         // else
//         // {
//         //     circle = 500000000;
//         //     //break;
//         // }
//     }
//     return 500000000;  //circle
// }

double Widget::frequency_value(int horizon_value){
    double circle;
    double start,end;
    int point_num=0,circle_num=0;
    int circle_ok=0;
    int i=0,j=0;
    for(i = dataStartIndex[time_flag]; i <= dataEndIndex[time_flag]-1; i++)
    {
        if((point[i]-horizon_value)*(point[i+1]-horizon_value)<=0)
        {
            start = i;
            end = i;
            j=i+2;
            while(j<=dataEndIndex[time_flag]-1)
            {
                if((point[j]-horizon_value)*(point[j+1]-horizon_value)<=0)
                {
                    point_num++;
                    if(point_num%2==0)
                    {
                        circle_num++;
                        end=j;
                        circle_ok=1;
                        circle=((end-start)/circle_num)* (k_time/20);
                    }
                    j+=2;
                }
                else
                {
                    j++;
                }
            }
        }
        if(circle_ok)
        {
            break;
        }
    }
    if(circle_ok)
        return circle;  //circle
    else
        return 500000000;  //circle
}

//CSV文件保存
QString Widget::lastSavePath()
{
    return (QSettings(QApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat).value("lastSavePath", QApplication::applicationDirPath()).toString()) ;
}

//保存文件为csv 文件
void Widget::saveCsvFile()
{

    //保存数据的文件路径
    path = Widget::lastSavePath();
    QString defaultFileName = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss") + ".csv";

    QFileDialog dialog;
    dialog.setStyleSheet("background-color: yellow;"); // 设置背景颜色为黄色
    QString csvFileName = dialog.getSaveFileName(nullptr, QApplication::applicationName(), defaultFileName , tr("csv File(*.csv)"),0,QFileDialog::DontUseNativeDialog);
    //QColor color = QColorDialog::getColor(Qt::red,this,tr("选择颜色"));
    //QString csvFileName = QFileDialog::getSaveFileName(this, QApplication::applicationName(), path, tr("csv File(*.csv)"));
    //QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss")

    QFile file(csvFileName);
    //    if (!file.exists())		//文件不存在的时候便新建，以当天日期为命名
    //    {
    //        file.open(QIODevice::WriteOnly);
    //        QTextStream txtOutPut(&file);
    //        txtOutPut <<  QDateTime::currentDateTime().toString("yyyy-MM-dd") + "\n";
    //        txtOutPut << "X,Y\n";	//注意，每行数据结束后要加换行符
    //        file.close();
    //    }

    //file.open(QIODevice::WriteOnly|QIODevice::Append);

    //file.open(QIODevice::Append);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream txtOutPut(&file);

    // 保存数据到文件
    for (int i = 0; i < ui->widget->graph(0)->dataCount(); ++i) {
        txtOutPut << ui->widget->graph(0)->data()->at(i)->key << "," << ui->widget->graph(0)->data()->at(i)->value << "\n\n";
    }
    file.flush();
    file.close();

}

void Widget::closeCsvFile()
{
    if(xlsxDevice)
    {
        xlsxRow = 1;
        xlsxDevice->save();
        delete xlsxDevice;
        xlsxDevice = nullptr;
        //ui->statusBar->showMessage (tr("取消保存数据到文件！"));
    }
}

// 读取CSV文件数据
void Widget::readCSVFile(QString fileName)
{
    // bool FLAG_readCSVFile = false;
    //读取csv文件
    //QDir dir = QDir::current();
    //QFile file(dir.filePath(fileName));
    QFile file(fileName);
    qDebug()<<"fileName="<<fileName<<'\n';
    QStringList item;
    int idx = 0;
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<<"open file failed!"<<'\n';
        //return ;
    }
    else
    {
        //        if(FLAG_readCSVFile)
        //        {
        //暂停接受的数据显示
        // PBUT23_styleSheet = "border-radius: 5px;"
        //                     "background-color:red;";
        // ui->pushButton_23-> setText("运行/暂停");
        // ui->label_11->setText("STOP");
        // ui->label_11->setStyleSheet("color:red");
        // ui->label_11->setAlignment(Qt::AlignCenter); //暂停
        // //Timer_->stop();
        // PBUT23_flag = !PBUT23_flag;
        // ui->pushButton_23->setStyleSheet(PBUT23_styleSheet);

        // Flag_Display = 3;
        // X_Range = 0;
        // Y_Range = 0;
        // ui->dial_X_6->setValue(50);
        // ui->dial_X_3->setValue(50);

        // 清除原有数据
        // timer->stop();
        measure_data = 2; // pause
        ui->widget->graph(0)->data()->clear();
        //
        //        }
    }
    QTextStream stream(&file);
    // 逐行读取数据
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        line = line.trimmed();  ;
        if(line.isEmpty()){
            qDebug()<<"CSV FILE HAS READ DONE！"<<'\n';
            break;
        }
        item = line.split(",");

        if (item.size() == 2) {
            //单独显示
            //double x = item[0].toDouble();
            //double y = item[1].toDouble();
            // 添加数据到QCustomPlot
            //ui->customPlot->graph(0)->addData(x, y);

            //定时显示
            // xData[idx] = item[0].toDouble();
            // yData[idx] = item[1].toDouble();
            //Average[idx] =  yData[idx];
            idx++;

            //FLAG_readCSVFile = true;
        }
    }
    //    QVector<double> first = Average.mid(0, 4096);
    //    ReadAverage = std::accumulate(first.begin(), first.end(), 0.0) / first.size();


    file.close();
    // 更新QCustomPlot显示
    //ui->customPlot->replot();
    qDebug()<<"read csv file successfully!"<<'\n';

    QVector<double> displayData; //y
    for(int i = 0; i <= 399; i++)
        displayData.append(yData[i]);

    QVector<double> xsize; //x
    for (int i = 0; i <= 399; i++) {
        xsize.append(xData[i]);

    }

    // for (int i=0; i<=399; i++)
    // {
    //     x[i] = xData[i] ; // x goes from -1 to 1
    //     y[i] = yData[i] ;
    // }
    ui->widget->graph(0)->setData(xsize, displayData);
    ui->widget->replot();
}

void Widget::on_pbtn_load_clicked()
{
    path = Widget::lastSavePath();
    QFileDialog dialog;
    dialog.setStyleSheet("background-color: yellow;"); // 设置背景颜色为黄色
    QString FileName = dialog.getOpenFileName(nullptr, QApplication::applicationName(), path, tr("csv File(*.csv)"),0,QFileDialog::DontUseNativeDialog);
    //+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss")+".csv"
    readCSVFile(FileName);
}

void Widget::on_pbtn_save_clicked()
{
    // 暂停接受的数据显示
    timer->stop();
    saveCsvFile();
    timer->start();
    // 获取QCustomPlot的数据

    //double xData[ui->customPlot->graph(0)->dataCount()];
    //double yData[ui->customPlot->graph(0)->dataCount()];
    for (int i = 0;i < ui->widget->graph(0)->dataCount();++i) {
        xData[i] = ui->widget->graph(0)->data()->at(i)->key;
        yData[i] = ui->widget->graph(0)->data()->at(i)->value;
        //qDebug() << xData[i] << " "<< yData[i] << endl;
    }

    // 打开文件
    QFile file("data.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // 保存数据到文件
        for (int i = 0; i < ui->widget->graph(0)->dataCount(); ++i) {
            out << xData[i] << " " << yData[i] << "\n";
        }

        file.close();
    }
}

/* 判断路径是否存在，不存在则新建，只能创建一级子目录，必须保证上级目录存在 */
bool Widget::isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists()) {
        return true;
    } else {
        // 创建一级子目录，必须保证上级目录存在
        bool ok = dir.mkdir(fullPath);
        return ok;
    }
}

void Widget::on_pbtn_prtsc_clicked()
{
    // 获取程序运行路径
    QString savePath = QCoreApplication::applicationDirPath() + "/output";
    // 判断文件路径
    if (isDirExist(savePath)) {
    } else {
        qDebug() << "保存文件目录未找到!";
        QMessageBox::warning(this,
                             "warning",
                             "保存文件目录未找到!",
                             QMessageBox::Yes,
                             QMessageBox::Yes);
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "保存波形数据",                    // 对话框的标题
                                                    savePath,                        // 保存的默认路径为程序运行路径
                                                    "Save Picture (*.png *jpg)");    // 打开文件的类型，;隔开
    // 如果用户点击了“取消”按钮
    if (fileName.isNull())
        return;
    // 保存图片

    ui->widget->savePng(fileName, 1280, 800, 1.0, -1, 255);
}

void Widget::InitGPIO(void)
{
    // 导出GPIO_53
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "53", 2);

    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_53为输入模式
    fdd = open("/sys/class/gpio/gpio53/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);
int
/*-------------------*/

    // 导出GPIO_52
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "52", 2);

    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_52为输入模式
    fdd = open("/sys/class/gpio/gpio52/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);


/*TIME_STATE*/

    // 导出GPIO_59
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "59", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_59为输入模式
    fdd = open("/sys/class/gpio/gpio59/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

    // 导出GPIO_58
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "58", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_58为输入模式
    fdd = open("/sys/class/gpio/gpio58/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

    // 导出GPIO_57
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "57", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_57为输入模式
    fdd = open("/sys/class/gpio/gpio57/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

    // 导出GPIO_56
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "56", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_56为输入模式
    fdd = open("/sys/class/gpio/gpio56/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

    // 导出GPIO_55
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "55", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_55为输入模式
    fdd = open("/sys/class/gpio/gpio55/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

/*CHANGE_FLAG*/
    // 导出GPIO_54
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "54", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_54为输入模式
    fdd = open("/sys/class/gpio/gpio54/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

/*MEASURE_STATE*/
    // 导出GPIO_51->47
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "47", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_51->47为输入模式
    fdd = open("/sys/class/gpio/gpio47/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

    // 导出GPIO_50->46
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "46", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_50->46为输入模式
    fdd = open("/sys/class/gpio/gpio46/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

 /*AMPL_STATE*/
    // 导出GPIO_49->45
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "45", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_49->45为输入模式
    fdd = open("/sys/class/gpio/gpio45/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

    // 导出GPIO_48->44
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "44", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_48->44为输入模式
    fdd = open("/sys/class/gpio/gpio44/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

    // 导出GPIO_47->51
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "51", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_47->51为输入模式
    fdd = open("/sys/class/gpio/gpio51/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "in", 2);

    if (ret < 0) {
        printf("read direction failed\n");
    }
    ::close(fdd);

    // 导出GPIO_46->50
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "50", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);

    // 设置GPIO_46->50为输出模式
    // fdd = open("/sys/class/gpio/gpio50/direction", O_WRONLY);
    // if (fdd < 0) {
    //     //printf("open direction failed\n");
    // }
    // ret = write(fdd, "out", 3);

    // if (ret < 0) {
    //     printf("write direction failed\n");
    // }
    // ::close(fdd);
    fdd = open("/sys/class/gpio/export", O_WRONLY);
    if (fdd < 0) {
        printf("open export failed\n");
    }
    ret = write(fdd, "60", 2);
    if (ret < 0) {
        printf("write export failed\n");
    }
    ::close(fdd);
    fdd = open("/sys/class/gpio/gpio60/direction", O_WRONLY);
    if (fdd < 0) {
        //printf("open direction failed\n");
    }
    ret = write(fdd, "out", 3);

    if (ret < 0) {
        printf("write direction failed\n");
    }
    ::close(fdd);


}

void Widget::resizeEvent(QResizeEvent *event)
{
    // 调用父类的resizeEvent方法
    QWidget::resizeEvent(event);

    // 重绘波形图，以确保波形完整填充窗口
    ui->widget->replot();
}

void Widget::RightLayoutInit()
{
    /*                      第一个                 */
    QString styleSheet00 ="border-style: solid;"
                           "border-width: 3px;"
                           "border-color: yellow;"
                           "color: black;"
                           "font-weight: bold;"
                           "background-color: gold;"
                           "font: 750 10pt SimHei;"
                           "border-top-left-radius: 0px;"
                           "border-top-right-radius: 12px;"
                           "border-bottom-right-radius:0px;"
                           "border-bottom-left-radius: 0px;";

    ui->label_spird->setStyleSheet(styleSheet00);
    ui->label_ftrig->setStyleSheet(styleSheet00);
    // ui->label_cflag->setStyleSheet(styleSheet00);
    ui->label_meas->setStyleSheet(styleSheet00);
    ui->label_frequ->setStyleSheet(styleSheet00);
    ui->label_circle->setStyleSheet(styleSheet00);

    // /*                      第二个                 */
    QString styleSheet01 ="border-style: solid;"
                           "border-width: 3px;"
                           "border-color: white;"
                           "color: black;"
                           "font-weight: bold;"
                           "background-color: silver;"
                           "font: 750 15pt SimHei;"
                           "border-top-left-radius: 12px;"
                           "border-top-right-radius: 12px;"
                           "border-bottom-right-radius:12px;"
                           "border-bottom-left-radius: 12px;";

    ui->label_ampl->setStyleSheet(styleSheet01);
    ui->label_time->setStyleSheet(styleSheet01);
    ui->label_max->setStyleSheet(styleSheet01);
    ui->label_min->setStyleSheet(styleSheet01);
    ui->label_Vpp->setStyleSheet(styleSheet01);

    ui -> label_spird -> setAlignment(Qt::AlignCenter);
    ui -> label_ftrig -> setAlignment(Qt::AlignCenter);
    // ui -> label_cflag -> setAlignment(Qt::AlignCenter);
    ui -> label_meas -> setAlignment(Qt::AlignCenter);
    ui -> label_ampl -> setAlignment(Qt::AlignCenter);
    ui -> label_time -> setAlignment(Qt::AlignCenter);
    ui -> label_max -> setAlignment(Qt::AlignCenter);
    ui -> label_min -> setAlignment(Qt::AlignCenter);
    ui -> label_Vpp -> setAlignment(Qt::AlignCenter);

}

void Widget::origionWindows()
    {
        ui->widget->setInteraction(QCP::iNone);
        ui->widget->xAxis->setRange(120,280);
        ui->widget->yAxis->setRange(0,1023);

        QPen pen;
        pen.setWidth(5);//曲线的粗细
        pen.setColor(Qt::yellow);
        ui->widget->graph(0)->setPen(pen);//曲线颜色设置
        ui->widget->graph(0)->setAntialiasedFill(true);//设置抗锯齿

        ui->widget->setStyleSheet("background-color:black");
        ui->widget->xAxis->setTickLabels(false);
        ui->widget->xAxis->ticker()->setTickCount(8);                //刻度数量
        ui->widget->xAxis->setNumberFormat("f");                      //x轴刻度值格式
        ui->widget->xAxis->setNumberPrecision(1);                     //刻度值精度

        ui->widget->yAxis->setTickLabels(false);
        ui->widget->yAxis->ticker()->setTickCount(10);                //刻度数量
        ui->widget->yAxis->setNumberFormat("f");                      //y轴刻度值格式
        ui->widget->yAxis->setNumberPrecision(0.01);                     //刻度值精度

        // ui->widget->yAxis->setVisible(true);//显示y轴
        // ui->widget->xAxis->setVisible(true);//显示x轴
        // ui->widget->yAxis2->setVisible(true);//显示y轴2
        // ui->widget->xAxis2->setVisible(true);//显示x轴2
        // ui->widget->axisRect()->setupFullAxesBox();//四边安装轴并显示
        // ui->widget->xAxis->grid()->setPen(QPen(Qt::white,4));
        // ui->widget->yAxis->grid()->setPen(QPen(Qt::white,4));
        // ui->widget->xAxis2->grid()->setPen(QPen(Qt::white,4));
        // ui->widget->yAxis2->grid()->setPen(QPen(Qt::white,4));


        QSharedPointer<QCPAxisTickerFixed>MyTicker(new QCPAxisTickerFixed);
        MyTicker->setTickStep(102.3);
        MyTicker->setTickCount(10);
        ui->widget->yAxis->setTicker(MyTicker);

        QSharedPointer<QCPAxisTickerFixed>MyTicker2(new QCPAxisTickerFixed);
        MyTicker2->setTickStep(20);
        MyTicker2->setTickCount(8);
        ui->widget->xAxis->setTicker(MyTicker2);


        ui->widget->axisRect()->setBackground(QBrush(Qt::black));//背景黑色
        ui->widget->xAxis->grid()->setPen(QPen(QColor(180, 180, 180), 2, Qt::PenStyle::SolidLine));//网格白色虚线
        ui->widget->yAxis->grid()->setPen(QPen(QColor(180, 180, 180), 2, Qt::PenStyle::SolidLine));//网格白色虚线
        ui->widget->xAxis->grid()->setSubGridPen(QPen(QColor(50, 50, 50), 1, Qt::DotLine));//网格浅色点线
        ui->widget->yAxis->grid()->setSubGridPen(QPen(QColor(50, 50, 50), 1, Qt::DotLine));//网格浅色点线
        ui->widget->xAxis->grid()->setSubGridVisible(true);//显示x轴子网格线
        ui->widget->yAxis->grid()->setSubGridVisible(true);//显示要轴子网格线
        ui->widget->xAxis->grid()->setZeroLinePen(QPen(Qt::white));//x轴0线颜色白色
        ui->widget->yAxis->grid()->setZeroLinePen(QPen(Qt::white));//y轴0线颜色白色

        ui->widget->setBackground(QBrush(QColor(0, 0, 0))); // 设置为黑色
}

void Widget::fftWindows()
{
    X0 = 0/*(dataStartIndex[time_flag]+dataEndIndex[time_flag]+1)/2*/;
    QPen ReferLinePen(Qt::white);
    ReferLinePen.setWidth(4);
    ReferLinePen.setStyle(Qt::PenStyle::SolidLine);

    //中心参考线
    // m_refer_lineY0 = new QCPItemStraightLine(ui->widget);
    m_refer_lineY0->point1->setCoords(X0, 100);               //设置点1的值
    m_refer_lineY0->point2->setCoords(X0, 300);              //设置点2的值
    m_refer_lineY0->setPen(ReferLinePen);

    m_refer_lineX0->point1->setCoords(100, 0);               //设置点1的值
    m_refer_lineX0->point2->setCoords(200, 0);              //设置点2的值
    m_refer_lineX0->setPen(ReferLinePen);

    QColor clrDefault(Qt::white);
    QPen penDefault(clrDefault);
    // m_arrowY0 = new QCPItemLine(ui->widget);
    m_arrowY0->setPen(penDefault);
    m_arrowY0->setLayer("overlay");
    m_arrowY0->setClipToAxisRect(false);
    m_arrowY0->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowY0->start->setCoords(X0, 512-15);
    m_arrowY0->end->setCoords(X0, 512);

    // m_arrowX0 = new QCPItemLine(ui->widget);
    m_arrowX0->setPen(penDefault);
    m_arrowX0->setLayer("overlay");
    m_arrowX0->setClipToAxisRect(false);
    m_arrowX0->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
    m_arrowX0->start->setCoords(500-20, 0);
    m_arrowX0->end->setCoords(500, 0);

    //重设绘图格式
    ui->widget->xAxis->setTickLabelSide(QCPAxis::LabelSide::lsInside);
    ui->widget->xAxis->setTickLabelColor(Qt::white);
    ui->widget->yAxis->setTickLabelSide(QCPAxis::LabelSide::lsInside);
    ui->widget->yAxis->setTickLabelColor(Qt::white);

    ui->widget->xAxis->setTickLabels(true);
    ui->widget->yAxis->setTickLabels(true);

    ui->widget->axisRect()->setupFullAxesBox();
    ui->widget->yAxis->setRange(0,10);
    ui->widget->yAxis->ticker()->setTickOrigin(0);//改变刻度原点为64 y

    ui->widget->xAxis->setRange(0,8);
    ui->widget->xAxis->ticker()->setTickOrigin(0);//改变刻度原点为64 x
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // 创建一个自定义的刻度间隔对象
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(0.5); // 设置刻度间隔为0.5
    ui->widget->xAxis->setTicker(fixedTicker);

    QSharedPointer<QCPAxisTickerFixed> fixedTicker_y(new QCPAxisTickerFixed);
    fixedTicker_y->setTickStep(1); // 设置刻度间隔为20
    fixedTicker_y->setTickOrigin(0);
    ui->widget->yAxis->setTicker(fixedTicker_y);
}

void Widget::CursorDataWindows()
{
    QString styleSheet02 =  "color: white;"
                           "font-weight: bold;"
                           "font: 11 11pt Microsoft YaHei;";
    ui -> label_x1 -> setStyleSheet(styleSheet02);
    ui -> label_x2 -> setStyleSheet(styleSheet02);
    ui -> label_ts -> setStyleSheet(styleSheet02);
    ui -> label_fs -> setStyleSheet(styleSheet02);
    ui -> label_y1 -> setStyleSheet(styleSheet02);
    ui -> label_y2 -> setStyleSheet(styleSheet02);
    ui -> label_vpp -> setStyleSheet(styleSheet02);
    // char str[30] ;
    double midx = (dataStartIndex[time_flag]+dataEndIndex[time_flag]+1)/2;

    if(cursors_flag)
    {
        if((((X1-midx)*k_time)/20)>-1000 && (((X1-midx)*k_time)/20)<1000)
            ui->label_x1->setText("光标X1 :  "+(QString::number( (floor((X1-midx)*k_time)/20) ))+"  ns");
        else if(abs(((X1-midx)*k_time)/20)<1000000 && abs(((X1-midx)*k_time)/20)>1000)
            ui->label_x1->setText("光标X1 :  "+(QString::number( (floor((X1-midx)*k_time)/20)/1000 ))+"  us");
        else
            ui->label_x1->setText("光标X1 :  "+(QString::number( (floor((X1-midx)*k_time)/20)/1000000 ))+"  ms");

        if((((X2-midx)*k_time)/20)>-1000 && (((X2-midx)*k_time)/20)<1000)
            ui->label_x2->setText("光标X2 :  "+(QString::number( (floor((X2-midx)*k_time)/20) ))+"  ns");
        else if(abs(((X2-midx)*k_time)/20)<1000000 && abs(((X2-midx)*k_time)/20)>1000)
            ui->label_x2->setText("光标X2 :  "+(QString::number( (floor((X2-midx)*k_time)/20)/1000 ))+"  us");
        else
            ui->label_x2->setText("光标X2 :  "+(QString::number( (floor((X2-midx)*k_time)/20)/1000000 ))+"  ms");

        if(abs(((X2-X1)*k_time)/20)<1000)
            ui->label_ts->setText("Ts    :  "+(QString::number( abs(floor((X2-X1)*k_time)/20) ))+"  ns");
        else if(abs(((X2-X1)*k_time)/20)<1000000 && abs(((X2-X1)*k_time)/20)>=1000)
            ui->label_ts->setText("Ts    :  "+(QString::number( abs(floor((X2-X1)*k_time)/20)/1000 ))+"  us");
        else
            ui->label_ts->setText("Ts    :  "+(QString::number( abs(floor((X2-X1)*k_time)/20)/1000000 ))+"  ms");


        if(abs(1/(((X2-X1)*k_time))/20)<1000)
            ui->label_fs->setText("Fs    :  "+(QString::number( abs(1/(floor((X2-X1)*k_time)/20))*1000000000 ))+"  Hz");
        else if(abs(1/(((X2-X1)*k_time))/20)<1000000 && abs(1/(((X2-X1)*k_time))/20)>=1000)
            ui->label_fs->setText("Fs    :  "+(QString::number( abs(1/(floor((X2-X1)*k_time)/20))*1000000 ))+"  KHz");
        else
            ui->label_fs->setText("Fs    :  "+(QString::number( abs(1/(floor((X2-X1)*k_time)/20))*1000 ))+"  MHz");


        if(((Y1-512)*k_ampl)/102.4>-1000&&((Y1-511.5)*k_ampl)/102.4<1000)
            ui->label_y1->setText("光标Y1 :  "+(QString::number(((Y1*10-5115)*k_ampl/1.024)/1000))+" mV");
        else
            ui->label_y1->setText("光标Y1 :  "+(QString::number(((Y1*10-5115)*k_ampl/1024)/1000))+"  V");

        if(((Y2-512)*k_ampl)/102.4>-1000&&((Y2-512)*k_ampl)/102.4<1000)
            ui->label_y2->setText("光标Y2 :  "+(QString::number(((Y2*10-5115)*k_ampl/1.024)/1000))+" mV");
        else
            ui->label_y2->setText("光标Y2 :  "+(QString::number(((Y2*10-5115)*k_ampl/1024)/1000))+"  V");

        if(abs(((Y2-Y1)*k_ampl)/0.1024)<1000000)
            ui->label_vpp->setText("Vpp   :  "+(QString::number(abs((Y2-Y1)*10*k_ampl/1.024)/1000))+" mV");
        else
            ui->label_vpp->setText("Vpp   :  "+(QString::number(abs((Y2-Y1)*k_ampl/102.4)/1000))+" V");

        ui -> label_x1 -> setAlignment(Qt::AlignLeft);
        ui -> label_x2 -> setAlignment(Qt::AlignLeft);
        ui -> label_ts -> setAlignment(Qt::AlignLeft);
        ui -> label_fs -> setAlignment(Qt::AlignLeft);
        ui -> label_y1 -> setAlignment(Qt::AlignLeft);
        ui -> label_y2 -> setAlignment(Qt::AlignLeft);
        ui -> label_vpp -> setAlignment(Qt::AlignLeft);
    }

    else
    {
        ui->label_x1->setText("光标X1 :  ?  ms");
        ui->label_x2->setText("光标X2 :  ?  ms");
        ui->label_ts->setText("Ts    :  ?  ms");
        ui->label_fs->setText("Fs    :  ?  Hz");
        ui->label_y1->setText("光标Y1 :  ？  V");
        ui->label_y2->setText("光标Y2 :  ？  V");
        ui->label_vpp->setText("VPP  :  ？  V");

        ui -> label_x1 -> setAlignment(Qt::AlignLeft);
        ui -> label_x2 -> setAlignment(Qt::AlignLeft);
        ui -> label_ts -> setAlignment(Qt::AlignLeft);
        ui -> label_fs -> setAlignment(Qt::AlignLeft);
        ui -> label_y1 -> setAlignment(Qt::AlignLeft);
        ui -> label_y2 -> setAlignment(Qt::AlignLeft);
        ui -> label_vpp -> setAlignment(Qt::AlignLeft);
    }
}

void Widget::on_pbtn_x1_clicked()
{
    if(!X1_flag){
        // m_refer_lineV->point1->setCoords(X1, 800);         //设置点1的值
        // m_refer_lineV->point2->setCoords(X1, 700);         //设置点2的值
        QPen ReferLinePen(Qt::green);
        ReferLinePen.setWidth(2);
        ReferLinePen.setStyle(Qt::PenStyle::DotLine);
        m_refer_lineV->setPen(ReferLinePen);
        m_arrow->setPen(ReferLinePen);
    }
    else{
        QPen ReferLinePen(Qt::red);
        ReferLinePen.setWidth(2);
        ReferLinePen.setStyle(Qt::PenStyle::DotLine);
        m_refer_lineV->setPen(ReferLinePen);
        m_arrow->setPen(ReferLinePen);
    }
    X1_flag = !X1_flag;
}

void Widget::on_pbtn_x2_clicked()
{
    if(!X2_flag){
        // m_refer_lineV2->point1->setCoords(X2, 800);         //设置点1的值
        // m_refer_lineV2->point2->setCoords(X2, 700);         //设置点2的值
        QPen ReferLinePen(Qt::green);
        ReferLinePen.setWidth(2);
        ReferLinePen.setStyle(Qt::PenStyle::DotLine);
        m_refer_lineV2->setPen(ReferLinePen);
        m_arrowX2->setPen(ReferLinePen);
    }
    else{
        QPen ReferLinePen(Qt::red);
        ReferLinePen.setWidth(2);
        ReferLinePen.setStyle(Qt::PenStyle::DotLine);
        m_refer_lineV2->setPen(ReferLinePen);
        m_arrowX2->setPen(ReferLinePen);
    }
    X2_flag = !X2_flag;
}

void Widget::on_pbtn_y1_clicked()
{
    if(!Y1_flag){
        QPen ReferLinePen(Qt::green);
        ReferLinePen.setWidth(2);
        ReferLinePen.setStyle(Qt::PenStyle::DotLine);
        m_refer_lineY1->setPen(ReferLinePen);
        m_arrowY1->setPen(ReferLinePen);
    }
    else{
        QPen ReferLinePen(Qt::red);
        ReferLinePen.setWidth(2);
        ReferLinePen.setStyle(Qt::PenStyle::DotLine);
        m_refer_lineY1->setPen(ReferLinePen);
        m_arrowY1->setPen(ReferLinePen);
    }
    Y1_flag = !Y1_flag;
}

void Widget::on_pbtn_y2_clicked()
{
    if(!Y2_flag){
        QPen ReferLinePen(Qt::green);
        ReferLinePen.setWidth(2);
        ReferLinePen.setStyle(Qt::PenStyle::DotLine);
        m_refer_lineY2->setPen(ReferLinePen);
        m_arrowY2->setPen(ReferLinePen);
    }
    else{
        QPen ReferLinePen(Qt::red);
        ReferLinePen.setWidth(2);
        ReferLinePen.setStyle(Qt::PenStyle::DotLine);
        m_refer_lineY2->setPen(ReferLinePen);
        m_arrowY2->setPen(ReferLinePen);
    }
    Y2_flag = !Y2_flag;
}



void Widget::on_pbtn_cursor_clicked()
{
    if(!cursors_flag){
        m_refer_lineY1->setVisible(true);
        m_refer_lineY2->setVisible(true);
        m_arrowY1->setVisible(true);
        m_arrowY2->setVisible(true);

        m_refer_lineV2->setVisible(true);
        m_refer_lineV->setVisible(true);
        m_arrow->setVisible(true);
        m_arrowX2->setVisible(true);

        ui->stackedWidget->setCurrentWidget(ui->page);
        ui->stackedWidget->setVisible(true);
        CursorDataWindows();
    }
    else{
        m_refer_lineY1->setVisible(false);
        m_refer_lineY2->setVisible(false);
        m_arrowY1->setVisible(false);
        m_arrowY2->setVisible(false);

        m_refer_lineV2->setVisible(false);
        m_refer_lineV->setVisible(false);
        m_arrow->setVisible(false);
        m_arrowX2->setVisible(false);

        ui->stackedWidget->setCurrentWidget(ui->page);
        ui->stackedWidget->setVisible(false);

    }
    cursors_flag = !cursors_flag;
}


void Widget::on_pbtn_valueUp_clicked()
{
    double axstep = 1;
    double aystep = 10;
    if(X1_flag)
    {
        if(X1>=dataEndIndex[time_flag])
            X1 = dataStartIndex[time_flag];
        else
            X1 = X1 + axstep;
        m_refer_lineV->point1->setCoords(X1, 800);               //设置点1的值
        m_refer_lineV->point2->setCoords(X1, 700);              //设置点2的值
        m_arrow->start->setCoords(X1, 1023);
        m_arrow->end->setCoords(X1, 1018);
    }
    else if(X2_flag)
    {
        if(X2>=dataEndIndex[time_flag])
            X2 = dataStartIndex[time_flag];
        else
            X2 = X2 + axstep;
        m_refer_lineV2->point1->setCoords(X2, 800);               //设置点1的值
        m_refer_lineV2->point2->setCoords(X2, 700);              //设置点2的值
        m_arrowX2->start->setCoords(X2, 1023);
        m_arrowX2->end->setCoords(X2, 1018);
    }
    else if(Y1_flag)
    {
        if(Y1>=1023)
            Y1 = 0;
        else
            Y1 = Y1 + aystep;
        m_refer_lineY1->point1->setCoords(100, Y1);               //设置点1的值
        m_refer_lineY1->point2->setCoords(300, Y1);              //设置点2的值
        m_arrowY1->start->setCoords(120, Y1);
        m_arrowY1->end->setCoords(122, Y1);
    }
    else if(Y2_flag)
    {
        if(Y2>=1023)
            Y2 = 0;
        else
            Y2 = Y2 + aystep;
        m_refer_lineY2->point1->setCoords(100, Y2);               //设置点1的值
        m_refer_lineY2->point2->setCoords(300, Y2);              //设置点2的值
        m_arrowY2->start->setCoords(120, Y2);
        m_arrowY2->end->setCoords(122, Y2);
    }
    CursorDataWindows();
}

void Widget::on_pbtn_valueDown_clicked()
{
    double mxstep = 1;
    double mystep = 10;
    if(X1_flag)
    {
        if(X1<=dataStartIndex[time_flag])
            X1 = dataEndIndex[time_flag];
        else
            X1 = X1 - mxstep;

        m_refer_lineV->point1->setCoords(X1, 800);               //设置点1的值
        m_refer_lineV->point2->setCoords(X1, 700);              //设置点2的值
        m_arrow->start->setCoords(X1, 1023);
        m_arrow->end->setCoords(X1, 1018);
    }
    else if(X2_flag)
    {
        if(X2<=dataStartIndex[time_flag])
            X2 = dataEndIndex[time_flag];
        else
            X2 = X2 - mxstep;
        m_refer_lineV2->point1->setCoords(X2, 800);               //设置点1的值
        m_refer_lineV2->point2->setCoords(X2, 700);              //设置点2的值
        m_arrowX2->start->setCoords(X2, 1023);
        m_arrowX2->end->setCoords(X2, 1018);
    }
    else if(Y1_flag)
    {
        if(Y1<=0)
            Y1 = 1023;
        else
            Y1 = Y1 - mystep;
        m_refer_lineY1->point1->setCoords(100, Y1);               //设置点1的值
        m_refer_lineY1->point2->setCoords(300, Y1);              //设置点2的值
        m_arrowY1->start->setCoords(120, Y1);
        m_arrowY1->end->setCoords(122, Y1);
    }
    else if(Y2_flag)
    {
        if(Y2<=0)
            Y2 = 1023;
        else
            Y2 = Y2 - mystep;
        m_refer_lineY2->point1->setCoords(100, Y2);               //设置点1的值
        m_refer_lineY2->point2->setCoords(300, Y2);              //设置点2的值
        m_arrowY2->start->setCoords(120, Y2);
        m_arrowY2->end->setCoords(122, Y2);
    }
    CursorDataWindows();
}

void Widget::on_pbtn_fft_clicked()
{
    fft_flag = !fft_flag;
}

void Widget::on_pbtn_rst_clicked()
{
    if(!rst_flag)
    {
        X1 = ((dataEndIndex[time_flag] + dataStartIndex[time_flag]+1)/2)-10;
        X2 = X1 + 20;

        m_refer_lineV->point1->setCoords(X1, 800);               //设置点1的值
        m_refer_lineV->point2->setCoords(X1, 700);              //设置点2的值
        m_arrow->start->setCoords(X1, 1023);
        m_arrow->end->setCoords(X1, 1018);

        m_refer_lineV2->point1->setCoords(X2, 800);               //设置点1的值
        m_refer_lineV2->point2->setCoords(X2, 700);              //设置点2的值
        m_arrowX2->start->setCoords(X2, 1023);
        m_arrowX2->end->setCoords(X2, 1018);

        Y1 = 512 + 150;
        Y2 = 512 - 150;

        m_refer_lineY1->point1->setCoords(100, Y1);               //设置点1的值
        m_refer_lineY1->point2->setCoords(300, Y1);              //设置点2的值
        m_arrowY1->start->setCoords(120, Y1);
        m_arrowY1->end->setCoords(122, Y1);

        m_refer_lineY2->point1->setCoords(100, Y2);               //设置点1的值
        m_refer_lineY2->point2->setCoords(300, Y2);              //设置点2的值
        m_arrowY2->start->setCoords(120, Y2);
        m_arrowY2->end->setCoords(122, Y2);
    }
    rst_flag = !rst_flag;

}

void Widget::on_pbtn_pause_clicked()
{
    if(!pause_flag){
        timer->stop();
        ui->label_meas->setText("PAUSE");
    }
    else{
        timer->start();
        ui->label_meas->setText("RUN");
    }
    pause_flag = !pause_flag;
}


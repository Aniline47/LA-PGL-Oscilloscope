#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include "qcustomplot.h"

#include <QApplication>
#include "CSpiDev.h"
#include <QObject>
#include <QLabel>
#include <QDebug>

#include <QCheckBox>

#include "xlsxdocument.h"


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    static int fd;
    int fdd, ret;
    QTimer*Timer_;

    QXlsx::Document* xlsxDevice = nullptr;
    uint32_t xlsxRow = 0;

    void saveCsvFile();
    void closeCsvFile();
    QString lastSavePath();
    QStringList getLineNames();
    bool isDirExist(QString fullPath);
    void setLastSavePath(const QString &path);
    void readCSVFile(QString fileName);
    QString path;

private slots:
    void updateData();

    void on_pbtn_load_clicked();

    void on_pbtn_save_clicked();

    void on_pbtn_x1_clicked();

    void on_pbtn_cursor_clicked();

    void on_pbtn_prtsc_clicked();

    void on_pbtn_x2_clicked();

    void on_pbtn_valueUp_clicked();

    void on_pbtn_y1_clicked();

    void on_pbtn_y2_clicked();

    void on_pbtn_valueDown_clicked();

    void on_pbtn_fft_clicked();

    void on_pbtn_rst_clicked();

    void on_pbtn_pause_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::Widget *ui;
    QTimer *timer;
    QVector<QString> dataArray;
    QLabel *label;

    //XY轴光标
    double X1 = 220;
    double X2 = 240;
    bool Flag_X12 = false; // false X1;

    bool Flag_Y12 = false; // false Y1;
    double Y1 = 850;
    double Y2 = 730;

    double X0 = 200;

    //中心

    QCPItemStraightLine *m_refer_lineX0;//参考线 X0
    QCPItemStraightLine *m_refer_lineY0;//参考线 Y0
    QCPItemLine *m_arrowX0;//X0箭头
    QCPItemLine *m_arrowY0;//Y0箭头


    //纵光标
    QCPItemTracer *m_tracer_temp;//追踪器的一个标记
    QCPItemText *m_cur_Label_temp;//显示坐标的文本
    QCPItemTracer *m_tracer_power; // 参考线 X1
    QCPItemText *m_cur_Label_power; //参考线 X2
    QCPItemStraightLine *m_refer_lineV;//参考线 X1
    QCPItemStraightLine *m_refer_lineV2;//参考线 X2
    QCPItemLine *m_arrow;//X1箭头
    QCPItemLine *m_arrowX2;//X2箭头

    //横光标
    QCPItemTracer *m_tracer_Y1;//追踪器的一个标记
    QCPItemText *m_cur_Label_Y1;//显示坐标的文本
    QCPItemTracer *m_tracer_Y2; // 参考线 Y1
    QCPItemText *m_cur_Label_Y2; //参考线 Y2
    QCPItemStraightLine *m_refer_lineY1;//参考线 X1
    QCPItemStraightLine *m_refer_lineY2;//参考线 X2
    QCPItemLine *m_arrowY1;//Y1箭头
    QCPItemLine *m_arrowY2;//Y2箭头


    int dataStartIndex[8];
    int dataEndIndex[8];


    void InitGPIO(void);
    void RightLayoutInit();
    void processSpiData();
    void CursorDataWindows();
    void origionWindows();
    void fftWindows();

    void Ycursor();
    void Xcursor();
    void Midcursor();

    void FFTCount(double SampleRate,QVector<double> x_,QVector<double>y_,double N);
    double frequency_value(int horizon_value);

};

// void openSpi()
// {
//     g_SpiDev->openDevSpi();
// }
// void testSpi()
// {
//     g_SpiDev->testSpiWR();
// }
// void writeSpi(quint8 txBuf[], int len)
// {
//     g_SpiDev->writeSpiData(txBuf,len);
// }
// void readSpi(quint8 rxBuf[],unsigned int len)
// {
//     g_SpiDev->readSpiData(rxBuf,len);
// }
// void transferSpi(quint8 txBuf[],quint8 rxBuf[],int len)
// {
//     g_SpiDev->transferSpiData(txBuf,rxBuf,len);
// }

#endif // WIDGET_H

#ifndef CSPIDEV_H
#define CSPIDEV_H

#include <QObject>


class CSpiDev
{

public:
    static CSpiDev* getInstance();
    virtual ~CSpiDev();

private:
    CSpiDev();
    static CSpiDev* m_pThis;

public:
    int openDevSpi();
    int closeDevSpi();
    int transferSpiData(const quint8 *txBuf, quint8 *rxBuf, int len);
    int writeSpiData(quint8 *txBuf,int len);
    int readSpiData(quint8 rxBuf[], unsigned int len);
    int testSpiWR();
    int changSpeed(quint32 speed_set);
     quint32 speed;

private:
    quint8 mode;
    quint8 bits;

    quint16 delay;
    const char *device;
    int fd;
    int tempFd;
};

#define g_SpiDev (CSpiDev::getInstance())  //使得外部可以调用该c文件函数及变量

#endif // CSPIDEV_H

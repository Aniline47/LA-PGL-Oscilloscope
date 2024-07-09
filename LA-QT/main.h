#ifndef MAIN_H
#define MAIN_H

#include "widget.h"
#include <QWidget>
#include <QApplication>
#include "CSpiDev.h"
#include <QObject>

void openSpi()
{
    g_SpiDev->openDevSpi();
}
void testSpi()
{
    g_SpiDev->testSpiWR();
}
void writeSpi(quint8 txBuf[], int len)
{
    g_SpiDev->writeSpiData(txBuf,len);
}
void readSpi(quint8 rxBuf[],unsigned int len)
{
    g_SpiDev->readSpiData(rxBuf,len);
}
void transferSpi(quint8 txBuf[],quint8 rxBuf[],int len)
{
    g_SpiDev->transferSpiData(txBuf,rxBuf,len);
}
#endif // MAIN_H

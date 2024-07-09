#include "CSpiDev.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <QDebug>

/*
*@tx_buf:写数据缓冲区的指针 或为null
*@rx_buf：接收数据缓冲区指针，或为null
*@len：读或写缓冲区长度
*@speed_hz：通信设备比特率
*@bits_per_word：设备的字长
*@delay_usecs：如果非零，本次传输最后一位完成，下次传输前取消设备的延时。
*@cs_change：确认在下次传输之前取消设备（spi 片选）
*/


CSpiDev* CSpiDev::m_pThis = NULL;
CSpiDev::CSpiDev()
{
    //SPI通信使用全双工，设置CPOL＝0，CPHA＝0
    mode = 0;
    //8bits读写，MSB first
    bits = 8;
    // 设置12M传输速度
    speed = 30*1000*1000;
    delay = 0;
    device = "/dev/spidev0.1";
    fd = 0;
    tempFd = 0;
}

CSpiDev::~CSpiDev()
{

}


CSpiDev *CSpiDev::getInstance()
{
    if (!m_pThis)
    {
        m_pThis =  new CSpiDev();
    }
    return m_pThis;
}



// ---------------------------------------------------------------- //
// 函数名称: openDevSpi
// 函数说明: 打开并初始化SPI
// 输入/输出参数: 无
// 返 回 值: 0 表示已打开  0XF1 表示SPI已打开 其它出错
// ----------------------------------------------------------------//
int CSpiDev::openDevSpi()
{
    int ret = 0;
    if (tempFd != 0) /* 设备已打开 */
    {
        qDebug() << "device has already open!";
        return 0xF1;

    }
    fd = open(device, O_RDWR);
    if (fd < 0)
    {
        qDebug() << "Can't open device!";
    }
    else
    {
        qDebug() << "SPI - Open Succeed. Start Init SPI...";
    }
    tempFd = fd;
    /*
     * spi mode
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    {
        qDebug() <<"can't set spi mode";
    }
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
    {
        qDebug() <<"can't set spi mode";
    }
    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        qDebug() << "can't set bits per word";
    }
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        qDebug() << "can't get bits per word";
    }

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        qDebug() << "can't set max speed hz";
    }

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        qDebug() << "can't get max speed hz";
    }
    qDebug() << "max speed hz = "<< speed <<"\n";
    return ret;
}

// ---------------------------------------------------------------- //
// 函数名称: closeDevSpi
// 函数说明: 关闭SPI
// 输入/输出参数: 无
// 返 回 值: 无
// ----------------------------------------------------------------//
int CSpiDev::closeDevSpi()
{
    fd = tempFd;
    if (fd == 0) /* SPI是否已经打开*/
    {
        return 0;
    }
    close(fd);
    tempFd = 0;

    return 0;
}

int CSpiDev::changSpeed(quint32 speed_set)
{

    speed = speed_set;
    return 0;
}


// ---------------------------------------------------------------- //
// 函数名称: transferSpiData
// 函数说明: 同步数据传输
// 输入/输出参数:     txBuf[In]:发送数据首地址
//                  rxBuf[In]:接收数据缓冲区
//                  len[In]:发送长度
// 返 回 值: 0 成功
// ----------------------------------------------------------------//
int CSpiDev::transferSpiData(const quint8 *txBuf, quint8 *rxBuf, int len)
{
    int ret;
    fd = tempFd;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)txBuf,
        .rx_buf = (unsigned long)rxBuf,
        .len = (uint32_t)len,
        .speed_hz = speed,
        .delay_usecs = delay,
        .bits_per_word = bits,
    };
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        //qDebug() << "can't send spi message";
    }
    return ret;
}


// ---------------------------------------------------------------- //
// 函数名称: writeSpiData
// 函数说明: 发送数据
// 输入/输出参数: txBuf[In]:发送数据首地址
//              len[In]:发送长度
// 返 回 值: 0 成功
// ----------------------------------------------------------------//
/*
int CSpiDev::writeSpiData(quint8 *txBuf, int len)
{
    int ret;
    fd = tempFd;
    ret = write(fd, txBuf, len);
    if (ret < 0)
    {
        qDebug() << "SPI Write error!";
    }
    else
    {
        qDebug() << "SPI Write May Successful";
    }
    return ret;
}
*/


int CSpiDev::writeSpiData(quint8 *txBuf, int len)
{
    int ret;
    fd = tempFd;
    //ret = write(fd, txBuf, len);
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)txBuf,
        .rx_buf = (unsigned long)0,
        .len = (uint32_t)len,
        .speed_hz = speed,
        .delay_usecs = delay,
        .bits_per_word = bits,
    };
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret > 1)
    {
         qDebug() << "SPI Write May Successful";
    }
    else
    {
        qDebug() << "SPI Write error!";
    }
    return ret;
}


// ---------------------------------------------------------------- //
// 函数名称: readSpiData
// 函数说明: 接收数据
// 输入/输出参数: rxBuf[In]:接收数据缓冲区
//              len[In]:接收到的长度
// 返 回 值: >=0 成功
// ----------------------------------------------------------------//

int CSpiDev::readSpiData(quint8 rxBuf[], unsigned int len)
{
    int ret;
    fd = tempFd;
    //quint8 txData[len] = {0};
    unsigned long long txData = NULL;
    struct spi_ioc_transfer tr = {
        //.tx_buf = (unsigned long long)(txData),
        //.rx_buf = (unsigned long long)rxBuf,
        .tx_buf = (txData),
        .rx_buf = (unsigned long long)(rxBuf),
        .len = len,
        .speed_hz = speed,
        .delay_usecs = delay,
        .bits_per_word = bits
    };
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret == -1)
    {
         qDebug() << "SPI Read error!";
    }
    return ret;
}


/*
int CSpiDev::readSpiData(quint8 *rxBuf, int len)
{
    int ret;
    fd = tempFd;

    ret = read(fd, rxBuf, len);
    if (ret < 0)
    {
        qDebug() << "SPI Read error!";
    }
    else
    {
        qDebug() << "SPI Read May Successful";
    }
    return ret;
}
*/

// ---------------------------------------------------------------- //
// 函数名称: testSpiWR
// 函数说明: 自发自收测试程序,在硬件上需要把输入与输出引脚短接
// 输入/输出参数: 无
// 返 回 值: 0 成功
// ----------------------------------------------------------------//
int CSpiDev::testSpiWR()
{
    int ret, i;
    const int BufSize = 16;
    quint8                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       tx[BufSize], rx[BufSize];

    //bzero(rx, sizeof(rx));
    memset(rx, '\0',sizeof(rx));
    for (i = 0; i < BufSize; i++)
    {
        tx[i] = i;
    }

    ret = transferSpiData(tx, rx, BufSize);
    if (ret > 1)
    {
        ret = memcmp(tx, rx, BufSize);
        if (ret != 0)
        {
            qDebug() << "LookBack Mode Test error";
        }
        else
        {
            qDebug() << "SPI - LookBack Mode  OK";
        }
    }
    return ret;
}



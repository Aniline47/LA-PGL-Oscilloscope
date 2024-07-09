#include "widget.h"
#include "qcustomplot.h"

#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // QCustomPlot customPlot;
    // customPlot.setBackground(QBrush(QColor(0, 0, 0))); // 设置为白色
    // customPlot.show();

    Widget w;
    w.show();
    return a.exec();

}

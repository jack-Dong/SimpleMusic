#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //解决中文乱码问题
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    Widget w;
    w.show();

    return a.exec();
}

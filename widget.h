#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTextCodec>
#include <QDebug>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QDir>
#include <QFileDialog>
#include <cassert>
#include <string>
#include <QBitArray>
#include <QProcess>
#include "mysqldb.h"
#include <QScrollArea>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWindowControl>
#include <QVideoWidget>
#include <QTime>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QImage>
#include <QRadioButton>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QMediaPlayer *player;
    QMediaPlaylist *playList;
    QVideoWidget * videoWidget;
    QPushButton *fullButton;
    //向歌单里面插入歌曲
    bool musicListAddMusic(QString,QString,QString);

public slots:
    void updateInfo();
    //进度条
    bool durationUpdate(qint64);
    bool positionUpdate(qint64);
    bool resetPositionUpdate();
    bool VolumeValueUpdate(int);
    //全屏
    bool fullScreenUp(bool);
    //选择歌曲
    bool chooseMusic(bool);
    //切换到点击的歌曲
    bool musicListItermClick();
protected:
    //窗口拖拽
    bool isDrag;
    QPoint m_position;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    //窗口重绘
    void paintEvent(QPaintEvent*);
private slots:
    void on_manage_clicked();

    void on_home_clicked();

    void on_search_clicked();

    void on_setting_clicked();

    void on_video_clicked();

    void on_addMusic_clicked();

    //更新manageWiget的列表
    bool manageListUpdate();

    //处理海报点击事件
    bool postClick();

    void on_pre_clicked();

    void on_next_clicked();

    void on_pause_toggled(bool checked);

    void on_fullScreen_clicked();

    void on_musicsearch_clicked();

private:
    MysqlDB *db;
    Ui::Widget *ui;
};

#endif // WIDGET_H

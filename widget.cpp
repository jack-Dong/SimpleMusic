#include "widget.h"
#include "ui_widget.h"
#include <QSlider>
#include <QSizePolicy>
#include <QEvent>
#include <QMouseEvent>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //去掉难看的标题栏
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);
   // setAttribute(Qt::WA_TranslucentBackground); 写上就没有视频了
    db = new MysqlDB();
    this->on_home_clicked();
    //添加播放器

    player = new QMediaPlayer(this);
    playList = new QMediaPlaylist(this);
    playList->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playList);

   videoWidget = new QVideoWidget(ui->videowidgepage);
   videoWidget->move(QPoint(10,0));
   videoWidget->setFixedSize(690,450);
   //双击关闭全屏
   fullButton = new QPushButton(videoWidget);
   fullButton->setMinimumSize(videoWidget->size());
   fullButton->setFlat(true);
   fullButton->setCheckable(true);
//   fullButton->setStyleSheet("color: rgb(100, 100, 100);");
   connect(fullButton,SIGNAL(toggled(bool)),this,SLOT(fullScreenUp(bool)));
   videoWidget->show();

    //下面这样写会崩
//    QGraphicsView *graphicsView = new QGraphicsView(ui->videowidgepage);
//    QGraphicsView *graphicsView = new QGraphicsView();
//    QGraphicsScene *scene = new QGraphicsScene();
//    graphicsView->setScene(scene);
//    videoIterm = new QGraphicsVideoItem();
//    videoIterm->setSize(QSize(400,400));
//    graphicsView->scene()->addItem(videoIterm);
//    graphicsView->show();
    player->setVideoOutput(videoWidget);

    //歌曲信息更新
    connect(player,SIGNAL(metaDataAvailableChanged(bool)),this,SLOT(updateInfo()));
    //进度条总长度更新
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationUpdate(qint64)));
    //进度条位置更新
    connect(player,SIGNAL( positionChanged(qint64) ),this,SLOT(positionUpdate(qint64)));
    //快进
    connect(ui->process,SIGNAL(valueChanged(int)),this,SLOT(resetPositionUpdate()));
    //音量更新 好像没什么用
    connect(player,SIGNAL(volumeChanged(int) ),this,SLOT(VolumeValueUpdate(int)));
    //设置音量
    connect(ui->volume,SIGNAL(valueChanged(int)),player,SLOT(setVolume(int)));
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::musicListAddMusic(QString name, QString id,QString imgPath)
{
    //增加最小高度
    int num = ui->rightWidght->children().size();
    QSize s = ui->rightWidght->minimumSize();
    s.setHeight( (num+1) * 80);

    ui->rightWidght->setMinimumSize(s);
    //加入图片和信息
    QWidget *iterm = new QWidget(ui->rightWidght);
    iterm->setMinimumSize(QSize(150,80));

    QPushButton *p = new QPushButton(iterm);
    p->setFlat(true);
    p->setIcon(QIcon(imgPath));
    p->setIconSize(QSize(80,80));

    //给加上点击事件
    connect(p,SIGNAL(clicked(bool)),this,SLOT(musicListItermClick()));

    QLabel *l = new QLabel(iterm);
    l->setFixedSize(QSize(70,40));
    l->move(82,20);
    l->setText(name);
    l->setAlignment(Qt::AlignVCenter);
    l->setStyleSheet("font: 7pt '微软雅黑';");

    QLabel *idLabel = new QLabel(iterm);
    idLabel->setText(id);
    idLabel->hide();
    //布局
    iterm->move(0,65*num);
    iterm->show();

    return true;
}

bool Widget::durationUpdate(qint64 range)
{
    ui->process->setRange(0,range);
    return true;
}

bool Widget::positionUpdate(qint64 position)
{
    ui->process->setValue(position);
    QString time = QTime::fromMSecsSinceStartOfDay(position).toString("hh:mm:ss");
    ui->time->setText(time);
    return true;
}

bool Widget::resetPositionUpdate()
{
    player->setPosition(ui->process->value());
    return 0;
}

bool Widget::VolumeValueUpdate(int v)
{
    qDebug() << "音量"<<v<<endl;
    ui->volume->setValue(v);
    return true;
}

bool Widget::fullScreenUp(bool ck)
{
    if(ck == true)
    {
        videoWidget->setFullScreen(true);
        fullButton->setFixedSize(videoWidget->size());
    }else
    {
        videoWidget->setFullScreen(false);
        fullButton->setFixedSize(videoWidget->size());
    }
//    qDebug() <<"ck= "<<ck<<endl;
    return true;
}

bool Widget::chooseMusic(bool)
{
    QObject *sd = sender();
    QObject *p = sd->parent();
    QObjectList list = p->children();
    QLabel * label = (QLabel *)list[0];
    qDebug() <<label->text();
    //拿到id
    QString id = label->text();
    //查询歌曲的信息
    QVector<QStringList> rt = db->serachById(id);
//    qDebug() <<rt<<endl;
    //添加到播放列表
    playList->addMedia(QUrl::fromLocalFile(rt[0][3]));
    this->musicListAddMusic(rt[0][1],rt[0][0],rt[0][4]);
    return true;
}

bool Widget::musicListItermClick()
{
    //拿到自己在父节点中的index
    int index = -1;
    QObject * f = sender()->parent();
    QObject * g = f->parent();
    QObjectList list = g->children();
    while(list[++index] != f) { }

    if( index != -1)
    {
        playList->setCurrentIndex(index);
    }

    return true;
}


void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(),QColor(255,255,255,255));
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        isDrag = true;
        m_position = e->globalPos() - this->pos();
        e->accept();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if(isDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - m_position);
        e->accept();
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    isDrag = false;
}

//左边页框的跳转
void Widget::on_home_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    //先删除所有子控件
    QObjectList list =  ui->homeScrollAreaWidgetContents->children();
//    qDebug() <<list.length() ;
    for(int i = 0; i < list.length();i++)
    {
       ((QWidget *)list[i])->close();
    }
    //拿到MV信息
    QVector<QStringList> rt = db->serachAll();
    //向页面中填充海报
    //id name singer path imgPath spell 点击直接加入歌单开始播放

    for(int i = 0; i < rt.size();i++)
    {
        QWidget *iterm = new QWidget(ui->homeScrollAreaWidgetContents);
        iterm->setMinimumSize(QSize(150,170));
        iterm->setStyleSheet("background-color: rgb(176, 200, 178);");

        QLabel *la = new QLabel(iterm);
        la->setFixedSize(QSize(150,20));
        la->setText(rt[i][0]);
        la->setAlignment(Qt::AlignCenter);
        la->setStyleSheet("color: rgb(255, 255, 255);font: 8pt '微软雅黑';");

        QPushButton *p = new QPushButton(iterm);
        p->move(0,20);
        p->setFlat(true);
        p->setIcon(QIcon(rt[i][4]));
        p->setIconSize(QSize(150,130));
        //给加上点击事件
        connect(p,SIGNAL(clicked(bool)),this,SLOT(postClick()));

        QLabel *l = new QLabel(iterm);
        l->setFixedSize(QSize(150,40));
        l->move(0,130);
        l->setText(rt[i][1] + rt[i][2]);
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet("background-color: rgb(200, 200, 200);font: 8pt '微软雅黑';");

        //对iterm进行布局 宽670

        int row = i/4;
        int col =i%4;

        iterm->move(col*160 + 20,row*180);

        //每次循环完要更改Content的大小
        QSize s = ui->homeScrollAreaWidgetContents->minimumSize();
        s.setHeight((row+1) * 180);
        ui->homeScrollAreaWidgetContents->setMinimumSize(s);

        iterm->show();
    }
}

void Widget::on_search_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_manage_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    this->manageListUpdate();
}

void Widget::on_setting_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Widget::on_video_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

//添加歌曲
void Widget::on_addMusic_clicked()
{
    //拿到文件路径
    QString initialName = QDir::homePath();
    QString AllPath = QFileDialog::getOpenFileName(this, tr("选择文件"), initialName, tr("*mp4 *wmv"));
    //解析文件路径 所有歌曲的文件名称命名例子："C:/迅雷下载/香港/香港男组合/Bliss/Bliss-情人借借（MV）.wmv"
    QString path= "";
    QString singer= "";
    QString name = "";
    QString form = "";
    QString nameAndSingerAndForm = "";
    QString nameAndSinger = "";
    int i = -1;
    if( ( i = AllPath.lastIndexOf("/") ) != -1 )
    {
        path = AllPath.left(i);
        nameAndSingerAndForm = AllPath.right(AllPath.length()- i - 1);
    }else
    {
        nameAndSingerAndForm = AllPath;
    }


    int formBgein = nameAndSingerAndForm.indexOf(".");
    assert(formBgein != -1);
    form = nameAndSingerAndForm.right(nameAndSingerAndForm.length() - formBgein -1);
    nameAndSinger = nameAndSingerAndForm.left(formBgein);
    int nameBegin = nameAndSinger.indexOf("-");
    assert(nameBegin != -1);
    name = nameAndSinger.right(nameAndSinger.length() - nameBegin -1);
    singer = nameAndSinger.left(nameBegin);

//    qDebug() <<path<<endl;
//    qDebug() <<nameAndSingerAndForm<<endl;
//    qDebug() <<nameAndSinger<<endl;
//    qDebug() <<singer<<endl;
//    qDebug() <<name<<endl;
//    qDebug() <<form<<endl;

    //移动文件到曲库
    QString storagePath = ui->pathlineEdit->text() + "/"+nameAndSingerAndForm;
//    qDebug() << storagePath<<endl;
    QFile::copy(AllPath,storagePath);
    //生成视频的海报
//    QString cmd="VideoGetPosterImage.exe ";
//    cmd.append(storagePath);
//    cmd.append(" >>a.txt");
//    QByteArray b = cmd.toLocal8Bit();
//    system(b.data());
//    qDebug() << cmd <<endl;
    QProcess p(0);
    p.start("VideoGetPosterImage.exe", QStringList()<<storagePath);
    p.waitForStarted();
    p.waitForFinished();
//    qDebug()<<QString::fromLocal8Bit(p.readAllStandardOutput());
    //把文件的信息存到数据库 先把所有关于数据的操作封装成一个类
    db->insert(name,singer,storagePath,ui->pathlineEdit->text()+"/"+nameAndSinger+".jpg","");
    //更新所有文件列表
    this->manageListUpdate();
}

bool Widget::manageListUpdate()
{
    //拿到数据
    QVector<QStringList> rt = db->serachAll();
    //清除所有项目
    QObjectList list =  ui->scrollAreaWidgetContents->children();
//    qDebug() <<list.length() ;
    for(int i = 0; i < list.length();i++)
    {
//       delete ((QWidget *)list[i]); 这样xie写会蹦
       ((QWidget *)list[i])->close();
    }
    //初始化这个大小
    ui->scrollAreaWidgetContents->setMinimumSize(QSize(640,10));
    //放到页面上去
    //id name singer 删除按钮 //先不做删除
    for(int i = 0; i < rt.size();i++)
    {
        QWidget *iterm = new QWidget(ui->scrollAreaWidgetContents);
//        iterm->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
//        iterm->setMinimumSize(640,30);
        QLabel *idLabel = new QLabel(iterm);
        QLabel *nameLabel = new QLabel(iterm);
        QLabel *singerLabel = new QLabel(iterm);
        idLabel->setText(rt[i][0]);
        nameLabel->setText(rt[i][1]);
        singerLabel->setText(rt[i][2]);
        idLabel->move(QPoint(10,10));
        nameLabel->move(QPoint(110,10));
        singerLabel->move(QPoint(310,10));
        iterm->setStyleSheet("height:30px;font: 8pt '微软雅黑';");
        iterm->move(QPoint(5,10+i*30));
        //每次循环完要更改Content的大小
        QSize s = ui->scrollAreaWidgetContents->minimumSize();
        s.setHeight(s.height() + 30);
        ui->scrollAreaWidgetContents->setMinimumSize(s);
        iterm->show();
    }

    return true;
}

bool Widget::postClick()
{
    QObject *sd = sender();
    QObject *p = sd->parent();
    QObjectList list = p->children();
    QLabel * label = (QLabel *)list[0];
    qDebug() <<label->text();
    //拿到id
    QString id = label->text();
    //查询歌曲的信息
    QVector<QStringList> rt = db->serachById(id);
//    qDebug() <<rt[0][0]<<endl;
//    qDebug() <<rt[0][1]<<endl;
//    qDebug() <<rt[0][2]<<endl;
//    qDebug() <<rt[0][3]<<endl;
    //添加到播放列表
    playList->addMedia(QUrl::fromLocalFile(rt[0][3]));
    this->musicListAddMusic(rt[0][1],rt[0][0],rt[0][4]);
    //更改当前歌曲为最后新加的这首歌
    playList->setCurrentIndex(ui->rightWidght->children().size() - 1 );
    //开始播放
    this->on_video_clicked();
    player->play();
    this->on_pause_toggled(true);
    return true;
}

void Widget::updateInfo()
{
    //设置时间
    int stime = player->metaData("Duration").toInt();
    QString time = QTime::fromMSecsSinceStartOfDay(stime).toString("hh:mm:ss");
    ui->length->setText(time);
    //更改海报歌曲信息等
    int index = playList->currentIndex();
    QObjectList list1 = ui->rightWidght->children();
    QObject *iterm = list1[index];
    QObjectList list2 = iterm->children();
    QLabel *label = (QLabel *)list2[2];
    QString id = label->text();
//    qDebug()<<"id"<<id<<endl;
    QVector<QStringList> rt = db->serachById(id);
    //id name singer path imgPath
    QImage img1(rt[0][4]);
    QImage img2 = img1.scaled(QSize(40,40));
    ui->photo->setPixmap(QPixmap::fromImage(img2));
    ui->photo->setStyleSheet("border-radius:20px;");
    ui->name->setText(rt[0][1]);
    ui->singer->setText(rt[0][2]);

}

void Widget::on_pre_clicked()
{
    int currentIndex = playList->currentIndex();
    if(--currentIndex < 0 ) currentIndex = 0;
    playList->setCurrentIndex(currentIndex);
    player->play();
    this->on_pause_toggled(true);
}

void Widget::on_next_clicked()
{
    int currentIndex=playList->currentIndex();
    if(++currentIndex == playList->mediaCount()) currentIndex=0;
    playList->setCurrentIndex(currentIndex);
    player->play();
    this->on_pause_toggled(true);
}

void Widget::on_pause_toggled(bool checked)
{
    ui->stackedWidget->setCurrentIndex(4);
    if(checked == true)
    {
        player->play();
        ui->pause->setIcon(QIcon(":/img/pause.png"));
        ui->pause->setIconSize(QSize(22,22));
    }else
    {
        player->pause();
        ui->pause->setIcon(QIcon(":/img/play.png"));
        ui->pause->setIconSize(QSize(21,21));
    }
}

void Widget::on_fullScreen_clicked()
{
    //怎么会有bug
    this->fullScreenUp(true);
}

void Widget::on_musicsearch_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    //判断是按名字还是歌手搜索
    QVector<QStringList> rt;
    if(ui->nameRadioButton->isChecked() == true)
    {
        rt = db->serachByName(ui->searchInput->text());
    }else if(ui->singerRadioButton->isCheckable() == true)
    {
        rt = db->serachBySinger(ui->searchInput->text());
    }
    int num = rt.size();
//    qDebug() <<rt<<endl;
    //清除原来页面内容
    QObjectList list =  ui->searchContents->children();
    for(int i = 0; i < list.length();i++)
    {
       ((QWidget *)list[i])->close();
    }
    //放入新的页面内容
    //id name singer 添加按钮
    for(int i = 0; i < rt.size();i++)
    {
        QWidget *iterm = new QWidget(ui->searchContents);
//        iterm->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
//        iterm->setMinimumSize(640,30);
        QLabel *idLabel = new QLabel(iterm);
        QLabel *nameLabel = new QLabel(iterm);
        QLabel *singerLabel = new QLabel(iterm);
        QPushButton *addButton = new QPushButton(iterm);

        idLabel->setText(rt[i][0]);
        nameLabel->setText(rt[i][1]);
        singerLabel->setText(rt[i][2]);
        addButton->setText("添加");
        addButton->setFlat(true);
        addButton->setFixedSize(QSize(40,20));
        addButton->setStyleSheet("font: 8pt '微软雅黑';border-radius:3px;background-color: rgb(132, 132, 132);");

        //为addButto 加上点击事件
        connect(addButton,SIGNAL(clicked(bool)),this,SLOT(chooseMusic(bool)));
        idLabel->move(QPoint(10,0));
        nameLabel->move(QPoint(110,0));
        singerLabel->move(QPoint(260,0));
        addButton->move(QPoint(360,0));

        iterm->setStyleSheet("height:30px;font: 8pt '微软雅黑';");
        iterm->move(QPoint(5,10+i*30));
        //每次循环完要更改Content的大小
        QSize s = ui->searchContents->minimumSize();
        s.setHeight(num*30);
        ui->searchContents->setMinimumSize(s);
        iterm->show();
    }
}

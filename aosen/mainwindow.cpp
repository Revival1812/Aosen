#include "mainwindow.h"
#include "ImageWindow.h"
#include "Spot.h"
#include "shortestpath.h"
#include <QVBoxLayout>
#include <QImage>
#include <QMessageBox>
#include <QDesktopServices>
#include <QCloseEvent>

void MainWindow::onPushBtnDisplay()
{
    ImageWindow *existingWindow = findChild<ImageWindow*>();
    if (existingWindow) {
        existingWindow->activateWindow();
        return;
    }
    //地图显示
    QString imagePath = ":/bkg/img/transportation.png";
    ImageWindow *window = new ImageWindow(imagePath,this);

    window->setAttribute(Qt::WA_DeleteOnClose);
    QPoint globalPos = this->mapToGlobal(QPoint(0,0));//父窗口绝对坐标
    int x = globalPos.x() - (this->width() - window->width()) / 2;
    int y = globalPos.y();
    window->move(x, y);
    window->show();
}

void MainWindow::onPushBtnSearch()
{
    Spot *existingSpot = findChild<Spot*>();
    if(existingSpot){
        existingSpot->activateWindow();
        return;
    }
    Spot *spot = new Spot(this);
    spot->setAttribute(Qt::WA_DeleteOnClose);
    spot->show();
}

void MainWindow::onPushBtnShortestPath()
{
    ShortestPath *existingshortestPath = findChild<ShortestPath*>();
    if(existingshortestPath){
        existingshortestPath->activateWindow();
        return;
    }
    //最短路径
    ShortestPath *shortestPath = new ShortestPath(this);
    shortestPath->setAttribute(Qt::WA_DeleteOnClose);
    shortestPath->show();
}

void MainWindow::onPushBtnExit()
{
    close();// 关闭所有窗口
}

void MainWindow::initSignalSlots()
{
    connect(display_map,SIGNAL(clicked()), this, SLOT(onPushBtnDisplay()));
    connect(search_landmark,SIGNAL(clicked()), this, SLOT(onPushBtnSearch()));
    connect(shortest_path_planning,SIGNAL(clicked()), this, SLOT(onPushBtnShortestPath()));
    connect(exit,SIGNAL(clicked()), this, SLOT(onPushBtnExit()));
    connect(linkLabel, &QLabel::linkActivated, [=](QString url){
        QDesktopServices::openUrl(QUrl(url));
    });
}

void MainWindow::initUi() {
    center_widget = new QWidget;

    display_map = new QPushButton("显示地图");
    display_map->setFixedSize(200, 50);

    search_landmark = new QPushButton("搜索地标");
    search_landmark->setFixedSize(200, 50);

    shortest_path_planning = new QPushButton("最短路径规划");
    shortest_path_planning->setFixedSize(200, 50);

    exit = new QPushButton("退出");
    exit->setFixedSize(200, 50);

    linkLabel = new QLabel;

    linkLabel->setText("<a style='color: blue; text-decoration: none' href = https://ditu.amap.com/search?query=%E5%8C%97%E4%BA%AC%E5%B8%82&city=110101&geoobj=116.132589%7C39.686589%7C117.000809%7C40.151027&zoom=10.59>点我试试");
    linkLabel->setAlignment(Qt::AlignCenter);//文字居中

    QVBoxLayout *VLayout = new QVBoxLayout;

    // 在顶部和底部添加拉伸项，使按钮集中在中间
    VLayout->addStretch(); // 顶部拉伸
    VLayout->addWidget(display_map, 0, Qt::AlignHCenter);
    VLayout->addWidget(search_landmark, 0, Qt::AlignHCenter);
    VLayout->addWidget(shortest_path_planning, 0, Qt::AlignHCenter);
    VLayout->addWidget(exit, 0, Qt::AlignHCenter);
    VLayout->addWidget(linkLabel);
    VLayout->addStretch(); // 底部拉伸


    VLayout->setContentsMargins(0, 0, 0, 0);   // 移除布局的外边距
    VLayout->setSpacing(5);                    // 进一步减小按钮间距至5像素

    center_widget->setLayout(VLayout);
    setCentralWidget(center_widget);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    initUi();
    setFixedSize(600, 600);
    initSignalSlots();
    this->setStyleSheet(
        "QMainWindow {"
        " background-image:url(:/bkg/img/bkg.jpg);"
        " background-position: center;"
        " background-repeat: no-repeat;"
        "}"
        );
}

MainWindow::~MainWindow() {}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#define SIZE 11
#define SPOTSIZE 8
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QString spots[SIZE]={"中华民族博物院","国家奥林匹克体育中心","水立方","鸟巢","中国共产党历史展览馆","科技馆",
                           "奥森南园","奥森北园","奥体中心地铁站","奥林匹克公园地铁站",
                           "森林公园南门地铁站"};
    double distance[SIZE][SIZE] = {
        {0,0.55, 2.0, 1.6, 3.2, 3.5, 3.5, 5.3, 0.55, -1, -1},
        {0.55, 0, 2.4, 2.0, 2.6, 3.1, 3.9, 4.8, 1.6, -1, -1},
        {2.0, 2.4, 0, 0.62, 1.8, 2.1, 2.1, 3.9, -1, 1.1, -1},
        {1.6, 2.0, 0.62, 0, 2.2, 2.5, 2.7, 4.4, 1, -1, -1},
        {3.2, 2.6, 1.8, 2.2, 0, 0.4, 1.3, 2.1, -1, 0.72, -1},
        {3.5, 3.1, 2.1, 2.5, 0.4, 0, 0.87, 1.9, -1, 0.9, -1},
        {3.5, 3.9, 2.1, 2.7, 1.3, 0.87, 0, 2.2, -1, -1, 0.1},
        {5.3, 4.8, 3.9, 4.4, 2.1, 1.9, 2.2, 0, -1, -1, 2.2},
        {0.55, 1.6, -1, -1, -1, -1, -1, -1, 0, -1, -1},
        {-1, -1, -1, -1, 0.72, 0.9, -1, -1, -1, 0, -1},
        {-1, -1, -1, -1, -1, -1, 0.1, 2.2, -1, -1, 0}
    };
private:
    QWidget *center_widget;
    QPushButton *display_map;
    QPushButton  *search_landmark;
    QPushButton *shortest_path_planning;
    QPushButton *exit;
    QLabel *linkLabel;
    QString filename = ":/bkg/img/bkg.jpg";
public:
    void initUi();
public slots:
    void onPushBtnDisplay();
    void onPushBtnSearch();
    void onPushBtnShortestPath();
    void onPushBtnExit();
public:
    void initSignalSlots();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H

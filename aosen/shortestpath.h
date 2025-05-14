#ifndef SHORTESTPATH_H
#define SHORTESTPATH_H

#include "mainwindow.h"
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
class ShortestPath:public MainWindow{
    Q_OBJECT
private:
    QWidget *centerWidget;
    QPushButton *pushBtn;
    QLineEdit *start;
    QLineEdit *end;
    QTextEdit *path;
    QListWidget *startSuggest;
    QListWidget *endSuggest;
    QStringList stationSuggestions;
    QStringList spotSuggestions;
public:
    void onInputError(int i);
    void init();
    QString DijkstraSpots(QString startText,QString endText);
    QString TSP(QString startText);
    void showStartSuggestion(QString text);
    void showEndSuggestion(QString text);
    void chooseStartSuggestion(QListWidgetItem *item);
    void chooseEndSuggestion(QListWidgetItem *item);
    int getSpotId(QString spot);
    int getStationId(QString station);
    int findnearestSpot(int stationId);
    int findnearestStation(int spotId);
    void onClickBtn();

public slots:
    void initSignalSlots();
public:
    explicit ShortestPath(QWidget *parent = nullptr);
};

#endif // SHORTESTPATH_H

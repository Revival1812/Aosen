#ifndef SPOT_H
#define SPOT_H

#include <qstring.h>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QAction>
#include <QListView>
#include <QListWidget>
#include "mainwindow.h"
class Spot: public MainWindow {
    Q_OBJECT
private:
    QWidget *centerWidget;
    QPushButton *pushBtn;
    QLineEdit *searchEdit;
    QAction *searchAction;
    QTextEdit *showText;
    QListWidget *suggestionList;
    QStringList suggestions;
public:
    void onInputError();
    void init();
    void onClickBtn();
    void showSuggestion(QString text);
    void chooseSuggestion(QListWidgetItem *item);
    QString listSpotsByDistance(QString text);
public slots:
    void initSignalSlots();

public:
    explicit Spot(QWidget *parent = nullptr);
};

#endif // SPOT_H

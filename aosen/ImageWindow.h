#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVector>
struct SpotScene {
    QString name;
    QString description;
    QString detail;
    QPoint position;
};

class ImageWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ImageWindow(const QString &imagePath, QWidget *parent = nullptr);

private slots:
    void showSpotDetail(); // 点击景点显示详细信息

private:
    QLabel *imageLabel;
    QVector<QPushButton*> spotButtons;
    QVector<SpotScene> spots;
};

#endif // IMAGEWINDOW_H

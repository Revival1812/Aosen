#ifndef SPOTDETAILDIALOG_H
#define SPOTDETAILDIALOG_H

#include <QDialog>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit> // 新增
#include <QString>

#include <QGroupBox>
#include <QVBoxLayout>

struct Place {
    QString name;
    QString type;
    double x;
    double y;
};

struct PlaceCloud {
    std::vector<Place> pts;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    // Returns the dim'th component of the idx'th point in the class
    inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
        if (dim == 0) return pts[idx].x;
        else return pts[idx].y;
    }

    // Optional bounding-box computation
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
};

class SpotDetailDialog : public QDialog {
    Q_OBJECT
public:
    explicit SpotDetailDialog(const QString &title,
                              const QString &description,
                              const QString &detailedDescription,
                              const QVector<QString> &imagePaths,
                              const QPoint &mainposition,
                              QWidget *parent = nullptr);

private slots:
    void showPreviousImage();
    void showNextImage();
    void onSearchClicked(); // 搜索按钮的槽函数

private:
    QVector<QString> imagePaths;
    QPoint userposition;
    int currentIndex = 0;

    QLabel *imageLabel;
    QLabel *descriptionLabel;
    QLabel *detailedDescriptionLabel;

    QLineEdit *searchLineEdit;   // 搜索框
    QPushButton *searchButton;   // 搜索按钮

    QGroupBox* recentGroupBox;
    QVBoxLayout* recentLayout;

    QPushButton *prevButton;
    QPushButton *nextButton;

    QString detailedDescription;

    void updateImage();

    void displayPlaces(const QVector<Place>& places);
    void displayTypeClustersOnMap(const QMap<QString, QVector<Place>> &typeGroups);

    // 预留接口
    void handleKeywordSearch(const QString &keyword,QVector<Place> &resultContainer);
    void handleDistanceSearch(double distance);
    void findNearestPlaces(const QPointF &target, const QVector<Place> &input, QVector<Place> &output, int K);
};

#endif // SPOTDETAILDIALOG_H

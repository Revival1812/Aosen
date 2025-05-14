#include "SpotDetailDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QMessageBox>
#include <QSqlQuery>
#include "nanoflann.h"
#include <QVector>
#include <QGroupBox>
#include <random>
#include <QPainter>

std::random_device rd;  // 真随机数种子
std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎

SpotDetailDialog::SpotDetailDialog(const QString &title,
                                   const QString &description,
                                   const QString &detailedDescription,
                                   const QVector<QString> &imagePaths,
                                   const QPoint &mainposition,
                                   QWidget *parent)
    : QDialog(parent), imagePaths(imagePaths), userposition(mainposition), detailedDescription(detailedDescription)
{
    setWindowTitle(title);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 新增：展示最近地点的小框 =====
    recentGroupBox = new QGroupBox("最近的地点", this);
    recentLayout = new QVBoxLayout(recentGroupBox);
    recentGroupBox->setLayout(recentLayout);
    recentGroupBox->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #2c3e50; border: 1px solid #cccccc; border-radius: 6px; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; top: 2px; }"
        );
    mainLayout->addWidget(recentGroupBox);
    // ====================================

    // ===== 搜索框和按钮 =====
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("请输入关键词或距离");
    searchButton = new QPushButton("搜索", this);
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(searchButton);
    mainLayout->addLayout(searchLayout);
    connect(searchButton, &QPushButton::clicked, this, &SpotDetailDialog::onSearchClicked);
    // ==============================

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(imageLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    prevButton = new QPushButton("上一张", this);
    nextButton = new QPushButton("下一张", this);
    buttonLayout->addWidget(prevButton);
    buttonLayout->addWidget(nextButton);
    mainLayout->addLayout(buttonLayout);

    descriptionLabel = new QLabel(description, this);
    descriptionLabel->setWordWrap(true);
    QFont descFont;
    descFont.setPointSize(14);
    descFont.setBold(true);
    descriptionLabel->setFont(descFont);
    descriptionLabel->setStyleSheet(
        "background-color: #eef6f9;"
        "border: 1px solid #d0d0d0;"
        "border-radius: 8px;"
        "padding: 10px;"
        "color: #2c3e50;"
        );
    mainLayout->addWidget(descriptionLabel);

    detailedDescriptionLabel = new QLabel(detailedDescription, this);
    detailedDescriptionLabel->setWordWrap(true);
    QFont detailFont;
    detailFont.setPointSize(12);
    detailedDescriptionLabel->setFont(detailFont);
    detailedDescriptionLabel->setStyleSheet(
        "background-color: #fefefe;"
        "border: 1px dashed #b0b0b0;"
        "border-radius: 8px;"
        "padding: 8px;"
        "color: #555555;"
        );
    mainLayout->addWidget(detailedDescriptionLabel);

    connect(prevButton, &QPushButton::clicked, this, &SpotDetailDialog::showPreviousImage);
    connect(nextButton, &QPushButton::clicked, this, &SpotDetailDialog::showNextImage);

    resize(400, 650); // 增高以适应新增控件
    updateImage();
}

void SpotDetailDialog::displayPlaces(const QVector<Place>& places) {
    // 清空旧内容
    QLayoutItem *child;
    while ((child = recentLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    for (const Place& place : places) {
        QString text = QString("名称: %1\n类型: %2\n位置: (%3, %4)")
                           .arg(place.name)
                           .arg(place.type)
                           .arg(place.x)
                           .arg(place.y);
        QLabel* placeLabel = new QLabel(text, this);
        placeLabel->setWordWrap(true);
        placeLabel->setStyleSheet(
            "background-color: #f0f8ff;"
            "border: 1px solid #a0a0a0;"
            "border-radius: 5px;"
            "padding: 6px;"
            );
        recentLayout->addWidget(placeLabel);
    }
}

void SpotDetailDialog::updateImage() {
    if (imagePaths.isEmpty()) return;
    QPixmap pix(imagePaths[currentIndex]);
    if (!pix.isNull()) {
        imageLabel->setPixmap(pix.scaled(300, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void SpotDetailDialog::showPreviousImage() {
    if (imagePaths.isEmpty()) return;
    currentIndex = (currentIndex - 1 + imagePaths.size()) % imagePaths.size();
    updateImage();
}

void SpotDetailDialog::showNextImage() {
    if (imagePaths.isEmpty()) return;
    currentIndex = (currentIndex + 1) % imagePaths.size();
    updateImage();
}

void SpotDetailDialog::onSearchClicked() {
    QString input = searchLineEdit->text().trimmed();
    if (input.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入关键词或距离");
        return;
    }

    bool isNumber = false;
    double distance = input.toDouble(&isNumber);
    QVector<Place> results;
    if (isNumber) {
        handleDistanceSearch(distance);
    } else {
        results.clear();
        handleKeywordSearch(input,results);
        QPointF userPos = userposition;
        QVector<Place> nearestPlaces;
        findNearestPlaces(userPos, results, nearestPlaces,3);
        displayPlaces(nearestPlaces);
    }
}

void SpotDetailDialog::handleKeywordSearch(const QString &keyword, QVector<Place> &resultContainer) {
    resultContainer.clear();

    QSqlQuery query;
    // 使用 LIKE 实现模糊搜索（如输入“餐厅”能匹配“西餐厅”、“中餐厅”）
    query.prepare("SELECT name, type, x, y FROM places WHERE name LIKE :keyword OR type LIKE :keyword");
    query.bindValue(":keyword", "%" + keyword + "%");
    if (!query.exec()) {
        qDebug() << "查询失败";
        return;
    }

    while (query.next()) {
        Place p;
        p.name = query.value(0).toString();
        p.type = query.value(1).toString();
        p.x = query.value(2).toDouble();
        p.y = query.value(3).toDouble();
        resultContainer.append(p);
    }
}


void SpotDetailDialog::handleDistanceSearch(double distance) {
    QVector<Place> allPlaces;

    // 1. 从数据库获取所有地点
    QSqlQuery query;
    if (query.exec("SELECT name, type, x, y FROM places")) {
        while (query.next()) {
            Place p;
            p.name = query.value(0).toString();
            p.type = query.value(1).toString();
            p.x = query.value(2).toDouble();
            p.y = query.value(3).toDouble();
            allPlaces.append(p);
        }
    }

    // 2. 计算每个地点到用户当前位置的距离
    QPointF userPos = userposition;
    QVector<Place> withinDistance;
    for (const Place &p : allPlaces) {
        double dx = p.x - userPos.x();
        double dy = p.y - userPos.y();
        double dist = std::sqrt(dx * dx + dy * dy);
        if (dist <= distance) {
            withinDistance.append(p);
        }
    }

    // ⬇️ 按类别分组（聚类）
    QMap<QString, QVector<Place>> typeGroups;
    for (const Place &p : withinDistance) {
        typeGroups[p.type].append(p);
    }

    // ⬇️ 显示聚类结果地图
    displayTypeClustersOnMap(typeGroups);
}

void SpotDetailDialog::displayTypeClustersOnMap(const QMap<QString, QVector<Place>> &typeGroups) {
    QDialog *mapDialog = new QDialog(this);
    mapDialog->setWindowTitle("类型聚类地图展示");
    QVBoxLayout *layout = new QVBoxLayout(mapDialog);

    QLabel *mapLabel = new QLabel(mapDialog);
    QPixmap mapPixmap(":/bkg/img/transportation.png");
    QPixmap paintedMap = mapPixmap.copy();

    QPainter painter(&paintedMap);
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);

    // 分配颜色
    QStringList colorList = { "#e74c3c", "#27ae60", "#3498db", "#f1c40f", "#9b59b6", "#1abc9c", "#34495e" };
    int colorIndex = 0;
    QMap<QString, QColor> typeColorMap;

    for (auto it = typeGroups.begin(); it != typeGroups.end(); ++it) {
        QString type = it.key();
        QColor color = QColor(colorList[colorIndex % colorList.size()]);
        typeColorMap[type] = color;
        ++colorIndex;

        for (const Place &p : it.value()) {
            // 坐标转换
            int px = static_cast<int>(p.x); // 可加偏移缩放映射到地图图像
            int py = static_cast<int>(p.y);
            painter.setBrush(color);
            painter.setPen(Qt::black);
            painter.drawEllipse(QPoint(px, py), 5, 5);
            painter.drawText(QPoint(px + 6, py - 6), p.name);
        }
    }

    painter.end();
    mapLabel->setPixmap(paintedMap);
    layout->addWidget(mapLabel);

    // 添加图例
    QLabel *legendLabel = new QLabel("图例：", mapDialog);
    layout->addWidget(legendLabel);
    for (auto it = typeColorMap.begin(); it != typeColorMap.end(); ++it) {
        QLabel *entry = new QLabel(QString("■ %1").arg(it.key()));
        entry->setStyleSheet(QString("color: %1").arg(it.value().name()));
        layout->addWidget(entry);
    }

    mapDialog->setLayout(layout);
    mapDialog->resize(600, 500);
    mapDialog->exec();
}


// 需要保护代码，访问的可能问题如下
// 1. 输入数据为空或数量不足
// 2. ret_index 的类型与 nanoflann 定义不一致（未修复）
// 3. QVector → std::vector 转换错误,若 input 为悬空引用或数据内容未初始化，也会崩溃。
void SpotDetailDialog::findNearestPlaces(const QPointF &target, const QVector<Place> &input, QVector<Place> &output, int K) {
    output.clear();

    if (input.size() < K || K <= 0) return;

    PlaceCloud cloud;
    cloud.pts.assign(input.begin(), input.end());

    typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<double, PlaceCloud>,PlaceCloud, 2> KDTree;

    KDTree tree(2, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    tree.buildIndex();

    std::vector<unsigned int> ret_index(K);
    std::vector<double> out_dist_sqr(K);
    double query_pt[2] = { target.x(), target.y() };

    tree.knnSearch(query_pt, K, ret_index.data(), out_dist_sqr.data());

    for (unsigned int i : ret_index) {
        if (i < input.size()) {
            output.append(input[i]);
        }
    }
}

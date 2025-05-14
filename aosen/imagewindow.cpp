#include "ImageWindow.h"
#include <QCloseEvent>
#include <QStyle>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include "SpotDetailDialog.h"

ImageWindow::ImageWindow(const QString &imagePath, QWidget *parent)
    : QMainWindow(parent)
{
    imageLabel = new QLabel(this);
    QPixmap pixmap(imagePath);
    pixmap = pixmap.scaledToWidth(1000, Qt::SmoothTransformation);
    if (pixmap.isNull()) {
        imageLabel->setText("无法加载图片");
    } else {
        imageLabel->setPixmap(pixmap);
    }
    imageLabel->setAlignment(Qt::AlignCenter);
    setCentralWidget(imageLabel);
    setWindowTitle("奥森地图");

    // 1. 准备景点信息
    spots = {
        {"中华民族博物馆",
         "踏入北门，仿佛穿越千年历史长河，中华文明的瑰宝在这里静静诉说着古老与辉煌，四周绿意盎然，停车亦极为便利。",
         "中华民族博物馆坐落于奥林匹克森林公园北门附近，展陈内容涵盖中华五千年文明发展的多个方面。馆内珍藏文物丰富，配备现代化展示技术，让参观者沉浸式感受历史脉动。交通便利，周边绿意环绕，是了解中华文化精髓的绝佳去处。",
         QPoint(100, 250)},

        {"国家奥林匹克体育中心",
         "碧水环绕，微风拂面，这里是运动与自然的交响乐章。湖面点点舟影，仿佛一场轻柔的梦在中央湖畔荡漾。",
         "国家奥林匹克体育中心不仅是大型体育赛事的主场馆群，还是市民健身休闲的好去处。中心设有田径场、游泳馆、综合球馆等设施。中央湖面开阔，周边绿道环绕，每年吸引大量游客骑行、散步，尽享城市中的自然气息。",
         QPoint(120, 385)},

        {"鸟巢",
         "巍巍巨构，宛若天地之眼，春来樱花飞雪，簇拥着这片钢铁森林，仿佛世界最温柔的拥抱。",
         "国家体育场 '鸟巢' 是北京2008年奥运会主体育场，因外形独特得名。它由42,000吨钢结构交错编织而成，夜晚灯光点缀下宛如奇迹。除赛事外，鸟巢还常年举办大型演唱会、展览及冰雪嘉年华活动，是城市地标之一。",
         QPoint(240, 310)},

        {"水立方嬉水乐园",
         "晶莹剔透的蓝色泡泡，在阳光下闪烁着童年的光芒。水声嬉笑间，快乐如潮水般涌动。",
         "国家游泳中心 '水立方' 在奥运会后改建为大型水上乐园，内设多条水滑道、造浪池、儿童戏水区等项目，全年恒温开放。其独特的蓝色气泡外观与夜晚灯光秀交相辉映，是亲子游玩的热门之选。",
         QPoint(240, 240)},

        {"中国共产党历史展览馆",
         "一砖一瓦，一图一卷，皆诉说着红色征程的壮阔。步入其中，如同亲历百年风云，心潮澎湃。",
         "中国共产党历史展览馆全面展现党的百年奋斗历程，采用实物、图片、影像与沉浸式体验相结合的方式，记录历史巨变。展馆建筑大气庄重，展览内容丰富深刻，是了解中国近现代史的重要窗口。",
         QPoint(440, 375)},

        {"科技馆",
         "未来的种子在这里萌芽，奇思妙想的世界在眼前展开。每一次触碰，都是一次通往未知星辰的旅行。",
         "北京科学技术馆是国内大型的综合性科技馆，分设力学、数学、信息技术、生命科学等多个主题展区，配备互动实验装置及4D影院，旨在激发青少年对科学的兴趣与探索精神，是科普教育的重要阵地。",
         QPoint(530, 375)},

        {"奥林匹克森林公园",
         "在城市之心藏着一片静谧森林，四季流转，风光各异。晨光微露时，林间鸟鸣如诗，草木低语似梦。",
         "奥林匹克森林公园是北京最大的城市公园之一，分为南北两园，中间以景观大道连接。公园内湖泊、湿地、草原交错，四季景色变幻，被誉为 '城市之肺'。晨跑、骑行、野餐、观鸟，这里都是理想之地。",
         QPoint(750, 279)}
    };

    // 2. 创建景点按钮
    for (const SpotScene &spot : spots) {
        QPushButton *btn = new QPushButton(imageLabel);
        btn->setGeometry(spot.position.x(), spot.position.y(), 20, 20); // 小圆点
        btn->setStyleSheet("background-color: rgba(255, 0, 0, 150); border-radius: 10px;"); // 红色小圆
        btn->setToolTip(spot.name); // 鼠标悬浮提示
        connect(btn, &QPushButton::clicked, this, &ImageWindow::showSpotDetail);
        spotButtons.append(btn);
    }
}

// 点击按钮弹出详细信息窗口
void ImageWindow::showSpotDetail() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int index = spotButtons.indexOf(btn);
    if (index == -1) return;

    SpotScene spot = spots[index];

    // 每个景点的图片路径集合，可以根据你的项目结构实际配置
    QVector<QString> images;
    if (spot.name == "中华民族博物馆") {
        images = {":/bkg/img/Native1.jpg", ":/bkg/img/Native2.jpg"};
    } else if (spot.name == "鸟巢") {
        images = {":/bkg/img/nest1.jpg", ":/bkg/img/nest2.jpg"};
    } else if (spot.name == "国家奥林匹克体育中心") {
        images = {":/bkg/img/OlyCen2.jpg", ":/bkg/img/OlyCen1.jpg"};
    } else if (spot.name == "水立方嬉水乐园") {
        images = {":/bkg/img/Water1.jpg", ":/bkg/img/Water2.jpg"};
    } else if (spot.name == "中国共产党历史展览馆") {
        images = {":/bkg/img/ChinaParty1.jpg", ":/bkg/img/ChinaParty2.jpg"};
    } else if (spot.name == "科技馆") {
        images = {":/bkg/img/Science1.jpg", ":/bkg/img/Science2.jpg"};
    } else if (spot.name == "奥林匹克森林公园") {
        images = {":/bkg/img/OlyPark1.jpg", ":/bkg/img/OlyPark2.jpg", ":/bkg/img/OlyPark3.jpg"};
    }
    // 其他景点也可以配置

    SpotDetailDialog *dialog = new SpotDetailDialog(spot.name, spot.description, spot.detail, images, spot.position , this);
    dialog->exec();
}

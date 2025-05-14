#include "shortestpath.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <queue>
#include <vector>
#include <algorithm>
#include <QMessageBox>
using std::vector;
using std::min;
using std::pair;

void ShortestPath::onInputError(int i){
    if(i == 1){
        QMessageBox::warning(this,
                             tr("输入错误"),
                             tr("请输入正确地铁站"),
                             QMessageBox::Ok);
    }else{
        QMessageBox::warning(this,
                             tr("输入错误"),
                             tr("请输入正确景点"),
                             QMessageBox::Ok);
    }
}

void ShortestPath::showStartSuggestion(QString text){
    if (text.isEmpty()) {
        startSuggest->hide();
        return;
    }

    // 过滤匹配项
    QStringList filtered;

    for (const QString &item : std::as_const(stationSuggestions)) {
        if (item.contains(text, Qt::CaseInsensitive)) {
            filtered << item;
        }
    }

    // 更新提示框
    startSuggest->clear();
    if (!filtered.isEmpty()) {
        startSuggest->addItems(filtered);
        startSuggest->setFixedHeight(std::min(static_cast<int>(filtered.size() * 20), 100));
        startSuggest->move(start->mapToParent(QPoint(0, start->height())));
        startSuggest->show();
    } else {
        startSuggest->hide();
    }
}

void ShortestPath::showEndSuggestion(QString text){
    if (text.isEmpty()) {
        endSuggest->hide();
        return;
    }

    // 过滤匹配项
    QStringList filtered;
    for (const QString &item : std::as_const(spotSuggestions)) {
        if (item.contains(text, Qt::CaseInsensitive)) {
            filtered << item;
        }
    }


    // 更新提示框
    endSuggest->clear();
    if (!filtered.isEmpty()) {
        endSuggest->addItems(filtered);
        endSuggest->setFixedHeight(std::min(static_cast<int>(filtered.size() * 20), 100));
        endSuggest->move(start->mapToParent(QPoint(0, start->height())));
        endSuggest->show();
    } else {
        endSuggest->hide();
    }
}

void ShortestPath::chooseStartSuggestion(QListWidgetItem *item){
    start->setText(item->text());
    startSuggest->hide();
}

void ShortestPath::chooseEndSuggestion(QListWidgetItem *item){
    end->setText(item->text());
    endSuggest->hide();
}

void ShortestPath::init(){
    centerWidget = new QWidget();
    setWindowTitle("最短路程");
    setFixedSize(800,600);

    QIcon icon(":/bkg/img/search.png");
    pushBtn = new QPushButton();
    pushBtn->setFlat(true);
    pushBtn->setToolTip("搜索");
    pushBtn->setIcon(icon);
    pushBtn->setIconSize(QSize(24, 24)); // 设置图标大小
    pushBtn->setFixedSize(40, 30);       // 设置按钮大小

    start = new QLineEdit();
    start->setPlaceholderText("请输入您所处地铁站");
    start->setFixedHeight(30);
    start->setMinimumWidth(300);

    startSuggest = new QListWidget(this);
    startSuggest->hide();

    end = new QLineEdit();
    end->setPlaceholderText("请输入想到达景点,如不填,默认逛完所有景点");
    end->setFixedHeight(30);
    end->setMinimumWidth(300);

    endSuggest = new QListWidget(this);
    endSuggest->hide();

    path = new QTextEdit();
    path->setReadOnly(true);
    path->setStyleSheet("font-size: 14px;");

    for(int i = 0;i<SPOTSIZE;i++){
        spotSuggestions.append(spots[i]);
    }
    for(int i = SPOTSIZE;i < SIZE;i++){
        stationSuggestions.append(spots[i]);
    }

    QVBoxLayout *VLayout = new QVBoxLayout;
    VLayout->addWidget(start);
    VLayout->addWidget(startSuggest);
    VLayout->addWidget(end);
    VLayout->addWidget(endSuggest);
    VLayout->setSpacing(10);

    QHBoxLayout *HLayout = new QHBoxLayout;
    HLayout->addLayout(VLayout);
    HLayout->addWidget(pushBtn);
    HLayout->setSpacing(10);

    QVBoxLayout *VLayoutTotal = new QVBoxLayout;
    VLayoutTotal->addLayout(HLayout);
    VLayoutTotal->addWidget(path);
    VLayoutTotal->setContentsMargins(15,15,15,15);
    VLayoutTotal->setSpacing(10);

    centerWidget->setLayout(VLayoutTotal);
    setCentralWidget(centerWidget);
}

int ShortestPath::getSpotId(QString spot){
    for(int i = 0;i < SPOTSIZE;i++){//找到景点编号
        if(spots[i] == spot){
            return i;
        }
    }
    return -1;
}

int ShortestPath::getStationId(QString station){
    for(int i = SPOTSIZE;i < SIZE;i++){//找到地铁站编号
        if(spots[i] == station){
            return i;
        }
    }
    return -1;
}

int ShortestPath::findnearestSpot(int stationId){
    if (stationId < SPOTSIZE || stationId >= SIZE) { // 地铁站ID范围是8~10
        return -1;
    }

    double minDist = INT_MAX;
    int nearestSpotId = -1;

    // 遍历所有景点（索引0~7）
    for (int spotId = 0; spotId < 8; ++spotId) {
        double dist = distance[stationId][spotId];
        if (dist != -1 && dist < minDist) {
            minDist = dist;
            nearestSpotId = spotId;
        }
    }

    if (nearestSpotId == -1) {
        return -1;
    }
    return nearestSpotId;
}

int ShortestPath::findnearestStation(int spotId){
    if (spotId < 0 || spotId >= SPOTSIZE) {
        return -1;
    }

    double minDist = INT_MAX;
    int nearestMetroId = -1;

    // 地铁站的索引是 8、9、10
    for (int metroId = 8; metroId <= 10; ++metroId) {
        double dist = distance[spotId][metroId];
        if (dist != -1 && dist < minDist) {
            minDist = dist;
            nearestMetroId = metroId;
        }
    }

    if (nearestMetroId == -1) {
        return -1;
    }

    return nearestMetroId;
}

QString ShortestPath::TSP(QString startText) {
    QString text = QString("从 %1 下车，建议的最短路线如下:\n").arg(startText);
    int stationId = getStationId(startText);
    if(stationId == -1){
        onInputError(1);
        return text = "输入地铁站错误，请根据地图框选地铁站输入";
    }
    int start = findnearestSpot(stationId);
    if(start == -1){
        return text = "无法找到最近景点";
    }
    const int full_mask = (1 << SPOTSIZE) - 1;
    vector<vector<double>> dp(full_mask + 1, vector<double>(SPOTSIZE, INT_MAX));
    vector<vector<pair<int, int>>> prev(full_mask + 1, vector<pair<int, int>>(SPOTSIZE, {-1, -1}));
    dp[1 << start][start] = 0;

    // DP 状态转移
    for (int mask = 0; mask <= full_mask; ++mask) {//外层循环
        for (int u = 0; u < SPOTSIZE; ++u) {//内层循环
            if (!(mask & (1 << u))) continue; // 未访问u，跳过
            for (int v = 0; v < SPOTSIZE; ++v) {
                if (mask & (1 << v)) continue; // 已访问v，跳过
                int new_mask = mask | (1 << v);
                if (dp[new_mask][v] > dp[mask][u] + distance[u][v]) {
                    dp[new_mask][v] = dp[mask][u] + distance[u][v];
                    prev[new_mask][v] = {mask, u}; // 记录前驱状态
                }
            }
        }
    }

    // 找到最优解的终点
    double min_dist = INT_MAX;
    int end_spot = -1;
    for (int u = 0; u < SPOTSIZE; ++u) {
        if (u == start) continue; // 跳过起点（如果不允许返回起点）
        if (dp[full_mask][u] < min_dist) {
            min_dist = dp[full_mask][u];
            end_spot = u;
        }
    }

    // 回溯路径
    vector<int> path;
    vector<double> distances;
    int current_mask = full_mask;
    int current_spot = end_spot;
    while (prev[current_mask][current_spot].first != -1) {
        auto [prev_mask, prev_spot] = prev[current_mask][current_spot];
        path.push_back(current_spot);
        distances.push_back(distance[prev_spot][current_spot]);
        current_mask = prev_mask;
        current_spot = prev_spot;
    }
    path.push_back(start); // 加入起点
    reverse(path.begin(), path.end()); // 反转得到从起点到终点的顺序
    reverse(distances.begin(), distances.end()); // 距离也需要反转

    text += QString("%1--%2 km-->")
                .arg(spots[stationId])
                .arg(distance[stationId][start]);
    // 总路线
    for (int i = 0; i < static_cast<int>(path.size()-1); ++i) {
        text += QString("%1--%2 km-->")
        .arg(spots[path[i]])
            .arg(distances[i]);
    }
    int end = path[path.size()-1];
    int endStation = findnearestStation(end);
    text += QString("%1--%2 km-->%3")
                .arg(spots[end])
                .arg(distance[end][endStation])
                .arg(spots[endStation]);

    min_dist += distance[end][endStation]+ distance[stationId][start];
    // 添加总距离
    text += QString("\n总距离: %1 km").arg(min_dist);
    return text;
}

QString ShortestPath::DijkstraSpots(QString startText,QString endText) {
    QString text = QString("从 %1 下车，建议的最短路线如下:\n").arg(startText);
    int stationId = getStationId(startText);
    int endSpotId = getSpotId(endText);
    if (stationId == -1){
        onInputError(1);
        return text = "输入地铁站错误，请根据地图框选地铁站输入";
    }
    if(endSpotId == -1){
        onInputError(2);
        return text = "输入景点错误，请根据地图框选景点输入";
    }

    int start = findnearestSpot(stationId);
    if (start == -1 || start >= SIZE) return "无法找到最近景点！";

    // 初始化
    std::vector<double> dist(SIZE, INT_MAX);
    std::vector<int> prev(SIZE, -1);
    std::vector<bool> visited(SIZE, false);

    dist[start] = 0;
    using PDI = std::pair<double, int>; // {距离, 节点}
    std::priority_queue<PDI, std::vector<PDI>, std::greater<PDI>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;

        for (int v = 0; v < SIZE; ++v) {
            if (!visited[v] && distance[u][v] != -1) {
                double newDist = dist[u] + distance[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
    }

    QString path = QString("%1 到 %2 Distance：%3 km\t")
                       .arg(spots[start])
                       .arg(spots[endSpotId])
                       .arg(dist[endSpotId]);
    std::vector<int> route;
    for (int at = endSpotId; at != -1; at = prev[at]) route.push_back(at);
    std::reverse(route.begin(), route.end());

    for (size_t j = 0; j < route.size(); ++j) {
        path += spots[route[j]];
        if (j + 1 < route.size()) path += " --> ";
    }

    text += path;

    return text;
}

void ShortestPath::onClickBtn(){
    QString endText = end->text();
    QString startText = start->text();

    if(!startText.isEmpty()){
        if(endText.isEmpty()){   //表示景点全游览
            path->setPlainText(TSP(startText));
        }
        else{   //表示从地铁站下后到
            path->setPlainText(DijkstraSpots(startText,endText));
        }
    }
}

void ShortestPath::initSignalSlots(){
    connect(pushBtn, &QPushButton::clicked, this, &ShortestPath::onClickBtn);
    connect(start, &QLineEdit::textChanged, this,&ShortestPath::showStartSuggestion);
    connect(startSuggest,&QListWidget::itemClicked,this,&ShortestPath::chooseStartSuggestion);
    connect(end,&QLineEdit::textChanged,this,&ShortestPath::showEndSuggestion);
    connect(endSuggest,&QListWidget::itemClicked,this,&ShortestPath::chooseEndSuggestion);
}

ShortestPath::ShortestPath(QWidget *parent)
    :MainWindow(parent) {
    init();
    initSignalSlots();
}

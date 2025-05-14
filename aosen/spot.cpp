#include "Spot.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <algorithm>

#include <QMessageBox>
void Spot::onInputError()
{
    QMessageBox::warning(this,
                         tr("输入错误"),
                         tr("请输入有效的景点"),
                         QMessageBox::Ok);
}

void Spot::showSuggestion(QString text) {
    if (text.isEmpty()) {
        suggestionList->hide();
        return;
    }

    // 过滤匹配项
    QStringList filtered;
    for (const QString &item : std::as_const(suggestions)) {
        if (item.contains(text, Qt::CaseInsensitive)) {
            filtered << item;
        }
    }

    // 更新提示框
    suggestionList->clear();
    if (!filtered.isEmpty()) {
        suggestionList->addItems(filtered);
        suggestionList->setFixedHeight(std::min(static_cast<int>(filtered.size() * 20), 100));
        suggestionList->move(searchEdit->mapToParent(QPoint(0, searchEdit->height())));
        suggestionList->show();
    } else {
        suggestionList->hide();
    }
}

void Spot::chooseSuggestion(QListWidgetItem *item){
    searchEdit->setText(item->text());
    suggestionList->hide();
}

QString Spot::listSpotsByDistance(QString text){
    int rowToSort = -1;
    for(int i = 0;i < SIZE;i++){
        if(text == spots[i])
            rowToSort = i;
    }
    if(rowToSort == -1){
        onInputError();
        return "景点输入错误，请根据地图框选景点输入！！";
    }

    std::pair<double, int> row[SPOTSIZE];  //first: distance,int: index of spot

    for(int i = 0 ;i < SPOTSIZE;++i){
        row[i] = {distance[rowToSort][i],i};
    }

    bool flag = false;
    for(int i = 0; i < SPOTSIZE - 1; i++) {
        for(int j = 0; j < SPOTSIZE - 1 - i; j++) {
            if(row[j].first > row[j + 1].first) {
                flag = true;
                std::swap(row[j], row[j + 1]);
            }
        }
        if(!flag)
            break;
        else
            flag = false;
    }

    // 将排序结果添加到 QString 中
    QString result;
    result += QString("从 %1 出发，按距离升序排序：\n").arg(spots[rowToSort]);

    for (int j = 0; j < SPOTSIZE; ++j) {
        if (row[j].first == -1) continue;
        int index = row[j].second;
        if(index == rowToSort) continue;
        result += QString("%1: %2 km\n").arg(spots[index]).arg(row[j].first);
    }
    return text = result;
}

void Spot::init() {
    centerWidget = new QWidget();
    setWindowTitle("景点距离排序搜索");
    setFixedSize(500, 400); // 设置窗口固定大小

    QIcon icon(":/bkg/img/search.png");
    pushBtn = new QPushButton();
    pushBtn->setFlat(true);
    pushBtn->setToolTip("搜索");
    pushBtn->setIcon(icon);
    pushBtn->setIconSize(QSize(24, 24)); // 设置图标大小
    pushBtn->setFixedSize(40, 30);       // 设置按钮大小

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("请输入你想要搜索的景点");
    searchEdit->setFixedHeight(30);
    searchEdit->setMinimumWidth(300);

    showText = new QTextEdit;
    showText->setReadOnly(true);
    showText->setStyleSheet("font-size: 14px;"); // 设置字体大小

    suggestionList = new QListWidget(this);
    suggestionList->setHidden(true);

    for(QString item : spots){
        suggestions.append(item);
    }

    // 设置水平布局：搜索框 + 按钮
    QHBoxLayout *HLayout = new QHBoxLayout;
    HLayout->addWidget(searchEdit);
    HLayout->addWidget(pushBtn);
    HLayout->setSpacing(10); // 组件间距

    // 垂直布局：上面是搜索栏，下面是显示框
    QVBoxLayout *VLayout = new QVBoxLayout;
    VLayout->addLayout(HLayout);
    VLayout->addWidget(suggestionList);
    VLayout->addWidget(showText);
    VLayout->setContentsMargins(15, 15, 15, 15); // 外边距
    VLayout->setSpacing(15);

    centerWidget->setLayout(VLayout);
    setCentralWidget(centerWidget);
}

void Spot::onClickBtn(){
    QString text = searchEdit->text();
    text = listSpotsByDistance(text);
    showText->setPlainText(text);
}

void Spot::initSignalSlots(){
    connect(pushBtn, &QPushButton::clicked, this, &Spot::onClickBtn);
    connect(searchEdit,&QLineEdit::textChanged,this,&Spot::showSuggestion);
    connect(suggestionList,&QListWidget::itemClicked,this,&Spot::chooseSuggestion);
}

Spot::Spot(QWidget *parent)
    :MainWindow(parent){
    init();
    initSignalSlots();
}

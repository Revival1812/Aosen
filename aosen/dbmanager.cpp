// dbmanager.cpp
#include "dbmanager.h"

DBManager::DBManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("place_data.db");

    if (!db.open()) {
        qDebug() << "❌ Failed to connect to DB:" << db.lastError().text();
    } else {
        qDebug() << "✅ Connected to SQLite database.";
    }
}

DBManager::~DBManager() {
    if (db.isOpen()) db.close();
}

void DBManager::createTable() {
    QSqlQuery query;
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS Places ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT, "
        "short_desc TEXT, "
        "long_desc TEXT, "
        "type TEXT, "
        "x INTEGER, "
        "y INTEGER)"
        );

    if (success) {
        qDebug() << "✅ Table 'Places' created or already exists.";
    } else {
        qDebug() << "❌ Failed to create table:" << query.lastError().text();
    }
}

void DBManager::insertPlace(const QString &name, const QString &shortDesc,
                            const QString &longDesc, const QString &type,
                            int x, int y) {
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM Places WHERE name = ? OR (x = ? AND y = ?)");
    checkQuery.addBindValue(name);
    checkQuery.addBindValue(x);
    checkQuery.addBindValue(y);

    if (checkQuery.exec() && checkQuery.next()) {
        int count = checkQuery.value(0).toInt();
        if (count > 0) {
            qDebug() << "⚠️ 已存在相同名称或坐标的地点，不进行插入：" << name;
            return;
        }
    }

    QSqlQuery query;
    query.prepare(
        "INSERT INTO Places (name, short_desc, long_desc, type, x, y) "
        "VALUES (?, ?, ?, ?, ?, ?)"
        );
    query.addBindValue(name);
    query.addBindValue(shortDesc);
    query.addBindValue(longDesc);
    query.addBindValue(type);
    query.addBindValue(x);
    query.addBindValue(y);

    if (!query.exec()) {
        qDebug() << "❌ Insert failed:" << query.lastError().text();
    } else {
        qDebug() << "✅ Place inserted:" << name;
    }
}

void DBManager::printAllPlaces() {
    QSqlQuery query("SELECT id, name, type, x, y FROM Places");
    while (query.next()) {
        qDebug() << "📍 ID:" << query.value(0).toInt()
            << " Name:" << query.value(1).toString()
            << " Type:" << query.value(2).toString()
            << " Pos:" << query.value(3).toInt() << "," << query.value(4).toInt();
    }
}

// dbmanager.cpp
void DBManager::clearTable() {
    QSqlQuery query;
    if (!query.exec("DELETE FROM Places")) {
        qDebug() << "❌ 清空Places表失败:" << query.lastError().text();
    } else {
        qDebug() << "🧹 Places表已清空";
    }
}

// dbmanager.h
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>
#include <QDebug>
#include <QPoint>

class DBManager {
public:
    DBManager();
    ~DBManager();
    void createTable();
    void insertPlace(const QString &name, const QString &shortDesc,
                     const QString &longDesc, const QString &type,
                     int x, int y);
    void printAllPlaces();
    void clearTable();

private:
    QSqlDatabase db;
};

#endif // DBMANAGER_H

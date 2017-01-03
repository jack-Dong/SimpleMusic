#ifndef MYSQLDB_H
#define MYSQLDB_H

#include <QSqlDatabase>
#include <QStringList>
#include <QSqlQuery>
#include <QVector>
#include <QVariant>

class MysqlDB:public QObject
{
    Q_OBJECT
private:
    QSqlDatabase *db;
    QSqlQuery *query;
public:
    MysqlDB();
    //插入
    bool insert(QString,QString, QString ,QString,QString);
    //查找
    QVector<QStringList> serachAll();
    QVector<QStringList> serachByName(QString);
    QVector<QStringList> serachBySinger(QString);
    QVector<QStringList> serachById(QString);
};


#endif // MYSQLDB_H

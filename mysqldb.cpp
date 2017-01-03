#include "mysqldb.h"
#include <QDebug>
#include <QSqlError>

MysqlDB::MysqlDB()
{
    db = & (QSqlDatabase::addDatabase("QMYSQL"));
    db->setHostName("localhost");
    db->setDatabaseName("music");
    db->setPort(3306);
    db->setUserName("root");
    db->setPassword("");
    bool ok = db->open();
    query = new QSqlQuery(*db);
    qDebug() <<ok;

}

bool MysqlDB::insert(QString name, QString singer, QString path, QString imgPath, QString spell)
{
    query->prepare("insert into video(name,singer,path,imgPath) values (:name,:singer,:path,:imgPath)");
    query->bindValue(":name",name);
    query->bindValue(":singer",singer);
    query->bindValue(":path",path);
    query->bindValue(":imgPath",imgPath);
    query->exec();
    qDebug() << query->lastError().text();
    return true;
}

QVector<QStringList> MysqlDB::serachAll()
{
    QVector<QStringList>  vsl;
    QStringList temp;
    query->exec("select * from video");
    while (query->next())
    {
        temp.push_back(query->value(0).toString());
        temp.push_back(query->value(1).toString());
        temp.push_back(query->value(2).toString());
        temp.push_back(query->value(3).toString());
        temp.push_back(query->value(4).toString());
        temp.push_back(query->value(5).toString());
        vsl.push_back(temp);
        temp.clear();
    }
    return vsl;
}

QVector<QStringList> MysqlDB::serachByName(QString name)
{
    QVector<QStringList>  vsl;
    QStringList temp;
    QString str = QString("select * from video where name like '%%1%'").arg(name);
    qDebug() <<str<<endl;
    query->exec(str);
    while (query->next())
    {
        temp.push_back(query->value(0).toString());
        temp.push_back(query->value(1).toString());
        temp.push_back(query->value(2).toString());
        temp.push_back(query->value(3).toString());
        temp.push_back(query->value(4).toString());
        temp.push_back(query->value(5).toString());
        vsl.push_back(temp);
        temp.clear();
    }
    return vsl;
}

QVector<QStringList> MysqlDB::serachBySinger(QString singer)
{
    QVector<QStringList>  vsl;
    QStringList temp;
    QString str = QString("select * from video where singer like '%%1%'").arg(singer);
    query->exec(str);
    while (query->next())
    {
        temp.push_back(query->value(0).toString());
        temp.push_back(query->value(1).toString());
        temp.push_back(query->value(2).toString());
        temp.push_back(query->value(3).toString());
        temp.push_back(query->value(4).toString());
        temp.push_back(query->value(5).toString());
        vsl.push_back(temp);
        temp.clear();
    }
    return vsl;
}

QVector<QStringList> MysqlDB::serachById(QString id)
{
    QVector<QStringList>  vsl;
    QStringList temp;
    query->exec(QString("select * from video where id = %1").arg(id));
    while (query->next())
    {
        temp.push_back(query->value(0).toString());
        temp.push_back(query->value(1).toString());
        temp.push_back(query->value(2).toString());
        temp.push_back(query->value(3).toString());
        temp.push_back(query->value(4).toString());
        temp.push_back(query->value(5).toString());
        vsl.push_back(temp);
        temp.clear();
    }
    return vsl;
}


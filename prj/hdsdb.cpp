#include "hdsdb.h"
#include "hdsconf.h"

IMPL_SINGLETON(HDsDB)

HDsDB::HDsDB()
{
    db = QSqlDatabase::addDatabase(dsconf->value("SQL/type"), "dsdb");
    db.setHostName(dsconf->value("SQL/hostname"));
    db.setPort(dsconf->value("SQL/port").toInt());
    db.setDatabaseName(dsconf->value("SQL/dbname"));
    db.setUserName(dsconf->value("SQL/username"));
    //db.setPassword(dsconf->value("SQL/password"));
    qInfo("type=%s hostname=%s port=%d dbname=%s username=%s passwd=%s", db.driverName().toLocal8Bit().data(),db.hostName().toLocal8Bit().data(),
          db.port(), db.databaseName().toLocal8Bit().data(), db.userName().toLocal8Bit().data(), db.password().toLocal8Bit().data());
    bool ok = db.open();
    qInfo("db connect result=%d", ok);
    if (!ok){
        qInfo(db.lastError().text().toLocal8Bit().data());
    }
}

HDsDB::~HDsDB(){
    db.close();
    QSqlDatabase::removeDatabase("dsdb");
}

QSqlQuery HDsDB::exec(QString sql){
    qInfo(sql.toLocal8Bit().data());
    QSqlQuery query = db.exec(sql);
    if (!query.isActive()){
        qInfo(query.lastError().text().toLocal8Bit().data());
        qInfo(db.lastError().text().toLocal8Bit().data());
    }

    return query;
}

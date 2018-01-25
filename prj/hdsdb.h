#ifndef HDSDB_H
#define HDSDB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "singleton.h"

class HDsDB
{
    DECLARE_SINGLETON(HDsDB)
public:
    HDsDB();
    ~HDsDB();

    QSqlQuery exec(QString sql);

public:
    QSqlDatabase db;
};

#define dsdb HDsDB::instance()

#endif // HDSDB_H

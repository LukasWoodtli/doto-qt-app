#pragma once

#include <QObject>
#include <QSqlDatabase>

namespace db {

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QString name, QObject *parent = nullptr);

    void createSchema();

private:
    QSqlDatabase m_database;

};

}

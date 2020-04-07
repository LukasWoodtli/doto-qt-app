#pragma once

#include <QObject>
#include <QSqlDatabase>

#include "internal/database_schema.h"
namespace db {

class DataBase : public QObject {
	Q_OBJECT
public:
	explicit DataBase(QString name, QObject* parent = nullptr);

	void createSchema();

	// CREATE
	template<class DTO>
	void createRecord(const DTO& dto) {
		createRecordFromSql(internal::createRecordSql(dto));
	}

private:
	void createRecordFromSql(QString sql);

private:
	QSqlDatabase m_database;
};

}

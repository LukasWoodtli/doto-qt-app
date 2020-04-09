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

	// READ
	template<class DTO_T>
	DTO_T readRecord(const QUuid& uuid) const {
		return internal::readRecord<DTO_T>(uuid);
	}

	// DELETE
	template<class DTO_T>
	void deleteRecord(const QUuid& uuid) const {
		internal::deleteRecord<DTO_T>(uuid);
	}

private:
	void createRecordFromSql(QString sql);

private:
	QSqlDatabase m_database;
};

}

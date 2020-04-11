#pragma once

#include <QObject>
#include <QSqlDatabase>

#include "internal/database_schema.h"
namespace db {

class DataBase : public QObject {
	Q_OBJECT
public:
	explicit DataBase(QString name, QObject* parent = nullptr);
	virtual ~DataBase();

	void createSchema();

	// CREATE
	template<class DTO_T>
	void createRecord(const DTO_T& dto) {
		internal::createRecord<DTO_T>(dto);
	}

	// READ
	template<class DTO_T>
	DTO_T readRecord(const QUuid& uuid) const {
		return internal::readRecord<DTO_T>(uuid);
	}

	// UPDATE
	template<class DTO_T>
	void updateRecord(const DTO_T& dto) const {
		internal::updateRecord<DTO_T>(dto);
	}

	// DELETE
	template<class DTO_T>
	void deleteRecord(const QUuid& uuid) const {
		internal::deleteRecord<DTO_T>(uuid);
	}

private:
	QSqlDatabase m_database;
};

}

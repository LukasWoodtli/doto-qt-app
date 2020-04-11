#include <QDate>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringList>

#include "data_transfer_objects.h"
#include "database_schema.h"
#include "mappings.h"

namespace {
QSqlQuery executeQuery(const QString& querySql) {
	QSqlQuery query = QSqlQuery();
	const auto success = query.exec(querySql);
	qDebug() << "Executing SQL: " << query.lastQuery();
	if (not success) {
		qCritical() << query.lastError();
	}
	Q_ASSERT(success);
	query.next();
	return query;
}
}
namespace db::internal {
const constexpr auto CREATE_TABLE_SQL_TEMPLATE =
  "CREATE TABLE IF NOT EXISTS %1 (%2)";
const constexpr auto FOREIGN_KEY_SQL_TEMPLATE =
  "FOREIGN KEY(%1) REFERENCES %2(%3)";
QStringList schemaCreationSql() {
	QStringList createTableQueries;
#define TABLE(name)                                                            \
	{                                                                            \
		const auto tableName = #name;                                              \
		QStringList columnsAndForeignConstraints;                                  \
		columnsAndForeignConstraints.append("UUID TEXT UNIQUE");
#define COLUMN(name, type, ...)                                                \
	columnsAndForeignConstraints.append(                                         \
	  QString(#name " ") + mappings::mapping<type>::DB_TYPE + " " #__VA_ARGS__);
#define FOREIGN_KEY(column, foreign_table, foreign_column)                     \
	columnsAndForeignConstraints.append(QString(FOREIGN_KEY_SQL_TEMPLATE)        \
	                                      .arg(#column)                          \
	                                      .arg(#foreign_table)                   \
	                                      .arg(#foreign_column));
#define ENDTABLE()                                                             \
	const auto queryForOneTable =                                                \
	  QString(CREATE_TABLE_SQL_TEMPLATE)                                         \
	    .arg(tableName)                                                          \
	    .arg(columnsAndForeignConstraints.join(", "));                           \
	createTableQueries << queryForOneTable.simplified();                         \
	}

#include "db_schema.xdef"

#undef TABLE
#undef COLUMN
#undef VARIANT_COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

	return createTableQueries;
}
void createSchema() {
	for (const auto& querySql : schemaCreationSql())
		executeQuery(querySql);
}

// CREATE
const constexpr auto CREATE_SQL_TEMPLATE = "INSERT INTO %1 (%2) VALUES(%3)";
// "INSERT INTO TASKS VALUES ('31bd6531-7e85-4fa3-9969-5e57aa65fa1e', '', '0',
// '')"
#define TABLE(name)                                                            \
	template<>                                                                   \
	void createRecord<name##_DTO>(const name##_DTO& dto) {                       \
		const auto tableName = #name;                                              \
		QStringList columnNames;                                                   \
		QStringList values;                                                        \
		columnNames.append("UUID");                                                \
		values.append("'" + dto.m_Uuid.toString(QUuid::WithoutBraces) + "'");
#define COLUMN(name, type, ...)                                                \
	columnNames.append(#name);                                                   \
	values.append("'" + mappings::mapping<type>::toDb(dto.m_##name) + "'");
#define FOREIGN_KEY(column, foreign_table, foreign_column)                     \
	qDebug() << "TODO: Add shared_ptr from " << #column << " to "                \
	         << #foreign_table << ":" << #foreign_column;
#define ENDTABLE()                                                             \
	const auto query = QString(CREATE_SQL_TEMPLATE)                              \
	                     .arg(tableName)                                         \
	                     .arg(columnNames.join(", "))                            \
	                     .arg(values.join(", "));                                \
	executeQuery(query);                                                         \
	}
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

// READ
const constexpr auto READ_SQL_TEMPLATE = "SELECT * FROM %1 WHERE UUID='%2'";
#define TABLE(name)                                                            \
	template<>                                                                   \
	name##_DTO readRecord<name##_DTO>(const QUuid& uuid) {                       \
		const auto querySql = QString(READ_SQL_TEMPLATE)                           \
		                        .arg(#name)                                        \
		                        .arg(uuid.toString(QUuid::WithoutBraces));         \
		const auto query = executeQuery(querySql);                                 \
		name##_DTO dto{query.value(0).value<QUuid>()};                             \
		auto columnCount = 1;
#define COLUMN(name, type, ...)                                                \
	dto.m_##name = query.value(columnCount).value<type>();                       \
	++columnCount;
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE()                                                             \
	return dto;                                                                  \
	}
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

// UPDATE
const constexpr auto UPDATE_SQL_TEMPLATE = "UPDATE %1 SET %2 WHERE UUID='%3'";
#define TABLE(name)                                                            \
	template<>                                                                   \
	void updateRecord<name##_DTO>(const name##_DTO& dto) {                       \
		const auto tableName = #name;
QStringList updateFieldList;
#define COLUMN(name, type, ...)                                                \
	updateFieldList.append(#name " = '" +                                        \
	                       mappings::mapping<type>::toDb(dto.m_##name) + "'");
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE()                                                             \
	auto querySql = QString(UPDATE_SQL_TEMPLATE)                                 \
	                  .arg(tableName)                                            \
	                  .arg(updateFieldList.join(", "))                           \
	                  .arg(dto.m_Uuid.toString(QUuid::WithoutBraces));           \
	executeQuery(querySql);                                                      \
	}
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

// DELETE
const constexpr auto DELETE_SQL_TEMPLATE = "DELETE * FROM %1 WHERE UUID='%2'";
#define TABLE(name)                                                            \
	template<>                                                                   \
	void deleteRecord<name##_DTO>(const QUuid& uuid) {                           \
		const auto querySql = QString(READ_SQL_TEMPLATE)                           \
		                        .arg(#name)                                        \
		                        .arg(uuid.toString(QUuid::WithoutBraces));         \
		executeQuery(querySql);
#define COLUMN(name, type, ...)
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE() }
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE
}

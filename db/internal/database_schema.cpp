#include <QDate>
#include <QSqlQuery>
#include <QString>
#include <QStringList>

#include <tuple>

#include "data_transfer_objects.h"
#include "database_schema.h"
#include "mappings.h"
#include <QSqlError>

namespace db::internal {

QStringList schemaCreationSql() {
	QStringList createTableQueries;
#define TABLE(name)                                                            \
	{                                                                            \
		auto query =                                                               \
		  QString("CREATE TABLE IF NOT EXISTS " #name " (UUID TEXT UNIQUE")
#define COLUMN(name, type, ...)                                                \
	+", " #name " " + mappings::mapping<type>::DB_TYPE + " " #__VA_ARGS__
#define FOREIGN_KEY(column, foreign_table, foreign_column)                     \
	+", FOREIGN KEY(" #column ") REFERENCES " #foreign_table "(" #foreign_column \
	 ")"
#define ENDTABLE()                                                             \
	+QStringLiteral(")");                                                        \
	createTableQueries << query.simplified();                                    \
	}

#include "db_schema.xdef"

#undef TABLE
#undef COLUMN
#undef VARIANT_COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

	return createTableQueries;
}

// CREATE
#define TABLE(name)                                                            \
	QString createRecordSql(const name##_DTO& dto) {                             \
		auto sqlQuery = QString("INSERT INTO " #name " VALUES (");                 \
		sqlQuery += "'" + dto.m_Uuid.toString(QUuid::WithoutBraces) + "'";
#define COLUMN(name, type, ...)                                                \
	sqlQuery += ", '" + mappings::mapping<type>::toDb(dto.m_##name) + "'";
#define FOREIGN_KEY(column, foreign_table, foreign_column)                     \
	qDebug() << "TODO: Add shared_ptr from " << #column << " to "                \
	         << #foreign_table << ":" << #foreign_column;
#define ENDTABLE()                                                             \
	sqlQuery += ")";                                                             \
	return sqlQuery.simplified();                                                \
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
		qDebug() << "READ SQL query: " << querySql;                                \
		QSqlQuery query(querySql);                                                 \
		const auto success = query.exec();                                         \
		Q_ASSERT(success);                                                         \
		query.next();                                                              \
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
	qDebug() << "UPDATE SQL query: " << querySql;                                \
	QSqlQuery query(querySql);                                                   \
	[[maybe_unused]] const auto success = query.exec();                          \
	qDebug() << query.lastQuery();                                               \
	qDebug() << query.lastError();                                               \
	Q_ASSERT(success);                                                           \
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
		qDebug() << "DELETE SQL query: " << querySql;                              \
		QSqlQuery query(querySql);                                                 \
		[[maybe_unused]] const auto success = query.exec();                        \
		Q_ASSERT(success);
#define COLUMN(name, type, ...)
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE() }
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE
}

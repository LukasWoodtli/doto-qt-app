#include <QStringList>
#include <QString>
#include <QDate>

#include <tuple>

#include "data_transfer_objects.h"
#include "database_schema.h"
#include "mappings.h"

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

}

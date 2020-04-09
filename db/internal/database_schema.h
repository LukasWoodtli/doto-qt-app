#pragma once

#include <QDebug>

#include "data_transfer_objects.h"
#include "mappings.h"

namespace db::internal {
QStringList schemaCreationSql();

// CREATE
#define TABLE(name) QString createRecordSql(const name##_DTO& dto);
#define COLUMN(name, type, ...)
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE()
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

// READ
template<class DTO_T>
DTO_T readRecord(const QUuid& uuid);

#define TABLE(name)                                                            \
	template<>                                                                   \
	name##_DTO readRecord<name##_DTO>(const QUuid& uuid);
#define COLUMN(name, type, ...)
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE()
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

// UPDATE
template<class DTO_T>
void updateRecord(const DTO_T& dto);

#define TABLE(name)                                                            \
	template<>                                                                   \
	void updateRecord<name##_DTO>(const name##_DTO& dto);
#define COLUMN(name, type, ...)
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE()
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

// DELETE
template<class DTO_T>
void deleteRecord(const QUuid& uuid);

#define TABLE(name)                                                            \
	template<>                                                                   \
	void deleteRecord<name##_DTO>(const QUuid& uuid);
#define COLUMN(name, type, ...)
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE()
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE
}

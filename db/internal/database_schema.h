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

}

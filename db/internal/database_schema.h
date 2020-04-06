#pragma once

#include <QDebug>

#include "data_transfer_objects.h"
#include "mappings.h"

namespace db::internal {
QStringList schemaCreationSql();

// CREATE
#define TABLE(name) inline QString createRecordSql(const name##_DTO &dto) { \
    auto sqlQuery = QString("INSERT INTO " #name " VALUES ("); \
    sqlQuery += dto.m_Uuid.toString(QUuid::WithoutBraces);
#define COLUMN(name, type) sqlQuery += mappings::mapping<type>::toDb(dto.m_##name);
#define VARIANT_COLUMN(name) sqlQuery += dto.m_##name.toString();
#define FOREIGN_KEY(column, foreign_table, foreign_column) qDebug() << "TODO: Add shared_ptr from " << #column << " to " << #foreign_table << ":" << #foreign_column;
#define ENDTABLE() return sqlQuery; }
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef VARIANT_COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

}

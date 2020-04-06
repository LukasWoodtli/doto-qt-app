#pragma once
#include <QString>
#include <QVariant>
#include <QDate>
#include <QUuid>

namespace db {

#define TABLE(name) struct name##_DTO { explicit name##_DTO() \
    : m_Uuid(QUuid::createUuid()) {}; \
    const QUuid m_Uuid;
#define COLUMN(name, type, ...) type m_##name;
#define FOREIGN_KEY(column, foreign_table, foreign_column)
#define ENDTABLE() };
#include "internal/db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE

}
#include <QStringList>
#include <QString>
#include <QDate>

#include <tuple>

#include "data_transfer_objects.h"
#include "database_schema.h"
#include "mappings.h"

namespace db::internal {

QStringList schemaCreationSql() {
    static const QStringList createTableQuerries = {
#define TABLE(name) { QString("CREATE TABLE IF NOT EXISTS " #name " (GUID TEXT UNIQUE")
#define COLUMN(name, type) + ", " #name " " + mappings::mapping<type>::DB_TYPE
#define VARIANT_COLUMN(name) COLUMN(name, QString)
#define FOREIGN_KEY(column, foreign_table, foreign_column) + ", FOREIGN KEY(" #column ") REFERENCES " #foreign_table "(" #foreign_column ")"
#define ENDTABLE() + QStringLiteral(")") },

#include "db_schema.xdef"

#undef TABLE
#undef COLUMN
#undef VARIANT_COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE
    };

    return createTableQuerries;
}

}



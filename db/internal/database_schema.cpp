#include <QStringList>
#include <QString>
#include <QDate>

#include <tuple>

#include "database_schema.h"



namespace {
namespace mappings {

template <class CPP_TYPE>
struct mapping;

template<>
struct mapping<QString> {
    static const constexpr auto DB_TYPE = "STRING";

    std::string toDb(QString string) const {
        return string.toLatin1().toStdString();
    }
};

template<>
struct mapping<QDate> {
    static const constexpr auto DB_TYPE = "STRING";

    std::string toDb(QDate date) const {
        return date.toString(Qt::ISODate).toLatin1().toStdString();
    }
};

template<>
struct mapping<bool> {
    static const constexpr auto DB_TYPE = "INT";

    std::string toDb(bool value) const {
        if (value)
            return "1";
        else
            return "0";
     }
};

}
}


namespace db::internal {

QStringList createSchemaQuerries() {
    static const QStringList createTableQuerries = {
#define TABLE(name) { QString("CREATE TABLE IF NOT EXISTS " #name " (GUID TEXT UNIQUE")
#define COLUMN(name, type) + ", " #name " " + mappings::mapping<type>::DB_TYPE
#define FOREIGN_KEY(column, foreign_table, foreign_column) + ", FOREIGN KEY(" #column ") REFERENCES " #foreign_table "(" #foreign_column ")"
#define ENDTABLE() + QStringLiteral(")") },
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef FOREIGN_KEY
#undef ENDTABLE
};

    return createTableQuerries;
}

}



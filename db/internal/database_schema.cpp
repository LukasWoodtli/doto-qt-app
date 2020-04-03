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


template<typename COLUMN_TYPE>
struct column {
    using CPP_TYPE = COLUMN_TYPE;
    char const * const NAME;
    //char const * const CONSTRAINT = "";
};

template<typename... COL>
struct table {
    char const * const TABLE_NAME;
    const std::tuple<COL...> COLUMNS;
};


template<typename... TABLES>
using database = std::tuple<TABLES...>;

struct NONE_TYPE {};
}


namespace db::internal {

QStringList createSchemaQuerries() {
    static const QStringList createTableQuerries = {
#define TABLE(name) { QString("CREATE TABLE IF NOT EXISTS " #name " (GUID TEXT UNIQUE")
#define COLUMN(name, type, ...) + ", " #name " " + mappings::mapping<type>::DB_TYPE
#define ENDTABLE() + QStringLiteral(")") },
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef ENDTABLE
    };

    return createTableQuerries;
}

namespace {
namespace generated::columns_ns {
#define TABLE(name) namespace table_##name##_ns {
#define COLUMN(name, type) const constexpr auto column_##name = column<type>{#name}; using column_##name##_type = column<type>;
#define ENDTABLE() }
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef ENDTABLE
}

namespace generated::columns_tuples_ns {
#define TABLE(name) namespace name##tuple_ns { \
    using namespace generated::columns_ns::table_##name##_ns; \
    constexpr const auto column_tuple_##name = std::tuple(
#define COLUMN(name, type) column_##name,
#define ENDTABLE() NONE_TYPE{}); }
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef ENDTABLE
}


namespace generated::tables_ns {
#define TABLE(name) namespace table_##name##_ns { \
                    using namespace generated::columns_ns::table_##name##_ns; \
                    const constexpr auto TABLE_NAME = #name; \
                    const constexpr auto COLUMNS_TUPLE = generated::columns_tuples_ns::name##tuple_ns::column_tuple_##name; \
                    const constexpr auto table_##name = table<
#define COLUMN(name, type) column_##name##_type,
#define ENDTABLE()  NONE_TYPE>{TABLE_NAME, COLUMNS_TUPLE}; }
#include "db_schema.xdef"
#undef TABLE
#undef COLUMN
#undef ENDTABLE
}
}
}



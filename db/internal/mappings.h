#pragma once

namespace db::internal::mappings {

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

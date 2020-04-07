#pragma once

namespace db::internal::mappings {

template<class CPP_TYPE>
struct mapping;

template<>
struct mapping<QString> {
	static const constexpr auto DB_TYPE = "STRING";

	static QString toDb(const QString& string) { return string.toUtf8(); }
};

template<>
struct mapping<QDate> {
	static const constexpr auto DB_TYPE = "STRING";

	static QString toDb(const QDate& date) {
		return date.toString(Qt::ISODate).toUtf8();
	}
};

template<>
struct mapping<bool> {
	static const constexpr auto DB_TYPE = "INT";

	static QString toDb(bool value) {
		if (value)
			return "1";
		else
			return "0";
	}
};

}

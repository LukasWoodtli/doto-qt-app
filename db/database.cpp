#include "database.h"

#include "internal/database_schema.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace {
void assertDbEncodingIsUtf8() {
	QSqlQuery query = QSqlQuery();
	[[maybe_unused]] const auto result = query.exec("PRAGMA encoding");
	Q_ASSERT(result);
	query.next();
	const QString encoding = query.value(0).toString();
	qDebug() << "Database encoding is " << encoding;
	Q_ASSERT(encoding == "UTF-8");
	}
}


namespace db {

DataBase::DataBase(QString dbName, QObject* parent)
  : QObject(parent) {
	m_database = QSqlDatabase::addDatabase("QSQLITE");
	Q_ASSERT(m_database.isValid());
	m_database.setDatabaseName(dbName);
}

void DataBase::createSchema() {

	if (not m_database.open())
		qCritical() << "Error opening database with name "
		            << m_database.databaseName()
		            << " Error: " << m_database.lastError();

	QSqlQuery query = QSqlQuery();
	const auto queryStrings = db::internal::schemaCreationSql();
	qDebug() << "Create DB schema with querries:";
	for (const auto& queryString : queryStrings) {
		qDebug() << queryString;
		[[maybe_unused]] const auto result = query.exec(queryString);
		Q_ASSERT(result);
	}

	assertDbEncodingIsUtf8();
}

}

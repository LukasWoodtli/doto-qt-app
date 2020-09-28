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
  : QObject(parent), m_dataBaseName(dbName) {
	auto database = QSqlDatabase::addDatabase("QSQLITE");
	Q_ASSERT(database.isValid());
	database.setDatabaseName(dbName);
	database.open();

  if (not database.open())
    qCritical() << "Error opening database with name "
                << database.databaseName()
                << " Error: " << database.lastError();
}

DataBase::~DataBase() {
  QSqlDatabase::removeDatabase(m_dataBaseName);
}

void DataBase::createSchema() {
	db::internal::createSchema();

	assertDbEncodingIsUtf8();
}

}

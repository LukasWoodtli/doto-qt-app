#include "data_transfer_objects.h"
#include "database.h"
#include "internal/database_schema.h"

#include <QSharedPointer>
#include <QSqlQuery>
#include <QtTest/QtTest>

class TestDb : public QObject {
	Q_OBJECT
public:
	TestDb() {}
private slots:
	void init() {
		m_db = QSharedPointer<db::DataBase>::create(":memory:", this);
		m_db->createSchema();
	}

	void testTaskTableExists() {
		for (const auto& table : {"TASKS", "TASK_LISTS", "SETTINGS"}) {
			QSqlQuery query = QSqlQuery();
			bool success = query.exec(checkIfTableExistsQuery(table));
			QVERIFY(success);
			query.next();
			QVERIFY(query.value(0).toBool());
		}
	}

	void testSchemaCreationSql() {
		static const QStringList expected = {
		  "CREATE TABLE IF NOT EXISTS TASKS (GUID TEXT UNIQUE , NAME STRING  , "
		  "DONE INT  , DUE_DATE STRING )",
		  "CREATE TABLE IF NOT EXISTS TASK_LISTS (GUID TEXT UNIQUE , TASK STRING , "
		  "FOREIGN KEY(TASK) REFERENCES TASKS(GUID))",
		  "CREATE TABLE IF NOT EXISTS SETTINGS (GUID TEXT UNIQUE , NAME STRING "
		  "UNIQUE , VALUE STRING  , CPP_TYPE STRING )"};
		QCOMPARE(db::internal::schemaCreationSql(), expected);
	}

	void testCreateTaskRecord() {
		db::TASKS_DTO tasksDto{};
		m_db->createRecord(tasksDto);
	}

private:
	static QString checkIfTableExistsQuery(QString tableName) {
		return QStringLiteral(
		         "SELECT name FROM sqlite_master WHERE type='table' AND name='%1';")
		  .arg(tableName);
	}
	QSharedPointer<db::DataBase> m_db;
};

QTEST_MAIN(TestDb)
#include "db_test.moc"

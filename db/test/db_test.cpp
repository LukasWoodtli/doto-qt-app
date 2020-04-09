#include "data_transfer_objects.h"
#include "database.h"
#include "internal/database_schema.h"

#include <QSharedPointer>
#include <QSqlError>
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
		  "CREATE TABLE IF NOT EXISTS TASKS (UUID TEXT UNIQUE, NAME STRING , "
		  "DONE INT , DUE_DATE STRING )",
		  "CREATE TABLE IF NOT EXISTS TASK_LISTS (UUID TEXT UNIQUE, TASK STRING , "
		  "FOREIGN KEY(TASK) REFERENCES TASKS(UUID))",
		  "CREATE TABLE IF NOT EXISTS SETTINGS (UUID TEXT UNIQUE, NAME STRING "
		  "UNIQUE, VALUE STRING , CPP_TYPE STRING )"};
		QCOMPARE(db::internal::schemaCreationSql(), expected);
	}

	void testCreateEmptyTaskRecord() {
		db::TASKS_DTO tasksDto{};
		m_db->createRecord(tasksDto);
		QSqlQuery query = QSqlQuery();
		bool success = query.exec("SELECT COUNT(*) FROM TASKS");
		QVERIFY(success);
		query.next();
		QCOMPARE(query.value(0).toInt(), 1);
	}

	void testCreateTaskRecord_data() {
		QTest::addColumn<QString>("name");
		QTest::addColumn<bool>("done");
		QTest::addColumn<QDate>("due");

		QTest::newRow("unfinished") << "hello" << false << QDate(1980, 7, 7);
		QTest::newRow("done") << "hello2" << true << QDate(2020, 7, 7);
		QTest::newRow("done-same-name") << "hello" << true << QDate(2020, 7, 7);
		QTest::newRow("done-same-fields") << "hello2" << true << QDate(2020, 7, 7);
	}
	void testCreateTaskRecord() {
		QFETCH(QString, name);
		QFETCH(bool, done);
		QFETCH(QDate, due);
		db::TASKS_DTO tasksDto{};
		tasksDto.m_NAME = name;
		tasksDto.m_DONE = done;
		tasksDto.m_DUE_DATE = due;
		m_db->createRecord(tasksDto);
		QSqlQuery query = QSqlQuery();
		const auto querySql =
		  QString("SELECT * FROM TASKS WHERE UUID='%1'")
		    .arg(tasksDto.m_Uuid.toString(QUuid::WithoutBraces));
		qDebug() << querySql;
		bool success = query.exec(querySql);
		qDebug() << query.lastError();
		QVERIFY(success);
		query.next();
		QCOMPARE(query.value(0).toString(),
		         tasksDto.m_Uuid.toString(QUuid::WithoutBraces));
		QCOMPARE(query.value(1).toString(), name);
		QCOMPARE(query.value(2).toBool(), done);
		QCOMPARE(query.value(3).toDate(), due);
	}

	void testReadTaskRecord_data() {
		QTest::addColumn<QString>("name");
		QTest::addColumn<bool>("done");
		QTest::addColumn<QDate>("due");

		QTest::newRow("unfinished") << "hello" << false << QDate(1980, 7, 7);
		QTest::newRow("done") << "hello2" << true << QDate(2020, 7, 7);
	}

	void testReadTaskRecord() {
		QFETCH(QString, name);
		QFETCH(bool, done);
		QFETCH(QDate, due);
		db::TASKS_DTO tasksDto{};
		tasksDto.m_NAME = name;
		tasksDto.m_DONE = done;
		tasksDto.m_DUE_DATE = due;
		m_db->createRecord(tasksDto);

		db::TASKS_DTO tasksDtoFromDb =
		  m_db->readRecord<db::TASKS_DTO>(tasksDto.m_Uuid);
		QCOMPARE(tasksDtoFromDb.m_Uuid, tasksDto.m_Uuid);
		QCOMPARE(tasksDtoFromDb.m_NAME, tasksDto.m_NAME);
		QCOMPARE(tasksDtoFromDb.m_DONE, tasksDto.m_DONE);
		QCOMPARE(tasksDtoFromDb.m_DUE_DATE, tasksDto.m_DUE_DATE);
	}

	void testReadUnavailableTaskRecord() {
		const auto uuid = QUuid::createUuid();
		db::TASKS_DTO tasksDtoFromDb = m_db->readRecord<db::TASKS_DTO>(uuid);
		QCOMPARE(tasksDtoFromDb.m_Uuid, QUuid());
	}

	void testUpdateTaskRecord_data() {
		QTest::addColumn<QString>("original_name");
		QTest::addColumn<bool>("original_done");
		QTest::addColumn<QDate>("original_due");

		QTest::addColumn<QString>("new_name");
		QTest::addColumn<bool>("new_done");
		QTest::addColumn<QDate>("new_due");

		QTest::newRow("task1") << "One" << false << QDate(1980, 7, 7) << "One"
		                       << true << QDate(1980, 7, 7);
		QTest::newRow("task2") << "Two" << true << QDate(2020, 7, 7) << "Three"
		                       << true << QDate(2020, 7, 7);
		QTest::newRow("task3") << "Four" << false << QDate(1970, 7, 7) << "Foure"
		                       << true << QDate(2020, 7, 7);
	}

	void testUpdateTaskRecord() {
		QFETCH(QString, original_name);
		QFETCH(bool, original_done);
		QFETCH(QDate, original_due);

		QFETCH(QString, new_name);
		QFETCH(bool, new_done);
		QFETCH(QDate, new_due);

		db::TASKS_DTO tasksDto{};
		tasksDto.m_NAME = original_name;
		tasksDto.m_DONE = original_done;
		tasksDto.m_DUE_DATE = original_due;
		m_db->createRecord(tasksDto);
		m_db->readRecord<db::TASKS_DTO>(tasksDto.m_Uuid);

		tasksDto.m_NAME = new_name;
		tasksDto.m_DONE = new_done;
		tasksDto.m_DUE_DATE = new_due;
		m_db->updateRecord(tasksDto);

		db::TASKS_DTO tasksDtoFromDb =
		  m_db->readRecord<db::TASKS_DTO>(tasksDto.m_Uuid);
		QCOMPARE(tasksDtoFromDb.m_Uuid, tasksDto.m_Uuid);
		QCOMPARE(tasksDtoFromDb.m_NAME, tasksDto.m_NAME);
		QCOMPARE(tasksDtoFromDb.m_DONE, tasksDto.m_DONE);
		QCOMPARE(tasksDtoFromDb.m_DUE_DATE, tasksDto.m_DUE_DATE);
	}

	void testDeleteTaskRecord_data() {
		QTest::addColumn<QString>("name");
		QTest::addColumn<bool>("done");
		QTest::addColumn<QDate>("due");

		QTest::newRow("unfinished") << "Task1" << false << QDate(1980, 7, 7);
		QTest::newRow("done") << "Task2" << true << QDate(2020, 7, 7);
	}

	void testDeleteTaskRecord() {
		QFETCH(QString, name);
		QFETCH(bool, done);
		QFETCH(QDate, due);
		db::TASKS_DTO tasksDto{};
		tasksDto.m_NAME = name;
		tasksDto.m_DONE = done;
		tasksDto.m_DUE_DATE = due;
		m_db->createRecord(tasksDto);

		m_db->deleteRecord<db::TASKS_DTO>(tasksDto.m_Uuid);

		QSqlQuery query = QSqlQuery();
		const auto querySql =
		  QString("SELECT * FROM TASKS WHERE UUID='%1'")
		    .arg(tasksDto.m_Uuid.toString(QUuid::WithoutBraces));
		qDebug() << querySql;
		bool success = query.exec(querySql);
		qDebug() << query.lastError();
		QVERIFY(success);
		query.next();
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

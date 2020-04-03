#include "database.h"
#include "internal/database_schema.h"

#include <QtTest/QtTest>
#include <QSqlQuery>



class TestDb: public QObject
{
    Q_OBJECT
public:
    TestDb() : m_db(":memory:", this) {}
private slots:
    void initTestCase() {
        m_db.createSchema();
    }

    void testTaskTableExists() {
        QSqlQuery query = QSqlQuery();
        bool success = query.exec(checkIfTableExistsQuery("TASKS"));
        QVERIFY(success);
        query.next();
        QVERIFY(query.value(0).toBool());
    }
    void test() {
        static const QStringList expected = {"CREATE TABLE IF NOT EXISTS TASKS (GUID TEXT UNIQUE, NAME STRING, DONE INT, DUE_DATE STRING)"};
        QCOMPARE(db::internal::createSchemaQuerries(), expected);
    }

private:
    static QString checkIfTableExistsQuery(QString tableName) {
        return QStringLiteral("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(tableName);
    }
    db::DataBase m_db;

};

QTEST_MAIN(TestDb)
#include "db_test.moc"

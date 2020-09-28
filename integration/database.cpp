#include "database.h"

#include <approval-tests/ApprovalTests.hpp>
#include <catch2/catch.hpp>
#include <QFile>

#include <iostream>

TEST_CASE("Data Base")
{
	const constexpr auto DB_FILE_PATH = TEST_WORKING_DIR "/integration_test.db";
  db::DataBase database{DB_FILE_PATH};

  auto dbFile = QFile(DB_FILE_PATH);
	const auto fileRemoved = dbFile.remove();
	REQUIRE(fileRemoved);
}
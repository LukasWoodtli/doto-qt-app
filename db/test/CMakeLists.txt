
add_executable(db_test)

set(CMAKE_AUTOMOC ON)

target_sources(db_test PRIVATE db_test.cpp)

target_link_libraries(db_test PRIVATE db Qt5::Core Qt5::Test Qt5::Sql)

add_test(db_test db_test)

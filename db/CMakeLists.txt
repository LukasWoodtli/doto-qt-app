

add_library(db)

target_sources(db PRIVATE database.h database.cpp internal/database_schema.h internal/database_schema.cpp internal/db_schema.xdef)
target_include_directories(db PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(db PRIVATE "-save-temps")
target_link_libraries(db PRIVATE Qt5::Core Qt5::Sql)

add_subdirectory(test)

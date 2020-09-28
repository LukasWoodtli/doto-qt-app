
add_executable(DbIntegrationTest)
target_sources(DbIntegrationTest PRIVATE main.cpp database.cpp)
target_compile_definitions(DbIntegrationTest PRIVATE -DTEST_WORKING_DIR="${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(DbIntegrationTest PRIVATE IntegrationTestLibs db)

add_custom_target(run-integration-test
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/DbIntegrationTest
        DEPENDS DbIntegrationTest
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

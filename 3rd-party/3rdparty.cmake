
add_library(IntegrationTestLibs INTERFACE)

target_include_directories(IntegrationTestLibs SYSTEM INTERFACE ${CMAKE_SOURCE_DIR}/3rd-party)

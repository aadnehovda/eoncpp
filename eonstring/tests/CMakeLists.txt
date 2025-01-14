cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(EonStringTests LANGUAGES CXX)
message(STATUS " - EonStringTests")

set(STRINGTEST_HEADERS
	Regression.h
)
set(STRINGTEST_SOURCES
	Regression.cpp
)

add_executable(EonStringTests ${STRINGTEST_HEADERS} ${STRINGTEST_SOURCES})
set_target_properties(
	EonStringTests
	PROPERTIES
		OUTPUT_NAME_DEBUG eon_string_tests
		OUTPUT_NAME_RELEASE eon_string_tests
)
set_property(TARGET EonStringTests PROPERTY CXX_STANDARD 17)

target_link_libraries(EonStringTests PUBLIC EonString EonTest EonRegex)
	
install(TARGETS EonStringTests
	RUNTIME DESTINATION "${EON_INSTALL_DIR}/${EON_TESTS_DIR}"
)

target_include_directories(EonStringTests PUBLIC "${EON_SOURCES_DIR}" "${EON_INSTALL_DIR}/${EON_INCLUDE_DIR}")

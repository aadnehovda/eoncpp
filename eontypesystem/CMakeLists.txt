cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(EonTypeSystem LANGUAGES CXX)
message(STATUS " - EonTypeSystem")

set(TYPESYSTEM_HEADERS
	Definitions.h
	TypeTuple.h
	Action.h
	PrimitiveTypes.h
	BitsTypes.h
	IntTypes.h
	FloatTypes.h
	TypeMap.h
	Tuple.h
	Expression.h
	Attribute.h
	TupleFactory.h
	Operators.h
	StringActions.h
)
set(TYPESYSTEM_SOURCES
	TypeTuple.cpp
	PrimitiveTypes.cpp
	TypeMap.cpp
	Tuple.cpp
	Expression.cpp
	Attribute.cpp
	TupleFactory.cpp
	Operators.cpp
	StringActions.cpp
)

if(WIN32)
	set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()

add_library(EonTypeSystem STATIC ${TYPESYSTEM_HEADERS} ${TYPESYSTEM_SOURCES})
set_target_properties(
	EonTypeSystem
	PROPERTIES
		OUTPUT_NAME_DEBUG eon_typesystem
		OUTPUT_NAME_RELEASE eon_typesystem
)
set_property(TARGET EonTypeSystem PROPERTY CXX_STANDARD 17)

target_link_libraries(EonTypeSystem PUBLIC EonString EonExcept EonRegex EonSource EonFileSys EonContainers EonTokenizer)
target_include_directories(EonTypeSystem PUBLIC "${EON_SOURCES_DIR}"
	"${EON_INSTALL_DIR}/${EON_INCLUDE_DIR}"
)

install(TARGETS EonTypeSystem
	LIBRARY DESTINATION "${EON_INSTALL_DIR}/${EON_LIBRARY_DIR}"
	ARCHIVE DESTINATION "${EON_INSTALL_DIR}/${EON_LIBRARY_DIR}"
)
install(FILES ${TYPESYSTEM_HEADERS} DESTINATION
	"${EON_INSTALL_DIR}/${EON_INCLUDE_DIR}/eontypesystem"
)
if(WIN32)
	install(FILES ${CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY}/Debug/eon_typesystem${CMAKE_DEBUG_POSTFIX}.pdb
		DESTINATION "${EON_INSTALL_DIR}/${EON_LIBRARY_DIR}" OPTIONAL)
endif()

if(EON_BUILD_TESTS)
	add_subdirectory(tests)
endif()

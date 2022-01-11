cmake_minimum_required(VERSION 3.0.0)
project(MetatagEX VERSION 0.1.0)

set(THREADS_PREFER_PTHREAD_FLAG ON)

include(CTest)
include(FindXercesC)
find_package(XercesC REQUIRED)
find_package(LibArchive REQUIRED)
if (UNIX)
	add_compile_definitions(OS_UNIX)
endif (UNIX)
find_package(RapidJSON REQUIRED)
find_package(Threads REQUIRED)
include_directories( ${XercesC_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIRS} ${LibArchive_INCLUDE_DIRS} )
enable_testing()

add_executable(MetatagEX main.cpp MetatagEX.cpp CommandParser.cpp extend/XmlParser.cpp util/Util.cpp)
target_link_libraries(${PROJECT_NAME} ${XercesC_LIBRARY} ${RAPIDJSON_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${LibArchive_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

cmake_minimum_required(VERSION 3.10)
project(LDView)

set(OpenGL_GL_PREFERENCE LEGACY)

find_package(QT NAMES Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets OpenGL PrintSupport LinguistTools Network REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(Threads REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(ZLIB REQUIRED)
message(STATUS "Qt:   ${QT_VERSION}")
message(STATUS "PNG:  ${PNG_VERSION_STRING}")
message(STATUS "JPEG: ${JPEG_VERSION}")
message(STATUS "ZLIB: ${ZLIB_VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_VERBOSE_MAKEFILE OK)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-D_NO_BOOST -D_QT -DHAVE_MINIZIP -DUSE_CPP11)
add_definitions(${Qt${QT_VERSION_MAJOR}Gui_DEFINITIONS})
add_definitions(${Qt${QT_VERSION_MAJOR}PrintSupport_DEFINITIONS})
add_definitions(${Qt${QT_VERSION_MAJOR}OpenGL_DEFINITIONS})

include_directories(
	${CMAKE_SOURCE_DIR}/..
	${CMAKE_SOURCE_DIR}/.ui
	${Qt${QT_VERSION_MAJOR}OpenGL_INCLUDE_DIRS}
	${Qt${QT_VERSION_MAJOR}Network_INCLUDE_DIRS}
	${Qt${QT_VERSION_MAJOR}PrintSupport_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIRS}
)

file(GLOB LDViewSources *.cpp *.h *.ui resources.qrc)
qt5_add_resources(LDViewSources resources.qrc)
add_executable(LDView ${LDViewSources})


file(GLOB ts_files *.ts)
qt5_add_translation(qm_files ${ts_files})

target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../TRE -lTRE-qt${QT_VERSION_MAJOR})
target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../LDLib -lLDraw-qt${QT_VERSION_MAJOR})
target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../TCFoundation -lTCFoundation-qt${QT_VERSION_MAJOR})
target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../LDExporter -lLDExporter-qt${QT_VERSION_MAJOR})
target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../LDLoader -lLDLoader-qt${QT_VERSION_MAJOR})
target_link_libraries(LDView -lgl2ps -ltinyxml -lminizip -l3ds-64 -L${CMAKE_SOURCE_DIR}/../lib)

target_link_libraries(LDView Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(LDView Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(LDView Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(LDView ${QT_LIBRARIES})
target_link_libraries(LDView Qt${QT_VERSION_MAJOR}::OpenGL ${OPENGL_LIBRARIES})
target_link_libraries(LDView ${X11_X11_LIB})
target_link_libraries(LDView ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(LDView ${PNG_LIBRARIES} ${JPEG_LIBRARIES})

add_dependencies(LDView TRE)
add_dependencies(LDView TCFoundation)
add_dependencies(LDView LDLib)
add_dependencies(LDView LDLoader)
add_dependencies(LDView LDExporter)

include(ExternalProject)

ExternalProject_Add(TRE
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/../TRE
	CONFIGURE_COMMAND ""
	DOWNLOAD_COMMAND ""
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE 1
	BUILD_COMMAND make POSTFIX=-qt${QT_VERSION_MAJOR} TESTING=-D_NO_BOOST\ -DHAVE_MINIZIP\ -DUSE_CPP11
	INSTALL_COMMAND ""
)

ExternalProject_Add(TCFoundation
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/../TCFoundation
	CONFIGURE_COMMAND ""
	DOWNLOAD_COMMAND ""
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE 1
	BUILD_COMMAND make POSTFIX=-qt${QT_VERSION_MAJOR} TESTING=-D_NO_BOOST\ -D_QT\ -I/usr/include/qt5\ -DHAVE_MINIZIP\ -DUSE_CPP11
	INSTALL_COMMAND ""
)

ExternalProject_Add(LDExporter
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/../LDExporter
	CONFIGURE_COMMAND ""
	DOWNLOAD_COMMAND ""
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE 1
	BUILD_COMMAND make POSTFIX=-qt${QT_VERSION_MAJOR} TESTING=-D_NO_BOOST\ -DHAVE_MINIZIP\ -DUSE_CPP11
	INSTALL_COMMAND ""
)

ExternalProject_Add(LDLoader
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/../LDLoader
	CONFIGURE_COMMAND ""
	DOWNLOAD_COMMAND ""
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE 1
	BUILD_COMMAND make POSTFIX=-qt${QT_VERSION_MAJOR} TESTING=-D_NO_BOOST\ -D_QT\ -DHAVE_MINIZIP\ -DUSE_CPP11
	INSTALL_COMMAND ""
)

ExternalProject_Add(LDLib
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/../LDLib
	CONFIGURE_COMMAND ""
	DOWNLOAD_COMMAND ""
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE 1
	BUILD_COMMAND make POSTFIX=-qt${QT_VERSION_MAJOR} TESTING=-D_NO_BOOST\ -DHAVE_MINIZIP\ -DUSE_CPP11
	INSTALL_COMMAND ""
)

get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
   message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

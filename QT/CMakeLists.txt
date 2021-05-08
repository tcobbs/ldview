cmake_minimum_required(VERSION 3.10)
project(LDView)

set(OpenGL_GL_PREFERENCE LEGACY)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets OpenGL PrintSupport LinguistTools REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(Threads REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
message(STATUS "Qt:   ${QT_VERSION}")
message(STATUS "PNG:  ${PNG_VERSION_STRING}")
message(STATUS "JPEG: ${JPEG_VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-D_NO_BOOST -D_QT)
add_definitions(${Qt${QT_VERSION_MAJOR}Gui_DEFINITIONS})
add_definitions(${Qt${QT_VERSION_MAJOR}rintSupport_DEFINITIONS})
add_definitions(${Qt${QT_VERSION_MAJOR}OpenGL_DEFINITIONS})

include_directories(
	${CMAKE_SOURCE_DIR}/..
	${CMAKE_SOURCE_DIR}/.ui
	${Qt${QT_VERSION_MAJOR}OpenGL_INCLUDE_DIRS}
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
target_link_libraries(LDView -lgl2ps -ltinyxml -l3ds-64 -L${CMAKE_SOURCE_DIR}/../lib)

target_link_libraries(LDView Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(LDView Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(LDView ${QT_LIBRARIES})
target_link_libraries(LDView Qt${QT_VERSION_MAJOR}::OpenGL ${OPENGL_LIBRARIES})
target_link_libraries(LDView ${X11_X11_LIB})
target_link_libraries(LDView ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(LDView ${PNG_LIBRARIES} ${JPEG_LIBRARIES})

get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
#   message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

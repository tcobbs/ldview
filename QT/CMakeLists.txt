cmake_minimum_required(VERSION 3.10)
project(LDView)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(X11)
find_package(Threads REQUIRED)
find_package(PNG)
find_package(JPEG)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-D_NO_BOOST -D_QT)
add_definitions(${Qt5Gui_DEFINITIONS})
add_definitions(${Qt5PrintSupport_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})

include_directories(.. .ui ${Qt5OpenGL_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS})

file(GLOB LDViewSources *.cpp)
file(GLOB ui_files ?*.ui)
add_executable(LDView ${LDViewSources} ${ui_files})
qt5_wrap_ui(UI_SRC ${ui_files})

file(GLOB ts_files *.ts)
qt5_add_translation(qm_files ${ts_files})

target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../TRE -lTRE-qt5)
target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../LDLib -lLDraw-qt5)
target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../TCFoundation -lTCFoundation-qt5)
target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../LDExporter -lLDExporter-qt5)
target_link_libraries(LDView -L${CMAKE_SOURCE_DIR}/../LDLoader -lLDLoader-qt5)
target_link_libraries(LDView -lgl2ps -ltinyxml -l3ds-64 -L${CMAKE_SOURCE_DIR}/../lib)

target_link_libraries(LDView Qt5::Widgets)
target_link_libraries(LDView ${Qt5PrintSupport_LIBRARIES})
target_link_libraries(LDView ${QT_LIBRARIES})
target_link_libraries(LDView Qt5::OpenGL ${OPENGL_LIBRARIES})
target_link_libraries(LDView ${X11_X11_LIB})
target_link_libraries(LDView ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(LDView ${PNG_LIBRARIES} ${JPEG_LIBRARIES})

get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
   message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

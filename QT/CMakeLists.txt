cmake_minimum_required(VERSION 3.10)
project(LDView)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
include_directories(..)
add_executable(
LDView
AlertHandler.cpp
LDViewBoundingBox.cpp
LDViewCustomizeToolbar.cpp
LDViewErrors.cpp
LDViewExportOption.cpp
LDViewExtraDir.cpp
LDViewJpegOptions.cpp
LDViewLatitudeLongitude.cpp
LDViewMainWindow.cpp
LDViewModelTree.cpp
LDViewMpdModel.cpp
LDViewPartList.cpp
LDViewSnapshotSettings.cpp
misc.cpp
ModelViewerWidget.cpp
Preferences.cpp
qrc_resources.cpp
QTMain.cpp
SnapshotAlertHandler.cpp
SnapshotTaker.cpp

)
target_link_libraries(LDView Qt5::Widgets)
target_link_libraries(LDView Qt5:OpenGL)

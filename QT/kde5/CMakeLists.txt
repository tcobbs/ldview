project(ldviewthumbnail)
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(KF5 REQUIRED COMPONENTS KIO CoreAddons)

kcoreaddons_add_plugin(ldviewthumbnail
   SOURCES ldviewthumbnailcreator.cpp
   INSTALL_NAMESPACE "kf5/thumbcreator"
)

target_link_libraries(ldviewthumbnail PRIVATE KF5::KIOGui KF5::CoreAddons)

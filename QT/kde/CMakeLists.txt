cmake_minimum_required(VERSION 2.6)
project(LDViewThumbnail)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(LDViewThumbnail_SRCS ldviewthumbnailcreator.cpp)

kde4_add_plugin(ldviewthumbnail ${LDViewThumbnail_SRCS})

target_link_libraries(ldviewthumbnail ${KDE4_KIO_LIBS} )

install(TARGETS ldviewthumbnail DESTINATION ${PLUGIN_INSTALL_DIR} )

install(FILES  ldviewthumbnailcreator.desktop DESTINATION ${SERVICES_INSTALL_DIR})


# LDLib/CMakeLists.txt

set(LDLIB_SOURCES
    LDConsoleAlertHandler.cpp
    LDHtmlInventory.cpp
    LDInputHandler.cpp
    LDLibraryUpdateInfo.cpp
    LDLibraryUpdater.cpp # Requires C++11 and Boost for some platform in VS project, ensure defines match
    LDModelParser.cpp
    LDModelTree.cpp
    LDObiInfo.cpp
    LDPartCount.cpp
    LDPartsList.cpp
    LDPreferences.cpp
    LDrawModelViewer.cpp
    LDSnapshotTaker.cpp
    LDViewPoint.cpp
)

set(LDLIB_HEADERS
    LDConsoleAlertHandler.h
    LDHtmlInventory.h
    LDInputHandler.h
    LDLibraryUpdateInfo.h
    LDLibraryUpdater.h
    LDModelParser.h
    LDModelTree.h
    LDObiInfo.h
    LDPartCount.h
    LDPartsList.h
    LDPreferences.h
    LDrawModelViewer.h
    LDSnapshotTaker.h
    LDUserDefaultsKeys.h
    LDViewPoint.h
)

add_library(ldview_ldlib_lib STATIC
    ${LDLIB_SOURCES}
    ${LDLIB_HEADERS}
)

target_compile_definitions(ldview_ldlib_lib PRIVATE
    USE_CPP11
    HAVE_MINIZIP
    _TC_STATIC
    EXPORT_3DS
    $<$<CONFIG:Debug>:_LOG_PERFORMANCE>
    $<$<CONFIG:Release>:_NO_BOOST>
    $<$<BOOL:${LDVIEW_USE_BOOST}>:USE_BOOST_LIBS>
)

if(MSVC)
    # Reflects MultiByte from vcxproj
    target_compile_definitions(ldview_ldlib_lib PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        _LIB
    )
else()
    target_compile_options(ldview_ldlib_lib PRIVATE -fPIC)
endif()

target_include_directories(ldview_ldlib_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}" # For its own headers
    "${CMAKE_SOURCE_DIR}"         # For includes like "TCFoundation/..."
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/3rdParty/gl2ps" # As per vcxproj
)

# Dependencies
target_link_libraries(ldview_ldlib_lib PRIVATE
    ldview_tcfoundation_lib
    ldview_ldloader_lib
    ldview_tre_lib
    ldview_ldexporter_lib
)

if(TARGET ldview_gl2ps_lib) # From bundled 3rdParty or system
    target_link_libraries(ldview_ldlib_lib PRIVATE ldview_gl2ps_lib)
endif()

# If LDExporter uses TinyXML in its public headers, LDLib might need its includes too.
# This can be handled by ldview_ldexporter_lib having PUBLIC include to tinyxml if needed.
# For now, direct dependency on tinyxml for LDLib is not assumed unless sources show it.

if(LDVIEW_USE_BOOST AND Boost_FOUND)
    target_include_directories(ldview_ldlib_lib PRIVATE ${Boost_INCLUDE_DIRS})
    # Link Boost::thread, Boost::date_time if LDLibraryUpdater specifically needs compiled Boost parts
    # The vcxproj for LDLib doesn't list boost libs, so it might be header-only usage or linked by main exe
    # For now, just include dirs. If linking errors appear for Boost symbols, add:
    # target_link_libraries(ldview_ldlib_lib PRIVATE Boost::thread Boost::date_time) # Example
endif()

message(STATUS "Configuring LDLib library target: ldview_ldlib_lib")

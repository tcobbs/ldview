# CUI/CMakeLists.txt

set(CUI_SOURCES
    CUIColorButton.cpp
    CUIDialog.cpp
    CUIOGLWindow.cpp
    CUIPropertySheet.cpp
    CUIScaler.cpp
    CUISubWindowInfo.cpp
    CUIThemes.cpp
    CUIWindow.cpp
    CUIWindowResizer.cpp
)

set(CUI_HEADERS
    CUIColorButton.h
    CUIDefines.h
    CUIDialog.h
    CUIModuleHolder.h
    CUIOGLWindow.h
    CUIPropertySheet.h
    CUIScaler.h
    CUISubWindowInfo.h
    CUIThemes.h
    CUIWindow.h
    CUIWindowResizer.h
)

add_library(ldview_cui_lib STATIC
    ${CUI_SOURCES}
    ${CUI_HEADERS}
)

target_compile_definitions(ldview_cui_lib PRIVATE
    USE_CPP11
    HAVE_MINIZIP # Common define
    _TC_STATIC
    _BUILDING_CUI_LIB # Specific to CUI
    $<$<CONFIG:Release>:_NO_BOOST> # Assuming this pattern, though not in CUI.vcxproj explicitly
    $<$<BOOL:${LDVIEW_USE_BOOST}>:USE_BOOST_LIBS>
)

# CUI is Windows-specific and uses Unicode
if(MSVC)
    target_compile_definitions(ldview_cui_lib PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        _LIB
        UNICODE # Key for CUI
        _UNICODE # Key for CUI
        _WIN32_WINDOWS=0x0410 # As per vcxproj
    )
else()
    # This library is likely Windows MSVC only.
    # To prevent trying to build it on other platforms if added to root CMakeLists unconditionally:
    message(WARNING "CUI library is typically Windows MSVC only and might not build on this platform.")
    # We could make its addition to the build conditional in the root CMakeLists.txt using if(WIN32 AND MSVC)
endif()

target_include_directories(ldview_cui_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}" # For its own headers
    "${CMAKE_SOURCE_DIR}"         # For includes like "TCFoundation/..."
)

# Dependencies
target_link_libraries(ldview_cui_lib PRIVATE
    ldview_tcfoundation_lib
)

if(LDVIEW_USE_BOOST AND Boost_FOUND)
    target_include_directories(ldview_cui_lib PRIVATE ${Boost_INCLUDE_DIRS})
endif()

message(STATUS "Configuring CUI library target: ldview_cui_lib (Windows Unicode)")

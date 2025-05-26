cmake_minimum_required(VERSION 3.15)
project(LDView LANGUAGES C CXX VERSION 4.6.1) # Version from LDView.vcxproj

# --- Global Settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Prefer explicit language features

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Define paths for prebuilt libraries on Windows
set(LDVIEW_LIB_DIR_WIN32 "${CMAKE_SOURCE_DIR}/lib")
set(LDVIEW_LIB_DIR_X64 "${CMAKE_SOURCE_DIR}/lib/x64")

# Default build type if none specified (e.g., Release or Debug)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type: Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()

# --- Platform-Specific Settings ---
if(MSVC)
    add_compile_options(/W4 /MP) # /W4 for high warning level, /MP for parallel builds
    # Add definition to use static runtime for Release, DLL for Debug (matching vcxproj)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>")
    # Target older Windows for broader compatibility (from vcxproj v141_xp toolset)
    # For v142+ this might not be strictly necessary in this form for XP,
    # but we'll keep the spirit of targeting older Windows.
    # CMAKE_SYSTEM_VERSION can be set, or specific defines.
    # For now, _WIN32_WINNT is often handled by Windows SDK headers based on toolset.
    # Add _WIN32_WINDOWS=0x0501 for WinXP/Server2003 compatibility
    add_compile_definitions(_WIN32_WINDOWS=0x0501)
    # For v141_xp toolset compatibility (Platform Toolset v141_xp in VS)
    # This is more about the toolset choice in CMake generator than a define.
    # e.g. cmake -G "Visual Studio 16 2019" -T v141_xp
else()
    # Common flags for GCC/Clang
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# --- Options for Building Components ---
option(LDVIEW_BUILD_WINDOWS_UI "Build the native Windows GUI application (MSVC)" ON)
option(LDVIEW_BUILD_QT_UI "Build the Qt-based GUI application" OFF) # Default OFF for initial Windows focus
option(LDVIEW_BUILD_OSMESA "Build the OSMesa off-screen rendering version" OFF) # Default OFF
option(LDVIEW_BUILD_LAUNCHER "Build the Launcher utility" ON) # Usually needed on Windows
option(LDVIEW_BUILD_THUMBS "Build LDViewThumbs Windows shell extension" ON) # If WIN32
option(LDVIEW_BUILD_UNMIRRORSTUDS "Build UnMirrorStuds utility" ON)
option(LDVIEW_BUILD_TRANSLATIONS "Build Translation DLLs (Windows)" ON) # If WIN32
option(LDVIEW_BUILD_LDVLIB_DLL "Build LDVLib DLL (Wrapper library)" OFF) # Optional

option(LDVIEW_USE_BUNDLED_DEPS "Use bundled 3rd-party libraries (minizip, tinyxml, gl2ps)" ON)
option(LDVIEW_USE_BOOST "Use Boost libraries (if found, for specific features like threading - potentially legacy)" OFF)

# --- Include Directories ---
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include" # For zlib.h, png.h etc. if placed there
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty" # For headers of libs in 3rdParty
)

# --- External Dependencies (Initial Setup) ---
find_package(OpenGL REQUIRED)
find_package(GLU REQUIRED) # GLU is often separate

# ZLIB
if(WIN32 AND MSVC)
    # Using prebuilt zlib libraries for Windows
    set(ZLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include") # Assuming zlib.h and zconf.h are here
    if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
        set(ZLIB_LIBRARY "${LDVIEW_LIB_DIR_X64}/zlib-vs2019.lib")
    else() # Win32
        set(ZLIB_LIBRARY "${LDVIEW_LIB_DIR_WIN32}/zlib-vs2017.lib")
    endif()
    if(NOT EXISTS "${ZLIB_INCLUDE_DIR}/zlib.h" OR NOT EXISTS "${ZLIB_LIBRARY}")
        message(FATAL_ERROR "Bundled ZLIB headers or library not found for MSVC at ${ZLIB_INCLUDE_DIR} or ${ZLIB_LIBRARY}")
    endif()
    add_library(ZLIB::ZLIB UNKNOWN IMPORTED)
    set_target_properties(ZLIB::ZLIB PROPERTIES
        IMPORTED_LOCATION "${ZLIB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIR}"
    )
    message(STATUS "Using bundled ZLIB for Windows: ${ZLIB_LIBRARY}")
else()
    find_package(ZLIB REQUIRED) # For Linux/macOS
endif()

# libpng
if(WIN32 AND MSVC)
    set(PNG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/3rdParty/libpng") # Main headers like png.h
    # vcxproj also includes ../3rdParty/libpng/Windows for pnglibconf.h
    # target_include_directories for targets using PNG should add this specific path if needed.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
        set(PNG_LIBRARY "${LDVIEW_LIB_DIR_X64}/libpng16-vs2019.lib")
    else() # Win32
        set(PNG_LIBRARY "${LDVIEW_LIB_DIR_WIN32}/libpng16-vs2017.lib")
    endif()
    if(NOT EXISTS "${PNG_INCLUDE_DIR}/png.h" OR NOT EXISTS "${PNG_LIBRARY}")
        message(FATAL_ERROR "Bundled libpng headers or library not found for MSVC at ${PNG_INCLUDE_DIR} or ${PNG_LIBRARY}")
    endif()
    add_library(PNG::PNG UNKNOWN IMPORTED)
    set_target_properties(PNG::PNG PROPERTIES
        IMPORTED_LOCATION "${PNG_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "ZLIB::ZLIB" # PNG depends on ZLIB
    )
    message(STATUS "Using bundled libpng for Windows: ${PNG_LIBRARY}")
else()
    find_package(PNG REQUIRED) # For Linux/macOS
endif()

# libjpeg
if(WIN32 AND MSVC)
    set(JPEG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/3rdParty/libjpeg") # Main headers like jpeglib.h
    # vcxproj also includes ../3rdParty/libjpeg/Windows for jconfig.h
    if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
        set(JPEG_LIBRARY "${LDVIEW_LIB_DIR_X64}/libjpeg-vs2019.lib")
    else() # Win32
        set(JPEG_LIBRARY "${LDVIEW_LIB_DIR_WIN32}/libjpeg-vs2017.lib")
    endif()
    if(NOT EXISTS "${JPEG_INCLUDE_DIR}/jpeglib.h" OR NOT EXISTS "${JPEG_LIBRARY}")
        message(FATAL_ERROR "Bundled libjpeg headers or library not found for MSVC at ${JPEG_INCLUDE_DIR} or ${JPEG_LIBRARY}")
    endif()
    add_library(JPEG::JPEG UNKNOWN IMPORTED)
    set_target_properties(JPEG::JPEG PROPERTIES
        IMPORTED_LOCATION "${JPEG_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${JPEG_INCLUDE_DIR}"
    )
    message(STATUS "Using bundled libjpeg for Windows: ${JPEG_LIBRARY}")
else()
    find_package(JPEG REQUIRED) # For Linux/macOS
endif()

option(LDVIEW_ENABLE_3DS_EXPORT "Enable 3DS Export functionality" ON)

if(LDVIEW_ENABLE_3DS_EXPORT)
    if(WIN32 AND MSVC)
        set(LIB3DS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/3rdParty/lib3ds")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
            set(LIB3DS_LIBRARY "${LDVIEW_LIB_DIR_X64}/lib3ds-vs2019.lib")
        else() # Win32
            set(LIB3DS_LIBRARY "${LDVIEW_LIB_DIR_WIN32}/lib3ds-vs2017.lib")
        endif()
        if(NOT EXISTS "${LIB3DS_INCLUDE_DIR}/lib3ds.h" OR NOT EXISTS "${LIB3DS_LIBRARY}")
            message(WARNING "Bundled lib3ds headers or library not found for MSVC. 3DS Export will be disabled. Searched: ${LIB3DS_INCLUDE_DIR}, ${LIB3DS_LIBRARY}")
            set(LDVIEW_ENABLE_3DS_EXPORT OFF CACHE BOOL "Disable 3DS export due to missing libs" FORCE)
        else()
            add_library(ldview_lib3ds UNKNOWN IMPORTED) # Using a non-namespaced target for consistency with how it might be found on other systems.
            set_target_properties(ldview_lib3ds PROPERTIES
                IMPORTED_LOCATION "${LIB3DS_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${LIB3DS_INCLUDE_DIR}"
            )
            message(STATUS "Using bundled lib3ds for Windows: ${LIB3DS_LIBRARY}")
            add_compile_definitions(EXPORT_3DS)
        endif()
    else()
        # For Linux/macOS - try to find system lib3ds or from prebuilt paths
        # This might involve find_library(LIB3DS_LIBRARY NAMES 3ds lib3ds PATHS ...)
        # and find_path(LIB3DS_INCLUDE_DIR NAMES lib3ds.h PATHS ...).
        # For now, placeholder:
        message(STATUS "lib3ds discovery for non-MSVC platforms needs to be configured if EXPORT_3DS is required.")
        # find_package(Lib3ds) # If a FindLib3ds.cmake module exists or is created.
        # If found, define EXPORT_3DS. If not, LDVIEW_ENABLE_3DS_EXPORT should be set to OFF.
    endif()
endif()
if(NOT LDVIEW_ENABLE_3DS_EXPORT)
    message(STATUS "3DS Export disabled.")
endif()

# Boost Integration
if(LDVIEW_USE_BOOST)
    # For Windows, Boost headers are expected in ./boost/include as per vcxproj
    # Compiled libs (if any) for Debug were in ./boost/lib.
    # For MSVC, prefer using the bundled headers if LDVIEW_USE_BOOST is ON.
    # For other platforms, find_package is more robust.
    if(WIN32 AND MSVC)
        set(Boost_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/boost/include")
        if(EXISTS "${Boost_INCLUDE_DIR}/boost/version.hpp")
            set(Boost_FOUND TRUE)
            # For MSVC Debug, vcxproj linked specific boost libs.
            # This part is complex due to versioned/mangled names and Debug/Release static/dynamic.
            # For now, primarily focus on header includes. If compiled libs are needed by a target,
            # it will fail to link, and we can address finding specific prebuilt boost libs then.
            message(STATUS "Using bundled Boost headers for Windows MSVC from: ${Boost_INCLUDE_DIR}")
        else()
            message(WARNING "LDVIEW_USE_BOOST is ON for MSVC, but Boost headers not found at ${Boost_INCLUDE_DIR}. Boost will be disabled.")
            set(LDVIEW_USE_BOOST OFF CACHE BOOL "Disable Boost due to missing headers" FORCE)
        endif()
    else() # For non-MSVC platforms
        find_package(Boost COMPONENTS thread date_time) # Add other components if identified as needed
        if(Boost_FOUND)
            message(STATUS "Found Boost version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
            add_compile_definitions(USE_BOOST_LIBS) # A generic define if any code needs it
        else()
            message(WARNING "LDVIEW_USE_BOOST is ON, but Boost libraries were not found. Some features might be disabled.")
            set(LDVIEW_USE_BOOST OFF CACHE BOOL "Disable Boost due to missing system libs" FORCE)
        endif()
    endif()
endif()

if(NOT LDVIEW_USE_BOOST)
    add_compile_definitions(_NO_BOOST) # Global define if Boost is not used/found
    message(STATUS "Boost support is disabled (_NO_BOOST defined).")
endif()

# --- Add Subdirectories for Libraries and Executables ---
# Core libraries first
add_subdirectory(TCFoundation)

if(LDVIEW_USE_BUNDLED_DEPS)
    add_subdirectory(3rdParty/minizip)
    add_subdirectory(3rdParty/tinyxml)
    add_subdirectory(3rdParty/gl2ps)
else()
    # Try to find system versions if not using bundled
    # find_package(ZLIB REQUIRED) # minizip has its own zlib functions or can link to system zlib
    # For now, we assume bundled minizip handles its zlib needs internally or links to a system zlib if configured.
    # If minizip requires an explicit ZLIB target, that would be handled within its own CMakeLists.txt
    # or by ensuring ZLIB is found before minizip if it's a system version.
endif()

add_subdirectory(LDLoader)
add_subdirectory(TRE)
add_subdirectory(LDExporter)

if(WIN32 AND MSVC AND LDVIEW_BUILD_WINDOWS_UI)
    add_subdirectory(CUI)
endif()

add_subdirectory(LDLib)

# Windows UI specific
if(LDVIEW_BUILD_WINDOWS_UI AND WIN32 AND MSVC)
    add_subdirectory(src) # For the main Windows LDView.exe
endif()

if(LDVIEW_BUILD_LAUNCHER AND WIN32) # Launcher is Windows specific
    add_subdirectory(Launcher)
endif()

if(LDVIEW_BUILD_THUMBS AND WIN32 AND MSVC)
    # For _AFXDLL, LDViewThumbs seems to use MFC.
    # CMAKE_MFC_FLAG = 2 # Use MFC in a shared DLL
    # This should ideally be set before project() or add_subdirectory if global,
    # or as a target property. For now, _AFXDLL is directly in LDViewThumbs' CMakeLists.
    add_subdirectory(LDViewThumbs)
endif()

if(LDVIEW_BUILD_TRANSLATIONS AND WIN32 AND MSVC) # Translations are Windows DLLs
    add_subdirectory(Translations/EnglishUS)
    # add_subdirectory(Translations/German) # etc. for others later
    # add_subdirectory(Translations/Czech)
    # add_subdirectory(Translations/Hungarian)
    # add_subdirectory(Translations/Italian)
endif()

if(LDVIEW_BUILD_UNMIRRORSTUDS)
    add_subdirectory(UnMirrorStuds)
endif()

# TODO: Add other subdirectories based on options (QT, OSMesa, etc.)
#       and once their CMakeLists.txt are being developed.

message(STATUS "LDView Project Version: ${PROJECT_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build Windows UI: ${LDVIEW_BUILD_WINDOWS_UI}")
message(STATUS "Build Qt UI: ${LDVIEW_BUILD_QT_UI}")

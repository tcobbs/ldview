# 3rdParty/minizip/CMakeLists.txt

set(MINIZIP_SOURCES
    ioapi.c
    mztools.c # Added as it's in Makefile and vcxproj
    unzip.c
    zip.c
)

# Add Windows-specific sources
if(WIN32)
    list(APPEND MINIZIP_SOURCES iowin32.c)
endif()

set(MINIZIP_HEADERS
    crypt.h
    ioapi.h
    iowin32.h # If iowin32.c is included
    mztools.h
    unzip.h
    zip.h
)

add_library(ldview_minizip_lib STATIC
    ${MINIZIP_SOURCES}
    ${MINIZIP_HEADERS}
)

target_include_directories(ldview_minizip_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}" # For its own headers
)

# Common definitions that might be needed by minizip source files
target_compile_definitions(ldview_minizip_lib PRIVATE
    # _UNIX_LIKE_SYSTEM_ # Example, if minizip's source needs it for non-Windows
)

if(WIN32)
    target_compile_definitions(ldview_minizip_lib PRIVATE 
        _CRT_SECURE_NO_WARNINGS # Often needed for older C code
        # IOWIN32_USING_WINRT_API # If specific WinRT features are used by iowin32.c
    )
else()
    # Potentially -DUSE_FILE32API for macOS/Linux if that's how it's built there
    # The LDView.pro used CFLAGS="-O -DUSE_FILE32API ..." for macOS minizip
    if(APPLE)
        target_compile_definitions(ldview_minizip_lib PRIVATE USE_FILE32API)
    endif()
    # Add -fPIC for Unix-like systems if this static lib might be linked into a shared lib later
    # Though for LDView, most libs are static and linked into the main EXE or specific DLLs.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

message(STATUS "Configuring bundled Minizip library target: ldview_minizip_lib")

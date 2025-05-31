# 3rdParty/gl2ps/CMakeLists.txt

set(GL2PS_SOURCES
    gl2ps.c
)

set(GL2PS_HEADERS
    gl2ps.h
)

add_library(ldview_gl2ps_lib STATIC
    ${GL2PS_SOURCES}
    ${GL2PS_HEADERS}
)

# Definitions needed by gl2ps
target_compile_definitions(ldview_gl2ps_lib PRIVATE
    GL2PS_HAVE_ZLIB
    GL2PS_HAVE_LIBPNG
)

# Include its own directory for gl2ps.h
target_include_directories(ldview_gl2ps_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# GL2PS requires ZLIB and PNG.
# These should be found by find_package in the root CMakeLists.txt
# and their targets (ZLIB::ZLIB, PNG::PNG) should be available.
target_link_libraries(ldview_gl2ps_lib PUBLIC
    ZLIB::ZLIB
    PNG::PNG
)

if(NOT WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

message(STATUS "Configuring bundled gl2ps library target: ldview_gl2ps_lib")

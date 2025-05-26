# 3rdParty/tinyxml/CMakeLists.txt

set(TINYXML_SOURCES
    tinystr.cpp
    tinyxml.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
)

set(TINYXML_HEADERS
    tinystr.h
    tinyxml.h
)

add_library(ldview_tinyxml_lib STATIC
    ${TINYXML_SOURCES}
    ${TINYXML_HEADERS}
)

target_compile_definitions(ldview_tinyxml_lib PUBLIC # PUBLIC because users of tinyxml need this
    TIXML_USE_STL
)

target_include_directories(ldview_tinyxml_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}" # For its own headers
)

if(NOT WIN32)
    # Add -fPIC for Unix-like systems if this static lib might be linked into a shared lib later
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

message(STATUS "Configuring bundled TinyXML library target: ldview_tinyxml_lib")

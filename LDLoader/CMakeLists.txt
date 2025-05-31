# LDLoader/CMakeLists.txt

set(LDLOADER_SOURCES
    LDLActionLine.cpp
    LDLAutoCamera.cpp
    LDLCamera.cpp
    LDLCommentLine.cpp
    LDLConditionalLineLine.cpp
    LDLEmptyLine.cpp
    LDLError.cpp
    LDLFacing.cpp
    LDLFileLine.cpp
    LDLFindFileAlert.cpp
    LDLLineLine.cpp
    LDLMainModel.cpp
    LDLModel.cpp
    LDLModelLine.cpp
    LDLPalette.cpp
    LDLPrimitiveCheck.cpp
    LDLQuadLine.cpp
    LDLShapeLine.cpp
    LDLTriangleLine.cpp
    LDLUnknownLine.cpp
    LDrawIni.c
)

set(LDLOADER_HEADERS
    LDLActionLine.h
    LDLAutoCamera.h
    LDLCamera.h
    LDLCommentLine.h
    LDLConditionalLineLine.h
    LDLEmptyLine.h
    LDLError.h
    LDLFacing.h
    LDLFileLine.h
    LDLFindFileAlert.h
    LDLLineLine.h
    LDLMacros.h
    LDLMainModel.h
    LDLModel.h
    LDLModelLine.h
    LDLPalette.h
    LDLPrimitiveCheck.h
    LDLQuadLine.h
    LDLShapeLine.h
    LDLTriangleLine.h
    LDLUnknownLine.h
    LDrawIni.h
    LDrawInP.h
)

add_library(ldview_ldloader_lib STATIC
    ${LDLOADER_SOURCES}
    ${LDLOADER_HEADERS}
)

target_compile_definitions(ldview_ldloader_lib PRIVATE
    USE_CPP11 # Common
    HAVE_MINIZIP # Common
    _TC_STATIC # Common
    _WIN_UTF8_PATHS # Specific to LDLoader
    $<$<CONFIG:Release>:_NO_BOOST>
    $<$<BOOL:${LDVIEW_USE_BOOST}>:USE_BOOST_LIBS>
)

if(MSVC)
    # Reflects MultiByte from vcxproj for this lib
    # If unifying to Unicode, this would change.
    target_compile_definitions(ldview_ldloader_lib PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        _LIB
        _CRT_SECURE_NO_WARNINGS # Added
    )
else()
    # Common Unix flags
    target_compile_options(ldview_ldloader_lib PRIVATE -fPIC) # Position Independent Code
endif()

target_include_directories(ldview_ldloader_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}" # For its own headers
    "${CMAKE_SOURCE_DIR}"         # For includes like "TCFoundation/TCUserDefaults.h"
)

# Dependencies
target_link_libraries(ldview_ldloader_lib PRIVATE
    ldview_tcfoundation_lib
)

if(LDVIEW_USE_BUNDLED_DEPS AND TARGET ldview_minizip_lib)
    target_link_libraries(ldview_ldloader_lib PRIVATE ldview_minizip_lib)
else()
    # TODO: Add find_package(Minizip) or similar if system minizip is an option
    # For now, this assumes bundled minizip if LDVIEW_USE_BUNDLED_DEPS is ON.
    # If LDVIEW_USE_BUNDLED_DEPS is OFF, this build might fail if system minizip isn't handled.
    # This can be refined when focusing on Linux/macOS.
endif()

if(LDVIEW_USE_BOOST AND Boost_FOUND)
    target_include_directories(ldview_ldloader_lib PRIVATE ${Boost_INCLUDE_DIRS})
endif()

message(STATUS "Configuring LDLoader library target: ldview_ldloader_lib")

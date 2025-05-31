# TRE/CMakeLists.txt

set(TRE_SOURCES
    TREColoredShapeGroup.cpp
    TREGLExtensions.cpp
    TREMainModel.cpp
    TREModel.cpp
    TREShapeGroup.cpp
    TRESmoother.cpp
    TRESubModel.cpp
    TRETexmappedShapeGroup.cpp
    TRETransShapeGroup.cpp
    TREVertexArray.cpp
    TREVertexStore.cpp
)

set(TRE_HEADERS
    TREColoredShapeGroup.h
    TREGL.h
    TREGLExtensions.h
    TREMainModel.h
    TREModel.h
    TREShapeGroup.h
    TRESmoother.h
    TRESubModel.h
    TRETexmappedShapeGroup.h
    TRETransShapeGroup.h
    TREVertexArray.h
    TREVertexKey.h
    TREVertexStore.h
)

add_library(ldview_tre_lib STATIC
    ${TRE_SOURCES}
    ${TRE_HEADERS}
)

target_compile_definitions(ldview_tre_lib PRIVATE
    USE_CPP11
    HAVE_MINIZIP # Commonly defined across projects
    _TC_STATIC
    $<$<CONFIG:Debug>:_ALLOW_RTCc_IN_STL> # Debug only
    $<$<CONFIG:Release>:_NO_BOOST>
    $<$<BOOL:${LDVIEW_USE_BOOST}>:USE_BOOST_LIBS>
)

if(MSVC)
    target_compile_definitions(ldview_tre_lib PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        _LIB
    )
else()
    target_compile_options(ldview_tre_lib PRIVATE -fPIC)
endif()

target_include_directories(ldview_tre_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}" # For its own headers
    "${CMAKE_SOURCE_DIR}"         # For includes like "TCFoundation/..." or "LDLoader/..."
    "${CMAKE_SOURCE_DIR}/include" # General shared includes
    "${CMAKE_SOURCE_DIR}/3rdParty/gl2ps" # As seen in vcxproj
)

# Dependencies
target_link_libraries(ldview_tre_lib PRIVATE
    ldview_tcfoundation_lib
    ldview_ldloader_lib # For LDLPrimitiveCheck.h
)

# Link gl2ps if it's built as a CMake target (used for vector export capabilities that TRE might support)
if(TARGET ldview_gl2ps_lib)
    target_link_libraries(ldview_tre_lib PRIVATE ldview_gl2ps_lib)
endif()

if(LDVIEW_USE_BOOST AND Boost_FOUND)
    target_include_directories(ldview_tre_lib PRIVATE ${Boost_INCLUDE_DIRS})
endif()

message(STATUS "Configuring TRE library target: ldview_tre_lib")

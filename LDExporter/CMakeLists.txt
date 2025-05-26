# LDExporter/CMakeLists.txt

set(LDEXPORTER_SOURCES
    LD3dsExporter.cpp
    LDExporter.cpp
    LDExporterSetting.cpp
    LDPovExporter.cpp
    LDStlExporter.cpp
    # LDLdrExporter.cpp # Not in vcxproj, consider adding if used by other build systems
)

set(LDEXPORTER_HEADERS
    LD3dsExporter.h
    LDExporter.h
    LDExporterSetting.h
    LDPovExporter.h
    LDStlExporter.h
    # LDLdrExporter.h
)

add_library(ldview_ldexporter_lib STATIC
    ${LDEXPORTER_SOURCES}
    ${LDEXPORTER_HEADERS}
)

target_compile_definitions(ldview_ldexporter_lib PRIVATE
    USE_CPP11
    HAVE_MINIZIP # Common define
    _TC_STATIC
    TIXML_USE_STL # For TinyXML
    EXPORT_3DS    # For 3DS export functionality
    $<$<CONFIG:Release>:_NO_BOOST> # As per vcxproj
    $<$<BOOL:${LDVIEW_USE_BOOST}>:USE_BOOST_LIBS>
)

if(MSVC)
    # Reflects MultiByte from vcxproj
    target_compile_definitions(ldview_ldexporter_lib PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        _LIB
    )
else()
    target_compile_options(ldview_ldexporter_lib PRIVATE -fPIC)
endif()

target_include_directories(ldview_ldexporter_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}" # For its own headers
    "${CMAKE_SOURCE_DIR}"         # For includes like "TCFoundation/..."
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/3rdParty/lib3ds"  # Specific for lib3ds
)

# Dependencies
target_link_libraries(ldview_ldexporter_lib PRIVATE
    ldview_tcfoundation_lib
    ldview_tre_lib          # Exporters will need geometry data from TRE
    # ldview_ldloader_lib   # Might be needed if exporter directly accesses LDL* types
    # ldview_ldlib_lib      # For LDPreferences or other common types
)

# Link TinyXML
if(LDVIEW_USE_BUNDLED_DEPS AND TARGET ldview_tinyxml_lib)
    target_link_libraries(ldview_ldexporter_lib PRIVATE ldview_tinyxml_lib)
else()
    # Attempt to find system TinyXML if not using bundled
    # find_package(TinyXML) or find_library/find_path for tinyxml
    # if(TinyXML_FOUND) target_link_libraries(ldview_ldexporter_lib PRIVATE TinyXML::TinyXML)
endif()

# Link lib3ds (assuming it's found and an imported target ldview_lib3ds is created)
# This part depends on how lib3ds is handled in the root CMakeLists.txt
if(TARGET ldview_lib3ds) # ldview_lib3ds would be an IMPORTED library or built from source
    target_link_libraries(ldview_ldexporter_lib PRIVATE ldview_lib3ds)
endif()

if(LDVIEW_USE_BOOST AND Boost_FOUND)
    target_include_directories(ldview_ldexporter_lib PRIVATE ${Boost_INCLUDE_DIRS})
endif()

message(STATUS "Configuring LDExporter library target: ldview_ldexporter_lib")

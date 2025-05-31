# UnMirrorStuds/CMakeLists.txt

set(UNMIRRORSTUDS_SOURCES
    UnMirrorStuds.cpp
    UnMirrorStudsMain.cpp
    UnMirrorStuds.rc
)

add_executable(ldview_unmirrorstuds_exe
    ${UNMIRRORSTUDS_SOURCES}
)
# No specific output name change, will be ldview_unmirrorstuds_exe.exe

target_compile_definitions(ldview_unmirrorstuds_exe PRIVATE
    USE_CPP11
    _CONSOLE
    _TC_STATIC # Uses TCFoundation and LDLoader
)

if(MSVC)
    # Reflects MultiByte from vcxproj
    target_compile_definitions(ldview_unmirrorstuds_exe PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )
    # Add standard Windows libraries if needed for console apps
    target_link_libraries(ldview_unmirrorstuds_exe PRIVATE kernel32.lib user32.lib advapi32.lib shell32.lib)
else()
    # For GCC/Clang, basic C++ std lib and pthreads if needed by TCFoundation
    # No specific system libs typically needed for console other than C/C++ runtime
endif()

target_include_directories(ldview_unmirrorstuds_exe PRIVATE
    "${CMAKE_SOURCE_DIR}" # For "TCFoundation/..." and "LDLoader/..."
)

# Dependencies
target_link_libraries(ldview_unmirrorstuds_exe PRIVATE
    ldview_tcfoundation_lib
    ldview_ldloader_lib
)

message(STATUS "Configuring UnMirrorStuds utility target: ldview_unmirrorstuds_exe")

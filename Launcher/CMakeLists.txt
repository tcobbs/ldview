# Launcher/CMakeLists.txt

add_executable(ldview_launcher_exe Launcher.cpp)

# Attempt to set output name to LDView.com
# This might require post-build renaming if CMake doesn't directly produce .com extension reliably.
set_target_properties(ldview_launcher_exe PROPERTIES
    OUTPUT_NAME "LDView"
    SUFFIX ".com"
)

target_compile_definitions(ldview_launcher_exe PRIVATE
    USE_CPP11 # From vcxproj
    _CONSOLE
)

if(MSVC)
    target_compile_definitions(ldview_launcher_exe PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        UNICODE # From vcxproj
        _UNICODE
    )
    # Link kernel32 and shlwapi. Conditionally link user32 for Debug.
    target_link_libraries(ldview_launcher_exe PRIVATE
        kernel32.lib
        shlwapi.lib
        $<$<CONFIG:Debug>:user32.lib>
    )
else()
    # Add any necessary non-Windows link libraries if this target were to be built there.
    # For now, it's primarily a Windows utility.
endif()

message(STATUS "Configuring Launcher target: ldview_launcher_exe (Output: LDView.com)")

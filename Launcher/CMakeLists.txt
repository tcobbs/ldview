# Launcher/CMakeLists.txt

add_executable(ldview_launcher_exe Launcher.cpp)

# Set target name to LDView and extension to .com as per vcxproj
# Note: CMake typically adds .exe on Windows. Setting .com might be tricky
# or require custom commands. For now, let's name it LDView_Launcher.exe
# and address the .com extension later if strictly necessary.
# A more standard approach is to keep it .exe if it's an executable.
# The vcxproj sets <TargetName>LDView</TargetName> and <TargetExt>.com</TargetExt>.
# This implies it wants to be named "LDView.com".
# We can try to set OUTPUT_NAME to "LDView" and SUFFIX to ".com".
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
    target_link_libraries(ldview_launcher_exe PRIVATE
        kernel32.lib
        shlwapi.lib
        $<$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>:$<TARGET_PROPERTY:ldview_launcher_exe,LINK_LIBRARIES>,user32.lib> # user32 only for Debug x64 in vcxproj, more general here
    )
else()
    # For non-MSVC, ensure similar basic libraries if any are needed for console apps
    # shlwapi is Windows-specific. If this needs to build on other platforms,
    # functionality from shlwapi might need alternatives.
    # For now, this target is primarily Windows-focused due to its nature.
endif()

message(STATUS "Configuring Launcher target: ldview_launcher_exe (Output: LDView.com)")

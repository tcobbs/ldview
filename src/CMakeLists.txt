# src/CMakeLists.txt

# --- Source File List ---
# Note: This list assumes files have been moved into the 'src' directory.
# Add all .cpp files that were previously in the root for LDView.exe
set(LDVIEW_APP_SOURCES
    LDViewMain.cpp
    LDViewWindow.cpp
    ModelWindow.cpp
    ModelLoader.cpp
    LDViewPreferences.cpp
    # Dialogs
    BatchConvertDialog.cpp # Recently added
    BoundingBoxDialog.cpp
    CameraLocationDialog.cpp
    ExportOptionsDialog.cpp
    JpegOptionsDialog.cpp
    LatLonDialog.cpp
    LocationDialog.cpp # Base class for LatLon/CameraLocation
    ModelTreeDialog.cpp
    MpdDialog.cpp
    OptionsCanvas.cpp    # Related to Preferences Dialog
    OptionsScroller.cpp  # Related to Preferences Dialog
    PartsListDialog.cpp
    RotationCenterDialog.cpp
    StatisticsDialog.cpp
    StepDialog.cpp
    # OptionUI
    BoolOptionUI.cpp
    EnumOptionUI.cpp
    FloatOptionUI.cpp
    GroupOptionUI.cpp
    LongOptionUI.cpp
    NumberOptionUI.cpp
    OptionUI.cpp
    PathOptionUI.cpp
    StringOptionUI.cpp
    # ScreenSaver related
    SSConfigure.cpp
    SSModelWindow.cpp
    SSPreview.cpp
    # Toolbar
    TbButtonInfo.cpp
    ToolbarStrip.cpp
    # Misc
    WinWebClientPlugin.cpp
    LDVExtensionsSetup.cpp
    BatchStlConverter.cpp # Recently added
)

# Resource file
set(LDVIEW_APP_RESOURCES LDView.rc)

# Manifest files are handled via VS_MANIFEST_FILES property below

add_executable(ldview_app_exe WIN32 # WIN32 for Windows GUI application
    ${LDVIEW_APP_SOURCES}
    ${LDVIEW_APP_RESOURCES}
)
set_target_properties(ldview_app_exe PROPERTIES OUTPUT_NAME "LDView")

# Set target name for 64-bit to LDView64 (matches vcxproj)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_target_properties(ldview_app_exe PROPERTIES OUTPUT_NAME "LDView64")
endif()

target_compile_definitions(ldview_app_exe PRIVATE
    USE_CPP11
    HAVE_MINIZIP
    _WINDOWS
    _WIN32_WINDOWS=0x0501 # For broader compatibility
    _WIN32_WINNT=0x0501   # For broader compatibility
    _TC_STATIC
    LDVIEW_APP
    EXPORT_3DS
    $<$<CONFIG:Release>:_NO_BOOST>
    $<$<CONFIG:Debug>:_LOG_PERFORMANCE>
    $<$<STREQUAL:${CMAKE_SIZEOF_VOID_P},4>:LDVIEW_NO_64> # Define LDVIEW_NO_64 for 32-bit
)

if(MSVC)
    target_compile_definitions(ldview_app_exe PRIVATE
        WIN32 # Already added by WIN32 in add_executable, but good for clarity
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        UNICODE # Main app is Unicode
        _UNICODE
    )
endif()

# Include directories
target_include_directories(ldview_app_exe PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}" # For its own headers (Resource.h etc.)
    "${CMAKE_SOURCE_DIR}"         # For accessing library headers like "TCFoundation/TCFoundation.h"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/3rdParty"
    # Add boost if used
    $<$<BOOL:${LDVIEW_USE_BOOST}>:${Boost_INCLUDE_DIRS}>
)

# Link core LDView libraries
target_link_libraries(ldview_app_exe PRIVATE
    ldview_cui_lib          # If CUI is built (Windows GUI)
    ldview_ldlib_lib
    ldview_ldexporter_lib
    ldview_tre_lib
    ldview_ldloader_lib
    ldview_tcfoundation_lib
)

# Link bundled 3rd party libraries (if built as targets)
if(LDVIEW_USE_BUNDLED_DEPS)
    if(TARGET ldview_gl2ps_lib)
        target_link_libraries(ldview_app_exe PRIVATE ldview_gl2ps_lib)
    endif()
    if(TARGET ldview_tinyxml_lib)
        target_link_libraries(ldview_app_exe PRIVATE ldview_tinyxml_lib)
    endif()
    if(TARGET ldview_minizip_lib) # TCFoundation and LDLoader might already link this
        # Check if already linked transitively before adding, or ensure it's an INTERFACE lib if header-only after build
        # For now, let's assume it might be needed directly too for some functions.
        target_link_libraries(ldview_app_exe PRIVATE ldview_minizip_lib)
    endif()
endif()

# Link system libraries and prebuilt 3rd party libs for Windows
if(WIN32)
    target_link_libraries(ldview_app_exe PRIVATE
        kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib
        opengl32 # CMake should find these via find_package(OpenGL)
        glu32    # CMake should find these via find_package(GLU)
        ws2_32.lib
        winmm.lib
        shlwapi.lib
        comctl32.lib
        Winhttp.lib
        version.lib
        HtmlHelp.lib # Added from vcxproj
        # Prebuilt image/compression libs (need to handle Debug/Release and Win32/x64 paths/names)
        # This requires more advanced setup for finding the correct lib based on config.
        # For now, placeholder names. Actual finding logic will be in root or a helper module.
        # Example for one lib (needs to be conditionalized):
        # $<$<CONFIG:Debug>:$<PLATFORM_ID:Win32>:${CMAKE_SOURCE_DIR}/lib/zlibd.lib> # Example
        # $<$<CONFIG:Release>:$<PLATFORM_ID:Win32>:${CMAKE_SOURCE_DIR}/lib/zlib.lib> # Example
    )
    # Add prebuilt libraries (zlib, png, jpeg, 3ds)
    # This needs a robust way in root CMakeLists to define imported targets for these.
    # For now, assume they will be found and linked (e.g. ZLIB::ZLIB from find_package)
    # If not found via find_package, direct paths or find_library calls are needed.
    target_link_libraries(ldview_app_exe PRIVATE ZLIB::ZLIB PNG::PNG JPEG::JPEG) # Assuming JPEG is found
    
    # Link lib3ds if found and EXPORT_3DS is intended
    if(TARGET ldview_lib3ds) # This would be an IMPORTED target for prebuilt lib3ds
         target_link_libraries(ldview_app_exe PRIVATE ldview_lib3ds)
    endif()

    # Handling manifests (simplified - vcxproj uses <Manifest> tag)
    # This usually means the linker handles embedding from .rc or specific manifest files.
    # If LDView.exe.manifest/LDView64.exe.manifest needs explicit linking:
    # For MSVC, could use target_link_options or source properties on the .rc file.
    # Or ensure the .rc file includes them.
    # The dpiv2.manifest is often added to the resource script.

    # Handling manifests using VS_MANIFEST_FILES property
    set(LDVIEW_APP_MANIFEST_FILES "")
    # dpiv2.manifest is typically embedded via the main .rc file or linked if needed.
    # The vcxproj has <AdditionalManifestFiles>dpiv2.manifest;%(AdditionalManifestFiles)</AdditionalManifestFiles>
    # AND <EmbedManifest>true</EmbedManifest> with <ManifestTool/> specifying the exe manifest.
    # This implies the target manifest (LDView.exe.manifest) is primary and dpiv2 is additional.
    # CMake's VS_MANIFEST_FILES is for *additional* manifest files. The primary is usually auto-generated or from .rc.
    # If LDView.rc already includes RT_MANIFEST for ID 1 (application manifest), then dpiv2.manifest is additional.
    # Let's assume LDView.rc handles the main application manifest.
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dpiv2.manifest")
        list(APPEND LDVIEW_APP_MANIFEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/dpiv2.manifest")
    endif()
    
    # The specific LDView.exe.manifest / LDView64.exe.manifest seems to be the *output* manifest name
    # in vcxproj, not an additional input. CMake generates one by default.
    # If these files contain specific content that needs to be *merged*,
    # then they might be additional inputs. Assuming for now they are not *additional* inputs
    # unless the default generated manifest + dpiv2 is insufficient.
    # The vcxproj's <Manifest Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    #   <AdditionalManifestFiles>dpiv2.manifest;%(AdditionalManifestFiles)</AdditionalManifestFiles>
    # </Manifest>
    # suggests dpiv2.manifest is the primary one to list as additional.
    # The final embedded manifest is a combination.

    if(LDVIEW_APP_MANIFEST_FILES)
        set_target_properties(ldview_app_exe PROPERTIES VS_MANIFEST_FILES "${LDVIEW_APP_MANIFEST_FILES}")
    endif()
endif()

message(STATUS "Configuring LDView Windows Application target: ldview_app_exe")

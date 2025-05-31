# Translations/EnglishUS/CMakeLists.txt

# Source files for this translation DLL
set(TRANS_EN_US_SOURCES
    EnglishUS.cpp
    LDView.rc # This is Translations/EnglishUS/LDView.rc
)

add_library(ldview_lang_en_us_dll SHARED
    ${TRANS_EN_US_SOURCES}
)

# Set the output name as per the vcxproj convention
set_target_properties(ldview_lang_en_us_dll PROPERTIES
    OUTPUT_NAME "LDView-English (United States)"
    # Ensure it creates a DLL on Windows, not a LIB for SHARED if no exports found
    WINDOWS_EXPORT_ALL_SYMBOLS OFF # We export via .rc / resource only
)

# Define as a Windows Resource DLL if possible (though CMake handles this mostly by content)
# No specific compile definitions were in the vcxproj beyond standard ones.
if(MSVC)
    target_compile_definitions(ldview_lang_en_us_dll PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        _WINDOWS # For DLLs
        _USRDLL  # For DLLs
        # According to EnglishUS.vcxproj, CharacterSet is MultiByte
        # If unifying, this would change. For now, reflect vcxproj.
    )
    target_link_libraries(ldview_lang_en_us_dll PRIVATE version.lib)
endif()

# The output directory for translation DLLs in vcxproj was:
# $(SolutionDir)..\..\Build\$(Configuration)\ or $(SolutionDir)..\..\Build\$(Configuration)64\
# This implies they go into the main application's output directory (e.g., ldview/build/bin/Release)
# CMake's CMAKE_RUNTIME_OUTPUT_DIRECTORY set in the root should handle this.
# However, they might need to go into a specific subdirectory like 'Translations' or 'lang/en-US'
# For now, let them go to CMAKE_RUNTIME_OUTPUT_DIRECTORY.
# If they need to be in a "Translations" subfolder of that:
# set_target_properties(ldview_lang_en_us_dll PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Translations")

message(STATUS "Configuring EnglishUS Translation DLL target: ldview_lang_en_us_dll")

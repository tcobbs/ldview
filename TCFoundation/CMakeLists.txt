# TCFoundation/CMakeLists.txt

set(TCFOUNDATION_SOURCES
    ConvertUTF.c
    mystring.cpp
    STB.cpp
    TCAlert.cpp
    TCAlertManager.cpp
    TCAutoreleasePool.cpp
    TCBmpImageFormat.cpp
    TCDictionary.cpp
    TCImage.cpp
    TCImageFormat.cpp
    TCImageOptions.cpp
    TCJpegImageFormat.cpp
    TCJpegOptions.cpp
    TCLocalStrings.cpp
    TCNetwork.cpp
    TCNetworkClient.cpp
    TCObject.cpp
    TCObjectArray.cpp
    TCPngImageFormat.cpp
    TCProgressAlert.cpp
    TCSortedStringArray.cpp
    TCStringArray.cpp
    TCUnzip.cpp
    TCUnzipStream.cpp
    TCUserDefaults.cpp
    TCVector.cpp
    TCWebClient.cpp
)

set(TCFOUNDATION_HEADERS
    ConvertUTF.h
    mystring.h
    TCAlert.h
    TCAlertManager.h
    TCAlertSender.h
    TCArray.h
    TCAutoreleasePool.h
    TCBmpImageFormat.h
    TCDefines.h
    TCDictionary.h
    TCImage.h
    TCImageFormat.h
    TCImageOptions.h
    TCJpegImageFormat.h
    TCJpegOptions.h
    TCLocalStrings.h
    TCMacros.h
    TCNetwork.h
    TCNetworkClient.h
    TCObject.h
    TCObjectArray.h
    TCPngImageFormat.h
    TCProgressAlert.h
    TCSortedStringArray.h
    TCStlIncludes.h
    TCStringArray.h
    TCTypedDictionary.h
    TCTypedObjectArray.h
    TCTypedPointerArray.h
    TCTypedValueArray.h
    TCUnzip.h
    TCUnzipStream.h
    TCUserDefaults.h
    TCVector.h
    TCWebClient.h
)

add_library(ldview_tcfoundation_lib STATIC
    ${TCFOUNDATION_SOURCES}
    ${TCFOUNDATION_HEADERS} # Headers often listed for IDE visibility
)

target_compile_definitions(ldview_tcfoundation_lib PRIVATE
    _TC_STATIC # As it's a static lib and used by others expecting this
    HAVE_MINIZIP # From vcxproj
    _BUILDING_TCFOUNDATION_LIB # From vcxproj
    $<$<CONFIG:Release>:_NO_BOOST> # From vcxproj
    $<$<BOOL:${LDVIEW_USE_BOOST}>:USE_BOOST_LIBS> # Optional, if we decide to use it via CMake option
)

# Add platform-specific definitions (Windows)
if(MSVC)
    # Based on TCFoundation.vcxproj, it uses MultiByte character set.
    # If we decide to unify to Unicode, this would change.
    # For now, let's reflect the vcxproj.
    target_compile_definitions(ldview_tcfoundation_lib PRIVATE
        $<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:WindowsStore>>:WIN32> # Generic WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        _LIB # It's a static library
        _CRT_SECURE_NO_WARNINGS # Added
        $<$<CONFIG:Release>:_WINSOCK_DEPRECATED_NO_WARNINGS>
    )
else()
    # Add flags for GCC/Clang if different from root CMakeLists.txt
    # (e.g. -fPIC if it were linked into shared libs, though as static it might not be needed)
endif()

target_include_directories(ldview_tcfoundation_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}" # So its own headers are found with ""
    "${CMAKE_SOURCE_DIR}" # For top-level includes like "TCFoundation/TCDefines.h" from other projects
    "${CMAKE_SOURCE_DIR}/include" # General include dir (for zlib, png etc. headers)
    "${CMAKE_SOURCE_DIR}/3rdParty" # For other 3rd party headers
    "${CMAKE_SOURCE_DIR}/3rdParty/libjpeg"
    "${CMAKE_SOURCE_DIR}/3rdParty/libjpeg/Windows" # As per vcxproj
    "${CMAKE_SOURCE_DIR}/3rdParty/libpng"
    "${CMAKE_SOURCE_DIR}/3rdParty/libpng/Windows"   # As per vcxproj
)

# If Boost is used and found by find_package(Boost) in root CMakeLists.txt
if(LDVIEW_USE_BOOST AND Boost_FOUND)
    target_include_directories(ldview_tcfoundation_lib PRIVATE ${Boost_INCLUDE_DIRS})
    # No linking for Boost header-only, or link components if needed
endif()

# Link minizip if it's built as a CMake target
if(TARGET ldview_minizip_lib)
    target_link_libraries(ldview_tcfoundation_lib PRIVATE ldview_minizip_lib)
endif()

# Example of how to handle precompiled headers if desired later (more complex)
# target_precompile_headers(ldview_tcfoundation_lib PRIVATE StdAfx.h) # If TCFoundation used PCH

message(STATUS "Configuring TCFoundation library target: ldview_tcfoundation_lib")

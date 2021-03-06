##########################################################################################
#                                                                                        #
# OpenSpace                                                                              #
#                                                                                        #
# Copyright (c) 2014-2018                                                                #
#                                                                                        #
# Permission is hereby granted, free of charge, to any person obtaining a copy of this   #
# software and associated documentation files (the "Software"), to deal in the Software  #
# without restriction, including without limitation the rights to use, copy, modify,     #
# merge, publish, distribute, sublicense, and/or sell copies of the Software, and to     #
# permit persons to whom the Software is furnished to do so, subject to the following    #
# conditions:                                                                            #
#                                                                                        #
# The above copyright notice and this permission notice shall be included in all copies  #
# or substantial portions of the Software.                                               #
#                                                                                        #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,    #
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A          #
# PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT     #
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF   #
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE   #
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                          #
##########################################################################################

include(${OPENSPACE_CMAKE_EXT_DIR}/module_definition.cmake)
include(${GHOUL_BASE_DIR}/support/cmake/handle_external_library.cmake)

set(WEBBROWSER_MODULE_NAME WebBrowser)
set(WEBBROWSER_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "WEBBROWSER_MODULE_PATH")

# wanted by CEF
set(PROJECT_ARCH "x86_64")
set(CMAKE_BUILD_TYPE Debug)

##########################################################################################
# Download CEF

# Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

cmake_minimum_required(VERSION 2.8.12.1)

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

# Use <PackageName>_ROOT variables
# https://cmake.org/cmake/help/git-stage/policy/CMP0074.html
cmake_policy(SET CMP0074 NEW)

# Specify the CEF distribution version.
set(CEF_VERSION "3.3029.1617.gaf831b6")

# Add this project's cmake/ directory to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cef_support)

# determine the current build platform
set_current_cef_build_platform()

# Download and extract the CEF binary distribution (executes DownloadCEF.cmake).
download_cef("${CEF_PLATFORM}" "${CEF_VERSION}" "${CMAKE_CURRENT_SOURCE_DIR}/ext/cef")

##########################################################################################
# Apply cmake patch

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/patch/CMakeLists.txt DESTINATION ${CEF_ROOT})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/patch/cmake/cef_variables.cmake DESTINATION ${CEF_ROOT}/cmake)

# CEF does not provide RelWithDebInfo libs, so we use the ones without Debug information
file(COPY ${CEF_ROOT}/Release/
  DESTINATION ${CEF_ROOT}/RelWithDebInfo
  FILES_MATCHING PATTERN *
)

##########################################################################################

# Add the CEF binary distribution's cmake/ directory to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Load the CEF configuration (executes FindCEF.cmake).
find_package(CEF REQUIRED)

# Include the libcef_dll_wrapper target (executes libcef_dll/CMakeLists.txt).
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

mark_as_advanced(CEF_DEBUG_INFO_FLAG USE_ATL USE_OFFICIAL_BUILD_SANDBOX USE_SANDBOX)

##########################################################################################
# Add CEF client files

set(WEBBROWSER_SOURCES
  webbrowsermodule.cpp
  src/browserclient.cpp
  src/eventhandler.cpp
  src/webbrowserapp.cpp
  src/defaultbrowserlauncher.cpp
)
set(WEBBROWSER_SOURCES_WINDOWS
  OpenSpace.exe.manifest
  cef_webgui.rc
  resource.h
  simple_handler_win.cc
)
APPEND_PLATFORM_SOURCES(WEBBROWSER_SOURCES)
APPEND_PLATFORM_SOURCES(WEBBROWSER_RESOURCES_SOURCES)

set(WEBBROWSER_SOURCES ${WEBBROWSER_SOURCES} ${WEBBROWSER_RESOURCES_SOURCES})

# CEF helper sources
set(WEBBROWSER_HELPER_SOURCES src/webbrowserapp.cpp)
set(WEBBROWSER_HELPER_SOURCES_MACOSX src/processhelpermac.cpp)
set(WEBBROWSER_HELPER_SOURCES_WINDOWS src/processhelperwindows.cpp)
APPEND_PLATFORM_SOURCES(WEBBROWSER_HELPER_SOURCES)

set(WEBBROWSER_RESOURCES_MAC_SOURCES_MACOSX
  mac/Info.plist
  mac/webgui.icns
)
APPEND_PLATFORM_SOURCES(WEBBROWSER_RESOURCES_MAC_SOURCES)

set(WEBBROWSER_RESOURCES_MAC_ENGLISH_LPROJ_SRCS_MACOSX
  mac/English.lproj/InfoPlist.strings
  mac/English.lproj/MainMenu.xib
)
APPEND_PLATFORM_SOURCES(WEBBROWSER_RESOURCES_MAC_ENGLISH_LPROJ_SRCS)

set(WEBBROWSER_RESOURCES_SRCS
  ${WEBBROWSER_RESOURCES_MAC_SOURCES}
  ${WEBBROWSER_RESOURCES_MAC_ENGLISH_LPROJ_SRCS}
)

# place helper in separate executable
set(CEF_HELPER_TARGET "openspace_web_helper" CACHE INTERNAL "CEF_HELPER_TARGET")

#
# CEF platform-specific config
#

if(OS_MACOSX)
  # set global CEF helper app target
  set(CEF_HELPER_APP "${CEF_TARGET_OUT_DIR}/${CEF_HELPER_TARGET}.app" CACHE INTERNAL "CEF_HELPER_APP")

  # Helper executable target.
  add_executable(${CEF_HELPER_TARGET} MACOSX_BUNDLE ${WEBBROWSER_HELPER_SOURCES})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_HELPER_TARGET})
  add_cef_logical_target("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
  add_dependencies(${CEF_HELPER_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_HELPER_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  set_target_properties(${CEF_HELPER_TARGET} PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/helper-Info.plist
  )

  # Fix the framework rpath in the helper executable.
  FIX_MACOSX_HELPER_FRAMEWORK_RPATH(${CEF_HELPER_TARGET})
endif()

if (OS_WINDOWS)
  message(STATUS "Setting up WebBrowser CEF helper executable: ${CEF_HELPER_TARGET}")
  set_openspace_cef_target_out_dir()
  add_executable(${CEF_HELPER_TARGET} ${WEBBROWSER_HELPER_SOURCES})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_HELPER_TARGET})
  add_dependencies(${CEF_HELPER_TARGET} libcef_dll_wrapper)
  # Logical target used to link the libcef library.
  add_cef_logical_target("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
  target_link_libraries(${CEF_HELPER_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  # Add the custom manifest files to the executable.
  add_windows_cef_manifest("${CEF_TARGET_OUT_DIR}" "${WEBBROWSER_MODULE_PATH}" "${CEF_HELPER_TARGET}" "exe")
endif (OS_WINDOWS)


##########################################################################################
# Create OpenSpace module.
set(HEADER_FILES
  webbrowsermodule.h
  include/webrenderhandler.h
  include/webkeyboardhandler.h
  include/browserclient.h
  include/screenspacebrowser.h
  include/browserinstance.h
  include/eventhandler.h
  include/webbrowserapp.h
  include/defaultbrowserlauncher.h
  include/cefhost.h
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
  webbrowsermodule.cpp
  src/webrenderhandler.cpp
  src/webkeyboardhandler.cpp
  src/browserclient.cpp
  src/screenspacebrowser.cpp
  src/browserinstance.cpp
  src/eventhandler.cpp
  src/webbrowserapp.cpp
  src/defaultbrowserlauncher.cpp
  src/cefhost.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

create_new_module(
  ${WEBBROWSER_MODULE_NAME}
  webbrowser_module
  STATIC
  ${HEADER_FILES} ${SOURCE_FILES}
)

set_folder_location(libcef_dll_wrapper "Helper")
set_folder_location(openspace_web_helper "Helper")


# Display CEF configuration settings.
PRINT_CEF_CONFIG()

target_include_directories(${webbrowser_module} PUBLIC ${CEF_ROOT})

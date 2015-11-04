###
# Copyright (c) 2009, Paul Gideon Dann
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
###

#find_package(LLVM REQUIRED CONFIG)
#
#message(WARNING "Found LLVM ${LLVM_PACKAGE_VERSION}")
#message(WARNING "Using LLVMConfig.cmake in: ${LLVM_DIR}")
#
##add_definitions(${LLVM_DEFINITIONS})
##link_directories()
#include_directories(${LLVM_INCLUDE_DIRS})

#find_package(Clang REQUIRED)
#
#include_directories(${CLANG_INCLUDE_DIRS})
#MESSAGE(WARNING ${CLANG_EXECUTABLE})
#add_definitions(${CLANG_DEFINITIONS})

#set(IWYU_LLVM_ROOT_PATH ${LLVM_PATH})
#
#if( DEFINED LLVM_PATH )
#    set(IWYU_LLVM_ROOT_PATH ${LLVM_PATH})
#    message(WARNING "LLVM_PATH is deprecated, use IWYU_LLVM_ROOT_PATH instead.")
#endif()
#
#if( DEFINED IWYU_LLVM_INCLUDE_PATH AND DEFINED IWYU_LLVM_LIB_PATH )
#    # User provided include/lib paths, fall through
#elseif ( DEFINED IWYU_LLVM_ROOT_PATH )
#    # Synthesize include/lib relative to a root.
#    set(IWYU_LLVM_INCLUDE_PATH ${IWYU_LLVM_ROOT_PATH}/include)
#    set(IWYU_LLVM_LIB_PATH ${IWYU_LLVM_ROOT_PATH}/lib)
#else()
#    # If none provided, fail.
#    message(FATAL_ERROR
#            "Don't know how to find LLVM headers/libraries. "
#            "Use -DIWYU_LLVM_ROOT_PATH=/xyz or both "
#            "-DIWYU_LLVM_INCLUDE_PATH=/xyz/include and -DIWYU_LLVM_LIB_PATH=/xyz/lib")
#endif()
#
#include_directories(${IWYU_LLVM_INCLUDE_PATH})
#link_directories(${IWYU_LLVM_LIB_PATH})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/defines.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/defines.h)

file(GLOB_RECURSE UI_FILES *.ui)
set(SOURCE_FILES
        ${SOURCE_FILES}
        main.cpp
        CodeTextEdit.cpp
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES})
qt5_add_resources(RESOURCE_FILES ../resources/resources.qrc)

# Windows application icon
if (WIN32)
  set(WINDOWS_RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/resources.obj)
  if (MSVC)
    add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
      COMMAND rc.exe /fo ${WINDOWS_RES_FILE} resources.rc
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/win
    )
  else()
    add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
      COMMAND windres.exe resources.rc ${WINDOWS_RES_FILE}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/win
    )
  endif()
endif()

add_executable(${CMAKE_PROJECT_NAME} WIN32
  ${UI_HEADERS}
  ${SOURCE_FILES}
  ${RESOURCE_FILES}
  ${WINDOWS_RES_FILE}
)
target_link_libraries(${CMAKE_PROJECT_NAME}
        Qt5::Widgets
        Qt5::Qml
        clang
)

if (UNIX)
  install(TARGETS ${CMAKE_PROJECT_NAME}
          RUNTIME DESTINATION bin)
elseif (WIN32)
  install(TARGETS ${CMAKE_PROJECT_NAME}
          DESTINATION .)
endif()

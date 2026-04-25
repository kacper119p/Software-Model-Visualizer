# MIT License
#
# Copyright (c) 2026 Kacper Pastuszka
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
set(SOURCE_FILES
        src/model.c
        src/rendering.c
)

set(HEADER_FILES
        inc/appWindow.h
        inc/graphicsMath.h
        inc/model.h
        inc/rendering.h
        inc/timeQuery.h
)

if (WIN32)
    list(APPEND SOURCE_FILES
            src/windows/appWindow.c
            src/windows/timeQuery.c
    )
elseif (UNIX AND NOT APPLE)
    list(APPEND SOURCE_FILES
            src/linux/appWindow.c
            src/linux/timeQuery.c
    )
endif ()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES src/main.c ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})

add_executable(${PROJECT_NAME} src/main.c ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(${PROJECT_NAME} PRIVATE cgltf)

if (WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE -static)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
elseif (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _POSIX_C_SOURCE=202405L)
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
endif ()

enable_testing()

set(TEST_NAME ${PROJECT_NAME}_Tests)

set(TEST_SOURCE_FILES
        test/graphicsMath.c
)

add_test_executable(${TEST_NAME}
        ${SOURCE_FILES}
        ${TEST_SOURCE_FILES}
)

target_link_libraries(${TEST_NAME} PRIVATE cgltf)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if (WIN32)
    target_link_options(${TEST_NAME} PRIVATE -static)
    target_compile_definitions(${TEST_NAME} PRIVATE NOMINMAX)
elseif (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(${TEST_NAME} PRIVATE X11)
    target_link_libraries(${TEST_NAME} PRIVATE m)
    target_compile_definitions(${TEST_NAME} PRIVATE _POSIX_C_SOURCE=202405L)
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${TEST_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
endif ()

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
function(add_test_executable TARGET)
    set(REGISTRY_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}TestCaseRegistry.c)

    set(ABSOLUTE_PATHS "")
    foreach (SOURCE_FILE ${ARGN})
        get_filename_component(ABSOLUTE_PATH "${SOURCE_FILE}" ABSOLUTE)
        list(APPEND ABSOLUTE_PATHS "${ABSOLUTE_PATH}")
    endforeach ()

    add_custom_command(
            OUTPUT ${REGISTRY_FILE}
            COMMAND ${CMAKE_COMMAND}
            -D OUT_FILE=${REGISTRY_FILE}
            -D SOURCE_FILES="${ABSOLUTE_PATHS}"
            -D SOURCE_PATHS="${ARGN}"
            -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/generateTestCaseRegistry.cmake
            DEPENDS ${ABSOLUTE_PATHS}
    )

    add_executable(${TARGET} ${ARGN} ${REGISTRY_FILE})
    target_link_libraries(${TARGET} PRIVATE TestFramework)

    foreach (SOURCE_FILE ${ARGN})
        string(REGEX REPLACE "[^a-zA-Z0-9]" "_" TEST_FILE_PREFIX "${SOURCE_FILE}")
        set_property(SOURCE "${SOURCE_FILE}" APPEND PROPERTY COMPILE_DEFINITIONS "TEST_FILE_PREFIX=${TEST_FILE_PREFIX}")
    endforeach ()

    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove ${REGISTRY_FILE}
    )
endfunction()

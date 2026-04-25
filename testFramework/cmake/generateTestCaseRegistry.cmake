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
string(REPLACE " " ";" SOURCE_FILES "${SOURCE_FILES}")
string(REPLACE " " ";" SOURCE_PATHS "${SOURCE_PATHS}")

set(DECLARATIONS "")
set(POINTERS_ARRAY_ENTRIES "")

list(LENGTH SOURCE_FILES SOURCE_FILES_COUNT)
math(EXPR SOURCE_FILES_UPPER_BOUND "${SOURCE_FILES_COUNT} - 1")

foreach (I RANGE ${SOURCE_FILES_UPPER_BOUND})
    list(GET SOURCE_FILES ${I} SOURCE_FILE)
    list(GET SOURCE_PATHS ${I} FILE_PATH)
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" TEST_NAME_PREFIX "${FILE_PATH}")

    file(STRINGS "${SOURCE_FILE}" LINES)
    set(IN_BLOCK_COMMENT FALSE)
    foreach (LINE ${LINES})
        if (IN_BLOCK_COMMENT)
            if (LINE MATCHES "\\*/")
                set(IN_BLOCK_COMMENT FALSE)
            endif ()
            continue()
        endif ()
        if (LINE MATCHES "/\\*")
            set(IN_BLOCK_COMMENT TRUE)
            continue()
        endif ()
        string(REGEX MATCH "^[ \t]*//" IS_LINE_COMMENT "${LINE}")
        if (IS_LINE_COMMENT)
            continue()
        endif ()
        if (NOT LINE MATCHES "(^|[^a-zA-Z0-9_])TEST\\(")
            continue()
        endif ()
        string(REGEX MATCH "(^|[^a-zA-Z0-9_])TEST\\(([^)]+)\\)" _ "${LINE}")
        set(TEST_NAME "${CMAKE_MATCH_2}")
        set(FULL_FUNCTION_NAME "${TEST_NAME_PREFIX}__${TEST_NAME}")
        string(APPEND DECLARATIONS "void ${FULL_FUNCTION_NAME}(TestEntry*);\n")
        string(APPEND POINTERS_ARRAY_ENTRIES "    {\"${TEST_NAME}\", \"${FILE_PATH}\",
            ${FULL_FUNCTION_NAME}, TestResult_Success},\n")
    endforeach ()
endforeach ()

file(WRITE "${OUT_FILE}"
        "#include <stddef.h>\n"
        "#include \"testFramework.h\"\n"
        "${DECLARATIONS}"
        "TestEntry tests[] = {\n"
        "${POINTERS_ARRAY_ENTRIES}"
        "};\n"
        "const size_t testCount = sizeof(tests) / sizeof(tests[0]);\n"
)

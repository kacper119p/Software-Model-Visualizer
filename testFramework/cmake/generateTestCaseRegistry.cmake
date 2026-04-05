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

    file(STRINGS "${SOURCE_FILE}" LINES REGEX "^TEST\\(")
    foreach (LINE ${LINES})
        string(REGEX MATCH "TEST\\(([^)]+)\\)" _ "${LINE}")
        set(TEST_NAME "${CMAKE_MATCH_1}")
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

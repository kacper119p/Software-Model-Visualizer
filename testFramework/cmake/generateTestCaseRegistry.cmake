string(REPLACE " " ";" SOURCES "${SOURCES}")

set(FORWARDS "")
set(ENTRIES "")

foreach (SRC ${SOURCES})
    file(STRINGS "${SRC}" LINES REGEX "^TEST\\(")
    foreach (LINE ${LINES})
        string(REGEX MATCH "TEST\\(([^)]+)\\)" _ "${LINE}")
        set(NAME "${CMAKE_MATCH_1}")
        string(APPEND FORWARDS "void ${NAME}(TestEntry*);\n")
        string(APPEND ENTRIES "    {\"${NAME}\", \"${SRC}\", ${NAME}, TestResult_Success},\n")
    endforeach ()
endforeach ()

file(WRITE "${OUT_FILE}"
        "#include <stddef.h>\n"
        "#include \"testFramework.h\"\n"
        "${FORWARDS}"
        "TestEntry tests[] = {\n"
        "${ENTRIES}"
        "};\n"
        "const size_t testCount = sizeof(tests) / sizeof(tests[0]);\n"
)

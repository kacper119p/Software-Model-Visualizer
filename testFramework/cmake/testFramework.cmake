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

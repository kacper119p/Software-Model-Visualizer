function(add_test_executable TARGET)
    set(REGISTRY_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_registry.c)

    set(ABS_SOURCES "")
    foreach (SRC ${ARGN})
        get_filename_component(ABS_SRC "${SRC}" ABSOLUTE)
        list(APPEND ABS_SOURCES "${ABS_SRC}")
    endforeach ()

    add_custom_command(
            OUTPUT ${REGISTRY_FILE}
            COMMAND ${CMAKE_COMMAND}
            -DOUT_FILE=${REGISTRY_FILE}
            -DSOURCES="${ABS_SOURCES}"
            -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/generateTestCaseRegistry.cmake
            DEPENDS ${ABS_SOURCES}
    )

    add_executable(${TARGET} ${ARGN} ${REGISTRY_FILE})
    target_link_libraries(${TARGET} PRIVATE TestFramework)

    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove ${REGISTRY_FILE}
    )
endfunction()

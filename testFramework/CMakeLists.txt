set(LIBRARY_NAME TestFramework)

set(SOURCE_FILES
        src/main.c
)

set(HEADER_FILES
        inc/ansiColors.h
        inc/testFramework.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})

add_library(${LIBRARY_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if (WIN32)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE NOMINMAX)
elseif (UNIX AND NOT APPLE)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE _POSIX_C_SOURCE=202405L)
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${LIBRARY_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
endif ()

set(SOURCE_FILES
        src/main.c
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

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

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

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
endif ()

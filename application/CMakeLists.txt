file(GLOB_RECURSE SOURCE_FILES
        "src/*.c")
file(GLOB_RECURSE HEADER_FILES
        "inc/*.h")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(${PROJECT_NAME} PRIVATE cglm)
target_link_libraries(${PROJECT_NAME} PRIVATE cgltf)

target_link_options(${PROJECT_NAME} PRIVATE -static)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
endif ()

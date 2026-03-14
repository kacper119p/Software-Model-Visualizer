file(GLOB_RECURSE SOURCE_FILES
        "src/*.c")
file(GLOB_RECURSE HEADER_FILES
        "inc/*.h")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE rasterizer)


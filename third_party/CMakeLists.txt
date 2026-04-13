set(CGLTF_DIR ${CMAKE_CURRENT_LIST_DIR}/cgltf)
set(CGLTF_SOURCE_DIR ${CGLTF_DIR} CACHE INTERNAL "")
add_library(cgltf STATIC ${CGLTF_DIR}/cgltf.c)
target_include_directories(cgltf PUBLIC ${CGLTF_DIR})

set_target_properties(
        cgltf
        PROPERTIES FOLDER "third_party")

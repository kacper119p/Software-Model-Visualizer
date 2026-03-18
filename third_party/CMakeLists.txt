set(CGLTF_DIR ${CMAKE_CURRENT_LIST_DIR}/cgltf)
set(CGLTF_SOURCE_DIR ${CGLTF_DIR} CACHE INTERNAL "")
add_library(cgltf STATIC ${CGLTF_DIR}/cgltf.c)
target_include_directories(cgltf PUBLIC ${CGLTF_DIR})

CPMAddPackage(
        NAME cglm
        GITHUB_REPOSITORY recp/cglm
        GIT_TAG v0.9.6
)

set_target_properties(
        cglm
        cgltf
        PROPERTIES FOLDER "third_party")

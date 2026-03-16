CPMAddPackage(
        NAME cglm
        GITHUB_REPOSITORY recp/cglm
        GIT_TAG v0.9.6
)

CPMAddPackage(
        NAME cgltf
        GITHUB_REPOSITORY jkuhlmann/cgltf
        GIT_TAG v1.15
)
    add_library(cgltf INTERFACE)
    target_include_directories(cgltf INTERFACE ${cgltf_SOURCE_DIR})

set_target_properties(
        cglm
        cgltf
        PROPERTIES FOLDER "third_party")

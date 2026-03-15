CPMAddPackage(
        NAME cglm
        GITHUB_REPOSITORY recp/cglm
        GIT_TAG v0.9.6
)

set_target_properties(
        cglm
        PROPERTIES FOLDER "ThirdParty")

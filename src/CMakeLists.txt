# define target
add_subdirectory(src)

set(ROOT_DEPENDENCIES Core EG Gpad Graf Hist MathCore RIO Tree)

get_directory_property(incdirs INCLUDE_DIRECTORIES)
generate_dictionary(MixingManager "MixingManagerLinkDef.h" "MixingManager.h" "${incdirs}")

add_executable(MixingManager.x  MixingManager.h MixingManagerLinkDef.h G__MixingManager.cxx)
target_link_libraries(MixingManager.x
    PUBLIC
    ${ROOT_DEPENDENCIES}
    src
    PRIVATE
    gsl)

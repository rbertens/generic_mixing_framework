# define target
add_subdirectory(runEventMixer)

set(ROOT_DEPENDENCIES Core EG Gpad Graf Hist MathCore RIO Tree)

get_directory_property(incdirs INCLUDE_DIRECTORIES)
generate_dictionary(runEventMixer "runEventMixerLinkDef.h" "runEventMixer.h" "${incdirs}")

add_executable(runEventMixer.x  runEventMixer.h runEventMixerLinkDef.h G__runEventMixer.cxx)
target_link_libraries(runEventMixer.x
    PUBLIC
    ${ROOT_DEPENDENCIES}
    runEventMixer)

set(MODULE src)

set(SRCS
    ${CMAKE_SOURCE_DIR}/src/${MODULE}/AliGMFEventContainer.cxx
    ${CMAKE_SOURCE_DIR}/src/${MODULE}/AliGMFEventReader.cxx
    ${CMAKE_SOURCE_DIR}/src/${MODULE}/AliGMFHistogramManager.cxx
    ${CMAKE_SOURCE_DIR}/src/${MODULE}/AliGMFMixingManager.cxx
    ${CMAKE_SOURCE_DIR}/src/${MODULE}/AliGMFSimpleTrackCuts.cxx
    ${CMAKE_SOURCE_DIR}/src/${MODULE}/AliGMFTTreeHeader.cxx
    ${CMAKE_SOURCE_DIR}/src/${MODULE}/AliGMFTTreeTrack.cxx
    )
# provide the headers
string(REPLACE "*.cxx" ".h" HDRS "${SRCS}")

set(ROOT_DEPENDENCIES Core Gpad Graf Hist MathCore Tree)

get_directory_property(incdirs INCLUDE_DIRECTORIES)
generate_dictionary("${MODULE}" "${CMAKE_SOURCE_DIR}/src/${MODULE}/${MODULE}LinkDef.h" "${HDRS}" "${incdirs}")

# Create an object to be reused in case of static libraries
# Otherwise the sources will be compiled twice
add_library(${MODULE} SHARED ${SRCS} ${HDRS} ${MODULE}LinkDef.h G__${MODULE}.cxx)
#target_link_libraries(${MODULE}
#  PUBLIC ${ROOT_DEPENDENCIES}
#  PRIVATE gsl)

install(TARGETS ${MODULE}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)

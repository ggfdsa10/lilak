set(LILAK_SOURCE_DIRECTORY_LIST ${LILAK_SOURCE_DIRECTORY_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/detector
    ${CMAKE_CURRENT_SOURCE_DIR}/tool
    ${CMAKE_CURRENT_SOURCE_DIR}/task
    ${CMAKE_CURRENT_SOURCE_DIR}/container
    ${CMAKE_CURRENT_SOURCE_DIR}/tool/GETDecoder/source/GETCore
    ${CMAKE_CURRENT_SOURCE_DIR}/tool/GETDecoder/source/GETFormat
    ${CMAKE_CURRENT_SOURCE_DIR}/tool/GETDecoder/source/GETUtil
    CACHE INTERNAL ""
)

set(LILAK_SOURCE_DIRECTORY_LIST_XLINKDEF ${LILAK_SOURCE_DIRECTORY_LIST_XLINKDEF}
    CACHE INTERNAL ""
)

set(LILAK_GEANT4_SOURCE_DIRECTORY_LIST ${LILAK_GEANT4_SOURCE_DIRECTORY_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/geant4
    CACHE INTERNAL ""
)

file(GLOB MACROS_FOR_EXECUTABLE_PROCESS ${CMAKE_CURRENT_SOURCE_DIR}/macros*/*.cc)

set(LILAK_EXECUTABLE_LIST ${LILAK_EXECUTABLE_LIST}
    ${MACROS_FOR_EXECUTABLE_PROCESS}
    CACHE INTERNAL ""
)
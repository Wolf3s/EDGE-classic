##########################################
# Edge Classic - CMake Utilities
##########################################

# target for generating epk packs
add_custom_target(GenerateEPKS)

# edge defs pack
add_custom_command(
    TARGET GenerateEPKS    
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/edge-defs.epk" --format=zip "."
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/edge_defs"
)

# game/mod packs
set (EPK_BASE_SOURCE    
    "edge_base/blasphemer"
    "edge_base/chex"
    "edge_base/doom"
    "edge_base/doom1"
    "edge_base/doom2"
    "edge_base/freedoom1"
    "edge_base/freedoom2"
    "edge_base/hacx"    
    "edge_base/heretic"
    "edge_base/plutonia"
    "edge_base/rekkr"
    "edge_base/tnt"    
) 

function( add_epk EPK_PATH )
    add_custom_command(
        TARGET GenerateEPKS
        COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/${EPK_PATH}.epk" --format=zip "."
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${EPK_PATH}"
    )
endfunction()

foreach( epk ${EPK_BASE_SOURCE})
    add_epk(${epk})
endforeach( epk )


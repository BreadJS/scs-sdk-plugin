# Source files
set(SOURCE_FILES
    src/sharedmemory.cpp
    src/sharedmemory_platform.cpp
    src/scs_telemetry.cpp
    src/scs_config_handlers.cpp
    src/scs_gameplay_event_handlers.cpp
)

# Optional logging
option(BUILD_WITH_LOGGING "Enable logging support" OFF)
if(BUILD_WITH_LOGGING)
    list(APPEND SOURCE_FILES src/log.cpp)
    add_definitions(-DLOGGING)
endif()

# Create library
add_library(scs-telemetry SHARED ${SOURCE_FILES})

# Include directories
target_include_directories(scs-telemetry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/scs_sdk/include
)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(scs-telemetry PRIVATE
        SCS_TELEMETRY_EXPORTS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )

    # Set output name
    set_target_properties(scs-telemetry PROPERTIES
        OUTPUT_NAME "scs-telemetry"
        SUFFIX ".dll"
        PREFIX ""
    )

    # Export symbols
    set(DEF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vs2012/scs-telemetry.def)
    if(EXISTS ${DEF_FILE})
        target_sources(scs-telemetry PRIVATE ${DEF_FILE})
    endif()

elseif(UNIX AND NOT APPLE)
    # Linux-specific settings
    target_compile_definitions(scs-telemetry PRIVATE
        _GNU_SOURCE
    )

    # Link pthread (required on Linux)
    target_link_libraries(scs-telemetry PRIVATE pthread)

    # Set visibility for exported symbols
    target_compile_options(scs-telemetry PRIVATE -fvisibility=hidden)
    target_compile_definitions(scs-telemetry PRIVATE
        SCS_TELEMETRY_EXPORTS
    )

    # Set output name
    set_target_properties(scs-telemetry PROPERTIES
        OUTPUT_NAME "scs-telemetry"
        SUFFIX ".so"
        PREFIX ""
    )

    # Create symbol version script for exports
    set(EXPORTS_FILE ${CMAKE_CURRENT_BINARY_DIR}/exports.txt)
    file(WRITE ${EXPORTS_FILE}
"{\n  global:\n    scs_telemetry_init;\n    scs_telemetry_shutdown;\n  local: *;\n};"
    )
    set_target_properties(scs-telemetry PROPERTIES
        LINK_FLAGS "-Wl,--version-script=${EXPORTS_FILE}"
    )
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(scs-telemetry PRIVATE /W4)
else()
    target_compile_options(scs-telemetry PRIVATE -Wall -Wextra -pedantic)
endif()

# Platform-specific compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(scs-telemetry PRIVATE
        -fPIC
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-g -O0>
    )
endif()

set(THIRD_PARTY_ARCHITECTURE_CMAKE
    "${CMAKE_CURRENT_LIST_DIR}/architectures/${CMAKE_SYSTEM_PROCESSOR}/CMakeLists.txt"
)

if(NOT EXISTS "${THIRD_PARTY_ARCHITECTURE_CMAKE}")
    message(FATAL_ERROR
        "No third-party configuration exists for architecture '${CMAKE_SYSTEM_PROCESSOR}'. "
        "Expected '${THIRD_PARTY_ARCHITECTURE_CMAKE}'."
    )
endif()

include("${THIRD_PARTY_ARCHITECTURE_CMAKE}")
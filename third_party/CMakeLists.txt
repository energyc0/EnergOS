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
add_library(LFS 
	LittleFS/lfs.c
	LittleFS/lfs_util.c
	lfs_impl.c
)
target_link_libraries(LFS PRIVATE HAL)

set(LFS_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR})
message(STATUS "LFS_CONFIG_DIR: ${LFS_CONFIG_DIR}")

target_include_directories(LFS PUBLIC 
	${LFS_CONFIG_DIR}
	${PROJECT_SOURCE_DIR}
)

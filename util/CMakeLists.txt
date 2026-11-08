add_library(UTILITY STATIC util.c)
target_include_directories(UTILITY PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}
)

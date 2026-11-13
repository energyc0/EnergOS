add_library(UTILITY STATIC util.c slab.c)
target_include_directories(UTILITY PUBLIC
    ${PROJECT_SOURCE_DIR}
)

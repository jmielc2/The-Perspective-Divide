target_sources(main PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c"
)

target_include_directories(main PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/glad/include/"
)
# Configure CMake
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-ggdb -DDEBUG")
set(CMAKE_CXX_FLAGS_INIT "-Wall -Weffc++ -Werror -Wpedantic -Wextra -Wconversion -Wsign-conversion")
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Perspective-Divide")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
project(The-Perspective-Divide)

# Define Macros
macro(create_symlink_dir old new)
    add_custom_command(TARGET main
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${old} ${new}
    ) 
endmacro()

# Find Dependencies
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(glm CONFIG REQUIRED)

# Add Source Files
add_executable(
    main
    "${CMAKE_SOURCE_DIR}/main.cpp"
    "${CMAKE_SOURCE_DIR}/rendering.cpp"
)
add_subdirectory(
    "${CMAKE_SOURCE_DIR}/extern"
)

# Add Inlude Dirs
target_include_directories(main PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

# Link Libraries
target_link_libraries(main PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    glm::glm
)

# Create Symbolic links to needed files
create_symlink_dir("${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets")
create_symlink_dir("${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/shaders")
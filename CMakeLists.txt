cmake_minimum_required(VERSION 3.20)
project(MeowDot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(ImGui REQUIRED)
find_package(Mono REQUIRED)  # For C# scripting

# Include directories
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLFW3_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${IMGUI_INCLUDE_DIRS})
include_directories(${MONO_INCLUDE_DIRS})

# Collect all header files
file(GLOB_RECURSE ENGINE_HEADERS "src/*.h")

# Source files
set(ENGINE_SOURCES
    src/main.cpp
)

# Create executable
add_executable(MeowDot ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Link libraries
target_link_libraries(MeowDot 
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLFW3_LIBRARIES}
    ${IMGUI_LIBRARIES}
    ${MONO_LIBRARIES}
)

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Windows specific
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

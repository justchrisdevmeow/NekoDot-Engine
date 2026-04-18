cmake_minimum_required(VERSION 3.20)
project(MeowDot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(ImGui REQUIRED)
find_package(Mono REQUIRED)
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${IMGUI_INCLUDE_DIRS})
include_directories(${MONO_INCLUDE_DIRS})
include_directories(${NLHOHMANN_JSON_INCLUDE_DIRS})
include_directories(src)

# Collect all header files
file(GLOB_RECURSE ENGINE_HEADERS "src/*.h")

# Source files
set(ENGINE_SOURCES
    src/main.cpp
)

set(EDITOR_SOURCES
    editor/editor.cpp
)

# Create engine executable
add_executable(MeowDot ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_link_libraries(MeowDot 
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${IMGUI_LIBRARIES}
    ${MONO_LIBRARIES}
)

# Create editor executable
add_executable(MeowDotEditor ${EDITOR_SOURCES} ${ENGINE_HEADERS})

target_link_libraries(MeowDotEditor 
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${IMGUI_LIBRARIES}
    ${MONO_LIBRARIES}
    ${NLHOHMANN_JSON_LIBRARIES}
)

# Windows specific
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    
    add_custom_command(TARGET MeowDot POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL_DIR}/SDL2.dll $<TARGET_FILE_DIR:MeowDot>)
    
    add_custom_command(TARGET MeowDotEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL_DIR}/SDL2.dll $<TARGET_FILE_DIR:MeowDotEditor>)
endif()

# Copy assets and shaders to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

cmake_minimum_required(VERSION 3.11)
project(snake-game)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if we're compiling with Emscripten
if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(PLATFORM "Web" CACHE STRING "Platform" FORCE)
endif()

# raylib
set(RAYLIB_VERSION 5.0)
include(FetchContent)
FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(raylib)
if(NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)

    # Disable raylib examples and audio module
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

    # Disable shared libraries
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    # Force OpenGL ES 2.0 for web builds and disable X11
    if(PLATFORM STREQUAL "Web")
        set(GRAPHICS "GRAPHICS_API_OPENGL_ES2" CACHE STRING "" FORCE)
        set(PLATFORM_CPP_FLAGS "-s USE_GLFW=3 -s ASYNCIFY" CACHE STRING "" FORCE)
        set(USE_WAYLAND OFF CACHE BOOL "" FORCE)
        set(USE_X11 OFF CACHE BOOL "" FORCE)
        set(SUPPORT_WINMM_HIGHRES_TIMER OFF CACHE BOOL "" FORCE)
    endif()

    # Add raylib as subdirectory
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
endif()

# Your game
add_executable(${PROJECT_NAME} src/snake-game.cpp)
target_link_libraries(${PROJECT_NAME} raylib)

if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(PLATFORM "Web" CACHE STRING "Platform" FORCE)

    # Add this line to package assets with the Emscripten build
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")
endif()

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# If compiling for Web, also copy assets to where the HTML file is
if(EMSCRIPTEN)
    file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/assets)
endif()
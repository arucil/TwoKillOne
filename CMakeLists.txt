cmake_minimum_required(VERSION 3.12)
project(TwoKillOne)

set(CMAKE_CXX_STANDARD 17)

file(GLOB sdl2pp_files "sdl2pp/src/*.cpp")

add_executable(TwoKillOne ${sdl2pp_files} src/main.cpp src/Game.cpp src/Game.h src/Board.cpp src/Board.h src/events.h)

include_directories(${CMAKE_SOURCE_DIR}/sdl2pp/include)
add_definitions(-DSDL2PP_USE_IMAGE)

if (EMSCRIPTEN)
    # Add a dummy library so the executable will be relinked when game.html is changed
    # See https://blog.squareys.de/cmake-relink-js/
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/for_detecting_html_change.cpp
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/for_detecting_html_change.cpp
            DEPENDS src/game.html)

    add_library(for_detecting_html_change STATIC src/game.html
            ${CMAKE_CURRENT_BINARY_DIR}/for_detecting_html_change.cpp)

    target_link_libraries(TwoKillOne PRIVATE for_detecting_html_change)

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}\
            -s WASM=1 -s USE_SDL=2\
            -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]'\
            -s DISABLE_EXCEPTION_CATCHING=0\
            --preload-file ../assets\
            -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\"]'\
            --shell-file ${CMAKE_SOURCE_DIR}/src/game.html")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

else()
    # Native build doesn't work properly, because the Game object needs to interact with the HTML page.

    target_link_libraries(${PROJECT_NAME} -lSDL2 -lSDL2_image)

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()
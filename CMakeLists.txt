include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER ${MINGW32_PATH}gcc.exe)
set(CMAKE_CXX_COMPILER ${MINGW32_PATH}g++.exe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyProject VERSION 1.0)

find_package(SFML 2.6.0 COMPONENTS system window graphics REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${IMGUI} ${IMGUI_SFML} ${MY_UTILS_PATH} ${MY_UTILS_PATH})

add_executable(MyProject
  ${SRC_DIR}main.cpp
  ${SRC_DIR}utils/ThreadPool.cpp
  ${SRC_DIR}utils/config.cpp
  ${SRC_DIR}physics/Physics.cpp
  ${SRC_DIR}render/Render.cpp
  ${MY_UTILS_PATH}myutils.cpp
  ${IMGUI}imgui.cpp
  ${IMGUI}imgui_widgets.cpp
  ${IMGUI}imgui_draw.cpp
  ${IMGUI}imgui_tables.cpp
  ${IMGUI_SFML}imgui-SFML.cpp
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${SRC_DIR}pch.h)
target_link_libraries(MyProject sfml-system sfml-window sfml-graphics ${OPENGL_LIBRARIES})


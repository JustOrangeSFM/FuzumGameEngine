cmake_minimum_required(VERSION 3.10)

project(GameEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Targets
option(EDITOR "Build the editor" ON)
option(GAME "Build the game" OFF)


if(EDITOR)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin/editor")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/editor)
endif()
if(GAME)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin/game")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/game)
    set(GAMES_DIR "${CMAKE_SOURCE_DIR}/Games")
    include_directories(${GAMES_DIR})
endif()

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/third_party/imgui")
set(FREEGLUT_DIR "${CMAKE_SOURCE_DIR}/third_party/freeglut/include")
set(GLEW_DIR "${CMAKE_SOURCE_DIR}/third_party/glew/include")
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/third_party/glfw/include")
set(SDL_DIR "${CMAKE_SOURCE_DIR}/third_party/sdl3/include")
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/third_party/glad/include")
set(GLAD_SRC_DIR "${CMAKE_SOURCE_DIR}/third_party/glad/src")
set(KHR_DIR "${CMAKE_SOURCE_DIR}/third_party/KHR/include")
set(FREETYPE_DIR "${CMAKE_SOURCE_DIR}/third_party/freetype/include")
set(GLM_DIR "${CMAKE_SOURCE_DIR}/third_party/glm")
set(LINEMATCH_DIR "${CMAKE_SOURCE_DIR}/third_party/linmath")
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/third_party/assimp/include/assimp")
set(ASSIMP2_DIR "${CMAKE_SOURCE_DIR}/third_party/assimp/build/x64/include/assimp")
set(STB_DIR "${CMAKE_SOURCE_DIR}/third_party/stb/include")
#set(ZLIB_DIR "${CMAKE_SOURCE_DIR}/third_party/zlib")

set(GIZMO_DIR "${CMAKE_SOURCE_DIR}/third_party/LibGizmo")

file(GLOB SOURCES "src/main.cpp")
file(GLOB SOURCES3 "src/Base.h")
file(GLOB SOURCES2 "Games/*.cpp")
file(GLOB SOURCES4 "Redirections.h")
file(GLOB GL_SOURCES "src/GL/*.cpp")
list(APPEND SOURCES ${GL_SOURCES})
list(APPEND SOURCES ${SOURCES2})
list(APPEND SOURCES ${SOURCES3})
list(APPEND SOURCES ${SOURCES4})
if(EDITOR)
    file(GLOB EDITOR_SOURCES "src/editor/*.cpp")
    list(APPEND SOURCES ${EDITOR_SOURCES})
endif()
if(GAME)
    file(GLOB GAME_SOURCES "src/game/*.cpp")
    list(APPEND SOURCES ${GAME_SOURCES})
endif()

file(GLOB IMGUI_SOURCES "${IMGUI_DIR}/*.cpp")
file(GLOB IMGUI_SOURCES2 "${IMGUI_DIR}/*.h")
list(APPEND IMGUI_SOURCES ${IMGUI_SOURCES2})

file(GLOB IMGUI_SOURCES_BACKENDS
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
)
file(GLOB IMGUI_HEADERS_BACKENDS
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.h"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.h"
)
list(APPEND IMGUI_SOURCES ${IMGUI_SOURCES_BACKENDS} ${IMGUI_HEADERS_BACKENDS})

link_directories("${CMAKE_SOURCE_DIR}/third_party/libs")
include_directories(${IMGUI_DIR})
include_directories(${FREEGLUT_DIR})
include_directories(${GLEW_DIR})
include_directories(${GLFW_DIR})
include_directories(${SDL_DIR})
include_directories(${GLAD_DIR})
include_directories(${GLAD_SRC_DIR})
file(GLOB KHR "${KHR_DIR}/*.h")
include_directories(${KHR_DIR})
include_directories(${FREETYPE_DIR})
include_directories(${GLM_DIR})
include_directories(${LINEMATCH_DIR})
include_directories(${ASSIMP_DIR})
include_directories(${ASSIMP2_DIR})
include_directories(${STB_DIR})
#include_directories(${ZLIB_DIR})

include_directories(${GIZMO_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src/Base.h)

#--------------------------------------------------------------------
# Hide the console window in visual studio projects
#--------------------------------------------------------------------
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

#--------------------------------------------------------------------
# Hide the console window in visual studio projects - Release
#--------------------------------------------------------------------
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

if(SOURCES)
    # Теперь создадим библиотеку imgui
    add_library(Imgui STATIC
        ${IMGUI_SOURCES}
        ${IMGUI_SOURCES2}
        ${IMGUI_SOURCES_BACKENDS}  # Если используете OpenGL
    )

    # Указываем директории для инклюдов
    target_include_directories(Imgui
        PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends  # Если используете backends
    )

    # Линковка сторонних библиотек
    target_link_libraries(Imgui
        PRIVATE
    )

    if(EDITOR)
         add_executable(Editor ${SOURCES} ${GLAD_SRC_DIR}/glad.c)
         target_compile_definitions(Editor PRIVATE EDITOR_MODE)

         add_custom_command(TARGET Editor POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different
         "${CMAKE_SOURCE_DIR}/third_party/libs/assimp-vc142-mt.dll"
         $<TARGET_FILE_DIR:Editor>)

         # Линковка библиотеки imgui с исполняемым файлом
         target_link_libraries(Editor Imgui glut32.lib freeglut.lib glfw3.lib opengl32.lib glu32.lib glew32.lib assimp-vc142-mt.lib ) #zlib.lib
    endif()
    if(GAME)
         add_executable(Game ${SOURCES} ${GLAD_SRC_DIR}/glad.c)
         target_compile_definitions(Game PRIVATE GAME_MODE)

         add_custom_command(TARGET Game POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different
         "${CMAKE_SOURCE_DIR}/third_party/libs/assimp-vc142-mt.dll"
         $<TARGET_FILE_DIR:Game>)

         # Линковка библиотеки imgui с исполняемым файлом
         target_link_libraries(Game Imgui glut32.lib freeglut.lib glfw3.lib opengl32.lib glu32.lib glew32.lib assimp-vc142-mt.lib) #zlib.lib
    endif()
else()
    message(FATAL_ERROR "No source files found in src directory!")
endif()
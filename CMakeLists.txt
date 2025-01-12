cmake_minimum_required(VERSION 3.7)
project(app)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_DEBUGGER_WORKING_DIRECTORY $<CONFIG>)

add_executable(${CMAKE_PROJECT_NAME})
set(SRC_FILES
    src/camera.cpp
    src/input.cpp
    src/logic.cpp
    src/main.cpp
    src/mesh.cpp
    src/monk_math.cpp
    src/render.cpp
    src/resources.cpp
    # Add/remove files here
)

set(VERTEX_SHADERS
	assets/vs_cubes.sc
	assets/vs_mesh.sc
	#assets/vs_sprite.sc
)

set(FRAGMENT_SHADERS
	assets/fs_cubes.sc
	assets/fs_mesh.sc
	#assets/fs_sprite.sc
)

set(VARYING_FILE
	assets/varying.def.sc
)

# Add the Dear ImGui library (STATIC or SHARED as you prefer)
set(IMGUI_FILES
    3rdparty/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui.cpp
    3rdparty/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui_draw.cpp
    3rdparty/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui_tables.cpp
    3rdparty/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui_widgets.cpp
    3rdparty/bgfx.cmake/bgfx/examples/common/imgui/imgui.cpp
)

# The list of files you want to copy
set(COPY_FILES
    assets/teapot.mobj
    assets/NewSponza_Main_Yup_003.fbx
)

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${SRC_FILES} ${IMGUI_FILES} ${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${VARYING_FILE} ${COPY_FILES})

# For each file, add a POST_BUILD step to copy it to the executable's folder
foreach(FILE_TO_COPY IN LISTS COPY_FILES)
    add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_TO_COPY}"
            "$<CONFIG>/${FILE_TO_COPY}"
        COMMENT "Copying ${FILE_TO_COPY} to output directory"
    )
endforeach()


#add_subdirectory("3rdparty/bgfx.cmake/bx")

add_subdirectory("3rdparty/bgfx.cmake")

set(BUILD_SHARED_LIBS 			OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS 			OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL 				OFF CACHE BOOL "" FORCE)
add_subdirectory("3rdparty/assimp")

set(GLFW_BUILD_DOCS 	OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS 	OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL 		OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS 	OFF CACHE BOOL "" FORCE)
add_subdirectory("3rdparty/glfw")

add_subdirectory("3rdparty/glm")


#if (MSVC)
#    target_compile_options(imgui PRIVATE /Zc:__cplusplus)
#	target_compile_options(imgui PRIVATE /Zc:preprocessor)
#endif()

#target_compile_definitions(imgui
#  PRIVATE
#    $<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>
#    $<$<CONFIG:Release>:BX_CONFIG_DEBUG=0>
#    $<$<CONFIG:RelWithDebInfo>:BX_CONFIG_DEBUG=0>
#    $<$<CONFIG:MinSizeRel>:BX_CONFIG_DEBUG=0>
#)

bgfx_compile_shaders(
	TYPE VERTEX
	SHADERS ${VERTEX_SHADERS}
	VARYING_DEF ${PROJECT_SOURCE_DIR}/${VARYING_FILE}
	OUTPUT_DIR $<CONFIG>/shaders
	INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/3rdparty/bgfx.cmake/bgfx/src
#	[AS_HEADERS]
)

bgfx_compile_shaders(
	TYPE FRAGMENT
	SHADERS ${FRAGMENT_SHADERS}
	VARYING_DEF ${PROJECT_SOURCE_DIR}/${VARYING_FILE}
	OUTPUT_DIR $<CONFIG>/shaders
	INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/3rdparty/bgfx.cmake/bgfx/src
#	[AS_HEADERS]
)

#target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${SRC_FILES} ${IMGUI_FILES} ${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${VARYING_FILE})

#target_include_directories(imgui PUBLIC "3rdparty/bgfx.cmake/bgfx/3rdparty/dear-imgui")

target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "3rdparty/bgfx.cmake/bx/include")
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "3rdparty/bgfx.cmake/bimg/include")
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "3rdparty/glm")
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "3rdparty/bgfx.cmake/bgfx/examples/common")
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "3rdparty/bgfx.cmake/bgfx/3rdparty")


target_link_libraries("${CMAKE_PROJECT_NAME}" bx bimg bgfx assimp glfw)
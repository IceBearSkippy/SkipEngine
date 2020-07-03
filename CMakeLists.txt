cmake_minimum_required (VERSION 3.16)
if (UNIX)
    set( CMAKE_C_COMPILER ${CMAKE_C_COMPILER_ID})
    set( CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER_ID})
endif()

set( APP_NAME SkipEngineDemo )

project( APP_NAME )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


set ( SOURCE_FOLDER ./src )
set ( INCLUDE_FOLDER ./include )
set ( LIBS_FOLDER ./libs )

if(DEFINED ENV{VULKAN_SDK})
    message( STATUS "Found sourced env variable VULKAN_SDK ")
    set ( VULKAN_SDK $ENV{VULKAN_SDK} )
else()
    message( FATAL_ERROR "You must execute `source source-env.sh`" )
endif()

if (UNIX)
    find_package( PkgConfig REQUIRED )
endif()

find_package( Vulkan REQUIRED )
find_package( glfw3 REQUIRED )
find_package( glm REQUIRED )
find_package( tinyobjloader REQUIRED )

list ( APPEND PROJECT_INCLUDE_DIRECTORIES
  ${SOURCE_FOLDER}
  ${LIBS_FOLDER}
  ${VULKAN_SDK}/include
  ${INCLUDE_FOLDER}
)

list ( APPEND PROJECT_EXECUTABLE_FILES
  ${SOURCE_FOLDER}/VulkanWindow.cpp
  ${SOURCE_FOLDER}/VulkanDevice.cpp
  ${SOURCE_FOLDER}/VulkanSwapchain.cpp
  ${SOURCE_FOLDER}/objects/SkipObject.cpp
  ${SOURCE_FOLDER}/objects/Model.cpp
  ${SOURCE_FOLDER}/Camera.cpp
  ${SOURCE_FOLDER}/VulkanManager.cpp
  ${SOURCE_FOLDER}/main.cpp

)

# include files
include_directories( ${PROJECT_INCLUDE_DIRECTORIES} )

# copy over resources directory
file(COPY ./resources DESTINATION ${CMAKE_BINARY_DIR})

# target
add_executable( ${APP_NAME} ${PROJECT_EXECUTABLE_FILES} )

find_library( VULKAN_SDK
  NAMES vulkan
  PATHS ${VULKAN_SDK}/lib
)

if(APPLE)
    target_link_libraries( ${APP_NAME} ${VULKAN_SDK}/lib/libvulkan.1.dylib )
else()
    target_link_libraries( ${APP_NAME} ${VULKAN_SDK}/lib/vulkan-1.lib )
endif()
target_link_libraries( ${APP_NAME} tinyobjloader::tinyobjloader )
target_link_libraries( ${APP_NAME} glfw )
target_link_libraries( ${APP_NAME} glm )

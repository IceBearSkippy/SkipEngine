cmake_minimum_required (VERSION 3.8)

project ("SkipEngine")

set ( CMAKE_CXX_FLAGS "-std=c++17 -g" )
set ( APP_NAME SkipEngine )
# TODO Fix glfw link
set ( SOURCE_FOLDER ./src )
set ( INCLUDE_FOLDER ./include )
set ( LIBS_FOLDER ./libs )

set ( VULKAN_SDK C:/VulkanSDK/1.2.135.0 )
find_package ( PkgConfig )
find_package( Vulkan REQUIRED )
find_package( glfw3 REQUIRED )

list ( APPEND PROJECT_INCLUDE_DIRECTORIES 
  ${SOURCE_FOLDER}
  ${INCLUDE_FOLDER}
  ${LIBS_FOLDER}
  ${VULKAN_SDK}/Include
)

list ( APPEND PROJECT_EXECUTABLE_FILES
  ${SOURCE_FOLDER}/VulkanWindow.cpp
  ${SOURCE_FOLDER}/VulkanManager.cpp
  ${SOURCE_FOLDER}/main.cpp
)

# include files
include_directories( ${PROJECT_INCLUDE_DIRECTORIES} )

# target
add_executable( ${APP_NAME} ${PROJECT_EXECUTABLE_FILES} )

find_library( VULKAN_SDK
  NAMES vulkan
  PATHS ${VULKAN_SDK}/Lib
)

target_link_libraries( ${APP_NAME}
  ${VULKAN_SDK}
  glfw
)

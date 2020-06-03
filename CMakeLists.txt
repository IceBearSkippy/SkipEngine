cmake_minimum_required (VERSION 3.8)

project ("SkipEngine")

set ( CMAKE_CXX_FLAGS "-std=c++17 -g" )
set ( APP_NAME SkipEngine )
# TODO Fix glfw link
set ( SOURCE_FOLDER ./src )
set ( INCLUDE_FOLDER ./include )
set ( LIBS_FOLDER ./libs )

set ( VULKAN_SDK C:/VulkanSDK/1.2.135.0 )
set ( GLFW_INCLUDE_DIRECTORIES C:/OpenGL/glfw/include )
set ( GLFW_STATIC_LIBRARIES C:/OpenGL/glfw/lib-vc2019 )
find_package ( PkgConfig )
find_package( Vulkan REQUIRED )

list ( APPEND PROJECT_INCLUDE_DIRECTORIES 
  ${SOURCE_FOLDER}
  ${INCLUDE_FOLDER}
  ${LIBS_FOLDER}
  ${GLFW_INCLUDE_DIRECTORIES}
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
  ${GLFW_STATIC_LIBRARIES}
  ${VULKAN_SDK}
)

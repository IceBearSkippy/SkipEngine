# TODO fix dependency with VULKAN_SDK
cmake_minimum_required (VERSION 3.8)

project ("SkipEngine")

set ( CMAKE_CXX_FLAGS "-std=c++17 -g" )

find_package ( PkgConfig )
find_package( glfw3 REQUIRED )

set ( APP_NAME SkipEngine )

set ( SOURCE_FOLDER ./src )
set ( INCLUDE_FOLDER ./include )
set ( LIBS_FOLDER ./libs )

set ( VULKAN_SDK C:/VulkanSDK/1.2.135.0 )

list ( APPEND PROJECT_INCLUDE_DIRECTORIES 
  ${SOURCE_FOLDER}
  ${INCLUDE_FOLDER}
  ${LIBS_FOLDER}
  ${GLFW_INCLUDE_DIRECTORIES}
)

list ( APPEND PROJECT_EXECUTABLE_FILES
  ${SOURCE_FOLDER}/main.cpp
)

# include files
include_directories( ${PROJECT_INCLUDE_DIRECTORIES} )

# target
add_executable( ${APP_NAME} ${PROJECT_EXECUTABLE_FILES}  "src/VulkanManager.cpp")

find_library( VULKAN_SDK
  NAMES vulkan
  PATHS ${VULKAN_SDK_EXECUTABLES}/lib
)

target_link_libraries( ${APP_NAME}
  ${GLFW_STATIC_LIBRARIES}
  ${VULKAN_SDK}
)

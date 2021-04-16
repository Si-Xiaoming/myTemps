file(GLOB SOURCES "*.h" "*.cpp" "*.hpp" "*.ui" "*.qrc")

add_executable(app ${SOURCES})

include_directories(${CMAKE_CURRENT_LIST_DIR})
find_package(PCL 1.11 REQUIRED)  # COMPONENTS common io visualization
include_directories(${PCL_INCLUDE_DIRS}) 
#把当前的目录添加到搜索目录
#include_directories(${CMAKE_CURRENT_LIST_DIR})   

link_directories(${PCL_LIBRARY_DIRS})                   #5
add_definitions(${PCL_DEFINITIONS})  
target_link_libraries(app PUBLIC Laszip::Laszip ${PCL_LIBRARIES})
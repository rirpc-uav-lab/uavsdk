cmake_minimum_required(VERSION 3.8)
project(uavsdk)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(include)

set(UTILS_LIB_SOURCES include/uavsdk/utils/conversion_functions.hpp include/uavsdk/utils/cpp_custom_extras.hpp)
add_library(utils_lib SHARED ${UTILS_LIB_SOURCES})
set_target_properties(utils_lib PROPERTIES LINKER_LANGUAGE CXX)

set(USEF_DATA_LIB 
include/uavsdk/useful_data_lib/useful_data_interfaces.hpp
include/uavsdk/useful_data_lib/useful_data_impl.hpp
)
add_library(useful_data_lib SHARED ${USEF_DATA_LIB})
set_target_properties(useful_data_lib PROPERTIES LINKER_LANGUAGE CXX)


set(DATA_ADAPTERS_LIB_ROS2
include/uavsdk/data_adapters/cxx/cxx.hpp
include/uavsdk/data_adapters/ros2/geometry_msgs/pose.hpp
include/uavsdk/data_adapters/ros2/geometry_msgs/point32.hpp
include/uavsdk/data_adapters/ros2/geometry_msgs/quaternion.hpp
)
add_library(data_adpters_lib_ros2 SHARED ${DATA_ADAPTERS_LIB_ROS2})
set_target_properties(data_adpters_lib_ros2 PROPERTIES LINKER_LANGUAGE CXX)


set(CMD_MANAGING_LIB 
include/uavsdk/command_manager/command_interface.hpp 
include/uavsdk/command_manager/manager.hpp 
include/uavsdk/command_manager/executors.hpp 
include/uavsdk/command_manager/control_nodes.hpp
include/uavsdk/command_manager/behaviour_interfaces.hpp
)
add_library(cmd_managing_lib SHARED ${CMD_MANAGING_LIB})
set_target_properties(cmd_managing_lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(cmd_managing_lib data_adpters_lib_ros2)


set(KALMAN_FILTERS_LIB_ROS2
include/uavsdk/kalman_filters/ukf/ukf.hpp
)
add_library(kalman_filters_ros2 SHARED ${KALMAN_FILTERS_LIB_ROS2})
set_target_properties(kalman_filters_ros2 PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(kalman_filters_ros2 Eigen3::Eigen)


include_directories(include)

install(
    TARGETS utils_lib
    EXPORT utils_lib
    LIBRARY DESTINATION ${COLCON_PACKAGE_LIB_DESTINATION}
    ARCHIVE DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    RUNTIME DESTINATION  ${COLCON_GLOBAL_LIB_DESTINATION}
    INCLUDES DESTINATION include
)

install(TARGETS utils_lib
DESTINATION lib/${PROJECT_NAME}/)

ament_export_targets(utils_lib HAS_LIBRARY_TARGET)



install(
    TARGETS useful_data_lib
    EXPORT useful_data_lib
    LIBRARY DESTINATION ${COLCON_PACKAGE_LIB_DESTINATION}
    ARCHIVE DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    RUNTIME DESTINATION  ${COLCON_GLOBAL_LIB_DESTINATION}
    INCLUDES DESTINATION include
)

install(TARGETS useful_data_lib
DESTINATION lib/${PROJECT_NAME}/)

ament_export_targets(useful_data_lib HAS_LIBRARY_TARGET)





install(
    TARGETS data_adpters_lib_ros2
    EXPORT data_adpters_lib_ros2
    LIBRARY DESTINATION ${COLCON_PACKAGE_LIB_DESTINATION}
    ARCHIVE DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    RUNTIME DESTINATION  ${COLCON_GLOBAL_LIB_DESTINATION}
    INCLUDES DESTINATION include
)

install(TARGETS data_adpters_lib_ros2
DESTINATION lib/${PROJECT_NAME}/)

ament_export_targets(data_adpters_lib_ros2 HAS_LIBRARY_TARGET)




install(
    TARGETS cmd_managing_lib
    EXPORT cmd_managing_lib
    LIBRARY DESTINATION ${COLCON_PACKAGE_LIB_DESTINATION}
    ARCHIVE DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    RUNTIME DESTINATION  ${COLCON_GLOBAL_LIB_DESTINATION}
    INCLUDES DESTINATION include
)

install(TARGETS cmd_managing_lib
DESTINATION lib/${PROJECT_NAME}/)

ament_export_targets(cmd_managing_lib HAS_LIBRARY_TARGET)




install(
    TARGETS kalman_filters_ros2
    EXPORT kalman_filters_ros2
    LIBRARY DESTINATION ${COLCON_PACKAGE_LIB_DESTINATION}
    ARCHIVE DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    RUNTIME DESTINATION  ${COLCON_GLOBAL_LIB_DESTINATION}
    INCLUDES DESTINATION include
)

install(TARGETS kalman_filters_ros2
DESTINATION lib/${PROJECT_NAME}/)

ament_export_targets(kalman_filters_ros2 HAS_LIBRARY_TARGET)



# add_executable(tests src/tests.cpp)
# target_link_libraries(tests MAVSDK::mavsdk utils_lib useful_data_lib mavsdk_fcu_tel_collector_lib data_adpters_lib_ros2)# backward_object)


# add_executable(inheritance_tests src/inheritance_tests.cpp)
# target_link_libraries(inheritance_tests useful_data_lib backward_object)


add_executable(bh_test src/bh_test.cpp)
target_link_libraries(bh_test cmd_managing_lib)# backward_object)


add_executable(ukf_test src/ukf_test.cpp)
target_link_libraries(ukf_test kalman_filters_ros2)# backward_object)


install(TARGETS
  # tests
  bh_test
  ukf_test
  # command_tests
  # inheritance_tests
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY include/${PROJECT_NAME}
  DESTINATION include
  )



if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(${PROJECT_NAME}_tutorial_test test/tutorial_test.cpp)
  target_include_directories(${PROJECT_NAME}_tutorial_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
  # target_link_libraries(${PROJECT_NAME}_tutorial_test)
endif()

ament_package()

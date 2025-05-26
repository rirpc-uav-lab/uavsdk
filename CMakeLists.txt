cmake_minimum_required(VERSION 3.8)
project(
    uavsdk
    VERSION 0.2.0
    DESCRIPTION
        "uavsdk"
    LANGUAGES CXX
)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)


# Create utils_lib target
set(UTILS_LIB_SOURCES include/uavsdk/utils/conversion_functions.hpp include/uavsdk/utils/cpp_custom_extras.hpp)
add_library(utils_lib SHARED ${UTILS_LIB_SOURCES})
target_include_directories(
    utils_lib 
    # PRIVATE
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>
)
set_target_properties(utils_lib PROPERTIES LINKER_LANGUAGE CXX)

set_target_properties(
    utils_lib
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR}
)


# Create useful_data_lib target
set(USEF_DATA_LIB 
include/uavsdk/useful_data_lib/utils.hpp
include/uavsdk/useful_data_lib/base_interfaces.hpp
include/uavsdk/useful_data_lib/useful_data_interfaces.hpp
include/uavsdk/useful_data_lib/useful_data_impl.hpp
)
add_library(useful_data_lib SHARED ${USEF_DATA_LIB})
target_include_directories(
    useful_data_lib 
    # PRIVATE
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>
)
set_target_properties(useful_data_lib PROPERTIES LINKER_LANGUAGE CXX)

set_target_properties(
    useful_data_lib
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR}
)


# Create data_adpters_lib_ros2 target
set(DATA_ADAPTERS_LIB_ROS2
include/uavsdk/data_adapters/cxx/cxx.hpp include/uavsdk/data_adapters/ros2/geometry_msgs/pose.hpp include/uavsdk/data_adapters/ros2/geometry_msgs/point32.hpp include/uavsdk/data_adapters/ros2/geometry_msgs/quaternion.hpp
)
add_library(data_adpters_lib_ros2 SHARED ${DATA_ADAPTERS_LIB_ROS2})
target_include_directories(
    data_adpters_lib_ros2 
    # PRIVATE
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>
)
set_target_properties(data_adpters_lib_ros2 PROPERTIES LINKER_LANGUAGE CXX)

set_target_properties(
    data_adpters_lib_ros2
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR}
)


# Create cmd_managing_lib target
set(CMD_MANAGING_LIB 
include/uavsdk/command_manager/command_interface.hpp 
include/uavsdk/command_manager/manager.hpp 
include/uavsdk/command_manager/executors.hpp 
include/uavsdk/command_manager/control_nodes.hpp
include/uavsdk/command_manager/behaviour_interfaces.hpp
)
add_library(cmd_managing_lib SHARED ${CMD_MANAGING_LIB})
set_target_properties(cmd_managing_lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(
    cmd_managing_lib 
    # PRIVATE
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>
)
target_link_libraries(cmd_managing_lib data_adpters_lib_ros2)

set_target_properties(
    cmd_managing_lib
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR}
)


# Install all includes into systemwide include directory
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


# Create configuration file
configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/uavsdkConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uavsdkConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/uavsdk
)

write_basic_package_version_file(
    "uavsdkConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(
    TARGETS utils_lib useful_data_lib data_adpters_lib_ros2 cmd_managing_lib
    EXPORT uavsdkTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT uavsdkTargets
    FILE uavsdkTargets.cmake
    NAMESPACE uavsdk::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/uavsdk
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/uavsdkConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/uavsdkConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/uavsdk
)


# Create tests
if (BUILD_TESTING)
    include(CTest)
    find_package(GTest REQUIRED)
    
    add_executable(${PROJECT_NAME}_unimapstr_test test/unimapstr_test.cpp)
    target_include_directories(${PROJECT_NAME}_unimapstr_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(${PROJECT_NAME}_unimapstr_test useful_data_lib GTest::GTest)
    add_test(NAME ${PROJECT_NAME}_unimapstr_test COMMAND ${PROJECT_NAME}_unimapstr_test)

    add_executable(${PROJECT_NAME}_blackboard_test test/blackboard_test.cpp)
    target_include_directories(${PROJECT_NAME}_blackboard_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(${PROJECT_NAME}_blackboard_test useful_data_lib GTest::GTest)
    add_test(NAME ${PROJECT_NAME}_blackboard_test COMMAND ${PROJECT_NAME}_blackboard_test)

    add_executable(${PROJECT_NAME}_blackboard_variable_test test/blackboard_variable_test.cpp)
    target_include_directories(${PROJECT_NAME}_blackboard_variable_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(${PROJECT_NAME}_blackboard_variable_test useful_data_lib GTest::GTest)
    add_test(NAME ${PROJECT_NAME}_blackboard_variable_test COMMAND ${PROJECT_NAME}_blackboard_variable_test)

    add_executable(${PROJECT_NAME}_type_interface_test test/type_interface_test.cpp)
    target_include_directories(${PROJECT_NAME}_type_interface_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(${PROJECT_NAME}_type_interface_test useful_data_lib GTest::GTest)
    add_test(NAME ${PROJECT_NAME}_type_interface_test COMMAND ${PROJECT_NAME}_type_interface_test)
endif()

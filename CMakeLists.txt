cmake_minimum_required(VERSION 3.8)
project(uavsdk)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# find dependencies
find_package(ament_cmake REQUIRED)
find_package(MAVSDK REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)


set(UTILS_LIB_SOURCES include/uavsdk/utils/coversion_functions.hpp)
add_library(utils_lib SHARED ${UTILS_LIB_SOURCES})
set_target_properties(utils_lib PROPERTIES LINKER_LANGUAGE CXX)

set(DACOLLINTER_LIB_SOURCES include/uavsdk/uav_control_interfaces/data_collector_interfaces.hpp)
add_library(data_collector_lib SHARED ${DACOLLINTER_LIB_SOURCES})
set_target_properties(data_collector_lib PROPERTIES LINKER_LANGUAGE CXX)

set(MAVSDK_FCUTELCOLLIB_SOURCES include/uavsdk/fcu_telemetry_collector/mavsdk_fcu_telemetry_collector.hpp)
add_library(mavsdk_fcu_tel_collector_lib SHARED ${MAVSDK_FCUTELCOLLIB_SOURCES})
set_target_properties(mavsdk_fcu_tel_collector_lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(mavsdk_fcu_tel_collector_lib MAVSDK::mavsdk data_collector_lib utils_lib)


add_executable(tests src/tests.cpp)
target_link_libraries(tests MAVSDK::mavsdk utils_lib data_collector_lib mavsdk_fcu_tel_collector_lib)

install(TARGETS
  tests
  DESTINATION lib/${PROJECT_NAME}
)

# install(
#   DIRECTORY include/${PROJECT_NAME}
#   DESTINATION include
#   )

include_directories(include)

ament_export_targets(utils_lib HAS_LIBRARY_TARGET)

install(
  DIRECTORY include/${PROJECT_NAME}
  DESTINATION include
)

install(
    TARGETS utils_lib
    EXPORT utils_lib
    LIBRARY DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    ARCHIVE DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    RUNTIME DESTINATION  ${COLCON_GLOBAL_BIN_DESTINATION}
    INCLUDES DESTINATION include
)

ament_export_targets(data_collector_lib HAS_LIBRARY_TARGET)

install(
    TARGETS data_collector_lib
    EXPORT data_collector_lib
    LIBRARY DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    ARCHIVE DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    RUNTIME DESTINATION  ${COLCON_GLOBAL_BIN_DESTINATION}
    INCLUDES DESTINATION include
)

ament_export_targets(mavsdk_fcu_tel_collector_lib HAS_LIBRARY_TARGET)

install(
    TARGETS mavsdk_fcu_tel_collector_lib
    EXPORT mavsdk_fcu_tel_collector_lib
    LIBRARY DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    ARCHIVE DESTINATION ${COLCON_GLOBAL_LIB_DESTINATION}
    RUNTIME DESTINATION  ${COLCON_GLOBAL_BIN_DESTINATION}
    INCLUDES DESTINATION include
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

ament_package()

cmake_minimum_required(VERSION 3.12)
project(bee-hive-finding VERSION 1.0.0 LANGUAGES CXX)
find_package(atta 0.2.0.0 REQUIRED)

atta_add_target(hive_component "src/hiveComponent.cpp")
atta_add_target(bee_component "src/beeComponent.cpp")
atta_add_target(project_script "src/projectScript.cpp")
target_link_libraries(project_script PRIVATE hive_component bee_component)

atta_add_target(bee_script "src/beeScript.cpp")
target_link_libraries(bee_script PRIVATE hive_component bee_component)

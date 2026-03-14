cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(Software_Model_Visualizer C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(global_settings)
include(CPM)

add_subdirectory(third_party)
add_subdirectory(rasterizer)
add_subdirectory(application)

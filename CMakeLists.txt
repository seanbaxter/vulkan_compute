cmake_minimum_required(VERSION 3.9)
project(compute)

# include_directories(../include)

set(SOURCE_FILES
  compute.cxx
  context.cxx
  VkBootstrap.cpp
)

set_source_files_properties(compute.cxx PROPERTIES COMPILE_FLAGS -shader)

add_executable(compute ${SOURCE_FILES})

target_link_libraries(compute
  glfw
  vulkan
  dl
  pthread
)

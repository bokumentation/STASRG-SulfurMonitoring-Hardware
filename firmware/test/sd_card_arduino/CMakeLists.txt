# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(.)

# Set Minimal Build
idf_build_set_property(MINIMAL_BUILD ON)

# Set extra components on components folder if we want to use it.
# set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/espressif__arduino-esp32")

# Fix compile commands for clangd. remove all '-m*' and '-f*' flags from the compile_commands.json.
if(EXISTS "${CMAKE_SOURCE_DIR}/fix_compile_commands.py")
  add_custom_target(
      fix_clangd ALL
      COMMAND ${CMAKE_COMMAND} -E echo "Running fix_compile_commands.py..."
      COMMAND ${CMAKE_COMMAND} -E env python ${CMAKE_SOURCE_DIR}/fix_compile_commands.py
      COMMENT "Cleaning compile_commands.json for clangd"
      VERBATIM
      )
endif()

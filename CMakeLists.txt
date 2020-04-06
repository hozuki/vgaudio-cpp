cmake_minimum_required(VERSION 3.0)
project(vgaudio)

include(ExternalProject)

include(cmake/common_defines.cmake)
include(cmake/project_directories.cmake)

include(cmake/project_options.cmake)

include(cmake/project_targets.cmake)

include(cmake/project_prebuild.cmake)

include(cmake/ext_proj_zlib.cmake)

include(cmake/project_includes.cmake)
include(cmake/project_compile_options.cmake)

include(cmake/project_link.cmake)
include(cmake/project_link_options.cmake)

include(cmake/project_postbuild.cmake)

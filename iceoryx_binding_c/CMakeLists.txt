# Copyright (c) 2020 by Robert Bosch GmbH, Apex.AI Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.7)

file (STRINGS "../VERSION" iceoryx_binding_c_VERSION)
project(iceoryx_binding_c VERSION ${iceoryx_binding_c_VERSION})

option(test "build test binaries")

find_package(iceoryx_posh REQUIRED)
find_package(iceoryx_utils REQUIRED)

include(IceoryxPackageHelper)
include(IceoryxPlatformDetection)

#
########## set variables for export ##########
#
setup_package_name_and_create_files(
    NAME ${PROJECT_NAME}
    NAMESPACE iceoryx_binding_c
    PROJECT_PREFIX ${PREFIX}
)

#
########## find_package in source tree ##########
#
set(${PROJECT_NAME}_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake
    CACHE FILEPATH
    "${PROJECT_NAME}Config.cmake to make find_package(${PROJECT_NAME}) work in source tree!"
    FORCE
)

#
########## build building-block library ##########
#
add_library(${PROJECT_NAME}
    STATIC
    source/c_user_trigger.cpp
    source/c_publisher.cpp
    source/c_node.cpp
    source/c_runtime.cpp
    source/c_subscriber.cpp
    source/c_trigger_info.cpp
    source/c_wait_set.cpp
    source/cpp2c_enum_translation.cpp
    source/cpp2c_publisher.cpp
    source/cpp2c_subscriber.cpp
)
add_library(${PROJECT_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PREFIX}>
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    iceoryx_posh::iceoryx_posh
    iceoryx_utils::iceoryx_utils
)

target_compile_options(${PROJECT_NAME} PRIVATE ${ICEORYX_WARNINGS})

#
########## build test executables ##########
#
if(test)
    add_subdirectory(test)
endif(test)

#
########## export library ##########
#
setup_install_directories_and_export_package(
    TARGETS ${PROJECT_NAME}
    INCLUDE_DIRECTORY include/
)


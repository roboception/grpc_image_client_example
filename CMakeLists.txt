cmake_minimum_required (VERSION 3.1.0)

project(grpc_image_client)

include(cmake/project_version.cmake)
include(GNUInstallDirs)

# For CMake Version >= 3.0: issue FATAL_ERROR if link dependency contains
# double-colons but is not an imported target.
if (POLICY CMP0028)
  cmake_policy(SET CMP0028 NEW)
endif ()

# - Configuration for code optimization -

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "Build type: DEBUG or RELEASE" FORCE)
endif ()

include(cmake/optimization_flags.cmake)

# - Standard definitions -

add_definitions(-Wall)

# - find protobuf and grpc and compile definitions -

find_package(PkgConfig)

pkg_check_modules(PROTOBUF REQUIRED protobuf)
message(STATUS "Using protobuf ${PROTOBUF_VERSION}")
message(STATUS "PROTOBUF_LIBRARIES ${PROTOBUF_LIBRARIES}")
find_program(_PROTOBUF_PROTOC protoc)

pkg_check_modules(gRPC REQUIRED grpc++)
message(STATUS "Using gRPC ${gRPC_VERSION}")
message(STATUS "gRPC_LIBRARIES ${gRPC_LIBRARIES}")
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

# Proto file
get_filename_component(ii_proto "protobuf_msgs/image_interface.proto" ABSOLUTE)
get_filename_component(ii_proto_path "${ii_proto}" PATH)
message(STATUS "ii_proto ${ii_proto}")
message(STATUS "ii_proto_path ${ii_proto_path}")

# Generated sources
set(ii_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/image_interface.pb.cc")
set(ii_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/image_interface.pb.h")
set(ii_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/image_interface.grpc.pb.cc")
set(ii_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/image_interface.grpc.pb.h")
add_custom_command(
      OUTPUT "${ii_proto_srcs}" "${ii_proto_hdrs}" "${ii_grpc_srcs}" "${ii_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${ii_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${ii_proto}"
      DEPENDS "${ii_proto}")


include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

# example client
add_executable(grpc_image_client
  src/grpc_image_client.cpp
  ${ii_proto_srcs}
  ${ii_grpc_srcs}
)
target_link_libraries(grpc_image_client
    ${PROTOBUF_LIBRARIES}
    ${gRPC_LIBRARIES}
)

install(TARGETS grpc_image_client COMPONENT bin DESTINATION bin)

# - Define information for packaging -
include(cmake/package_debian.cmake)

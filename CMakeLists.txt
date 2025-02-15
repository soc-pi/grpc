cmake_minimum_required(VERSION 3.10)

# Set the project name
project(grpc_app)

# Set gRPC_DIR.  Replace with the actual path to your gRPC installation.
# You might need to adjust this based on where apt installs gRPC.
# Common locations include /usr, /usr/local, or /opt.
# Try running `find /usr /usr/local /opt -name gRPCConfig.cmake` to locate the file.
set(gRPC_DIR /usr/local) #  <---  Adjust this path!

find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)

# Set output paths
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out/lib)

# Add include directories
include_directories(${gRPC_INCLUDE_DIRS} ${Protobuf_INCLUDE_DIRS} include)

# Find the protoc-gen-grpc executable
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin
    HINTS ${gRPC_BINARY_DIR}
    PATH_SUFFIXES bin
    DOC "The gRPC C++ plugin for protoc"
)

if(NOT GRPC_CPP_PLUGIN)
    message(FATAL_ERROR "grpc_cpp_plugin not found.  Please set gRPC_BINARY_DIR to the gRPC build directory.")
endif()

# Enable testing
enable_testing()

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add compiler warnings
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Generate gRPC code
add_custom_command(
    OUTPUT src/common/generated/service.grpc.pb.h src/common/generated/service.pb.h
    COMMAND ${CMAKE_COMMAND} -E make_directory src/common/generated
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            --grpc_out=src/common/generated
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
            -I${CMAKE_SOURCE_DIR}/src/common
            ${CMAKE_SOURCE_DIR}/src/common/service.proto
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            --cpp_out=src/common/generated
            -I${CMAKE_SOURCE_DIR}/src/common
            ${CMAKE_SOURCE_DIR}/src/common/service.proto
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/src/common/service.proto
)

add_custom_target(grpc_proto ALL
    DEPENDS src/common/generated/service.grpc.pb.h src/common/generated/service.pb.h
)

# Client executable
add_executable(grpc_client src/client/client.cpp)
target_link_libraries(grpc_client ${gRPC_LIBRARIES} ${Protobuf_LIBRARIES})
target_include_directories(grpc_client PUBLIC src/common/generated)
add_dependencies(grpc_client grpc_proto)
target_sources(grpc_client PRIVATE src/common/generated/service.grpc.pb.h src/common/generated/service.pb.h)

# Server executable
add_executable(grpc_server src/server/server.cpp)
target_link_libraries(grpc_server ${gRPC_LIBRARIES} ${Protobuf_LIBRARIES})
target_include_directories(grpc_server PUBLIC src/common/generated)
add_dependencies(grpc_server grpc_proto)
target_sources(grpc_server PRIVATE src/common/generated/service.grpc.pb.h src/common/generated/service.pb.h)

# Create a library called "grpc"
add_library(grpc_lib grpc.cpp)

# Specify include directories for the library
target_include_directories(grpc_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

cmake_minimum_required(VERSION 3.5)
project(FileSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(gRPC REQUIRED CONFIG)
find_package(protobuf REQUIRED CONFIG)
set(ABSL_DIR "/usr/local/lib/cmake/absl")
find_package(absl REQUIRED CONFIG)

add_executable(filesystem_server filesystem_server.cpp filesystem.pb.cc filesystem.grpc.pb.cc)
target_link_libraries(filesystem_server 
    PRIVATE 
        gRPC::grpc++ 
        gRPC::grpc++_reflection
        protobuf::libprotobuf
        absl::base
        absl::strings
        absl::status
        absl::statusor
        absl::time
        absl::log
        absl::log_initialize
        absl::log_entry
        absl::log_internal_message
        absl::log_internal_check_op
        absl::log_internal_conditions
        absl::log_internal_format
        absl::log_internal_globals
        absl::log_internal_log_sink_set
        absl::log_internal_nullguard
        absl::log_internal_proto
        absl::log_sink
)

add_executable(filesystem_client filesystem_client.cpp filesystem.pb.cc filesystem.grpc.pb.cc)
target_link_libraries(filesystem_client
    PRIVATE 
        gRPC::grpc++ 
        gRPC::grpc++_reflection
        protobuf::libprotobuf
        absl::base
        absl::strings
        absl::status
        absl::statusor
        absl::time
        absl::log
        absl::log_initialize
        absl::log_entry
        absl::log_internal_message
        absl::log_internal_check_op
        absl::log_internal_conditions
        absl::log_internal_format
        absl::log_internal_globals
        absl::log_internal_log_sink_set
        absl::log_internal_nullguard
        absl::log_internal_proto
        absl::log_sink
)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    /usr/local/include
    /usr/local/Cellar/protobuf/27.1/include
)

# Add library directories
link_directories(
    /usr/local/lib
    /usr/local/Cellar/protobuf/27.1/lib
)

# Print debug information
set(CMAKE_VERBOSE_MAKEFILE ON)
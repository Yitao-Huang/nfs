# nfs

Simple File Server based on gRPC.

## Build
```console
protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` filesystem.proto
protoc -I=. --cpp_out=. filesystem.proto
```
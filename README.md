# nfs

Simple File Server based on gRPC.

## Build
```console
$ git clone https://github.com/Yitao-Huang/nfs.git
$ cd nfs
$ protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` filesystem.proto
$ protoc -I=. --cpp_out=. filesystem.proto
$ mkdir build && cd build && cmake ..
$ ./filesystem_server& ; ./filesystem_client
$ kill -9 ${filesystem_server_pid}
```
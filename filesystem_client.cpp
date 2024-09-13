// filesystem_client.cpp
#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "filesystem.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using filesystem::FileSystem;
using filesystem::ReadFileRequest;
using filesystem::ReadFileResponse;
using filesystem::WriteFileRequest;
using filesystem::WriteFileResponse;
using filesystem::ListFilesRequest;
using filesystem::ListFilesResponse;

class FileSystemClient {
public:
  FileSystemClient(std::shared_ptr<Channel> channel)
    : stub_(FileSystem::NewStub(channel)) {}

  std::string ReadFile(const std::string& filename) {
    ReadFileRequest request;
    request.set_filename(filename);
    ReadFileResponse response;
    ClientContext context;
    Status status = stub_->ReadFile(&context, request, &response);
    if (status.ok()) {
      return response.content();
    } else {
      std::cout << "ReadFile failed: " << status.error_message() << std::endl;
      return "";
    }
  }

  bool WriteFile(const std::string& filename, const std::string& content) {
    WriteFileRequest request;
    request.set_filename(filename);
    request.set_content(content);
    WriteFileResponse response;
    ClientContext context;
    Status status = stub_->WriteFile(&context, request, &response);
    if (status.ok()) {
      return response.success();
    } else {
      std::cout << "WriteFile failed: " << status.error_message() << std::endl;
      return false;
    }
  }

  std::vector<std::string> ListFiles(const std::string& directory) {
    ListFilesRequest request;
    request.set_directory(directory);
    ListFilesResponse response;
    ClientContext context;
    Status status = stub_->ListFiles(&context, request, &response);
    std::vector<std::string> filenames;
    if (status.ok()) {
      for (const auto& filename : response.filenames()) {
        filenames.push_back(filename);
      }
    } else {
      std::cout << "ListFiles failed: " << status.error_message() << std::endl;
    }
    return filenames;
  }

private:
  std::unique_ptr<FileSystem::Stub> stub_;
};

int main(int argc, char** argv) {
  FileSystemClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  
  std::string filename = "test.txt";
  std::string content = "Hello, gRPC!";
  
  if (client.WriteFile(filename, content)) {
    std::cout << "WriteFile success" << std::endl;
  }
  
  std::string file_content = client.ReadFile(filename);
  if (!file_content.empty()) {
    std::cout << "ReadFile success: " << file_content << std::endl;
  }
  
  std::vector<std::string> files = client.ListFiles(".");
  std::cout << "ListFiles success:" << std::endl;
  for (const auto& file : files) {
    std::cout << file << std::endl;
  }

  return 0;
}

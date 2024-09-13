// filesystem_server.cpp
#include <iostream>
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grpcpp/grpcpp.h>
#include <unordered_map>
#include "filesystem.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using filesystem::FileSystem;
using filesystem::ReadFileRequest;
using filesystem::ReadFileResponse;
using filesystem::WriteFileRequest;
using filesystem::WriteFileResponse;
using filesystem::ListFilesRequest;
using filesystem::ListFilesResponse;

class FileSystemServiceImpl final : public FileSystem::Service {
public:
  Status ReadFile(ServerContext* context, const ReadFileRequest* request,
                  ReadFileResponse* response) override {
    int fd = open(request->filename().c_str(), O_RDONLY);
    if (fd == -1) {
      return Status(grpc::NOT_FOUND, "File not found");
    }

    {
      std::unique_lock<std::mutex> ul(mtx);
      if (cache.count(request->filename())) {
        response->set_content(cache[request->filename()]);
        return Status::OK;
      }
    }

    char buffer[1024];
    ssize_t bytesRead;
    std::string content;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        content.append(buffer, bytesRead);
    }
    close(fd);

    if (bytesRead == -1) {
        return Status::CANCELLED;
    }

    response->set_content(content);
    return Status::OK;
  }

  Status WriteFile(ServerContext* context, const WriteFileRequest* request,
                   WriteFileResponse* response) override {
    int fd = open(request->filename().c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
      return Status::CANCELLED;
    }

    if (close(fd) == -1) {
        perror("Failed to close file");
        return Status::CANCELLED;
    }

    {
      std::unique_lock<std::mutex> ul(mtx);
      cache[request->filename()] = request->content();
    }

    response->set_success(true);
    return Status::OK;
  }

  Status ListFiles(ServerContext* context, const ListFilesRequest* request,
                   ListFilesResponse* response) override {
    // For simplicity, let's list files in the current directory
    DIR* dir = opendir(request->directory().c_str());
    if (dir == nullptr) {
        perror("Failed to open directory");
        return Status::CANCELLED;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip the "." and ".." entries
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            response->add_filenames(std::string(entry->d_name));
        }
    }

    if (closedir(dir) == -1) {
        perror("Failed to close directory");
    }

    return Status::OK;
  }

private:
  std::mutex mtx;
  std::unordered_map<std::string, std::string> cache;

  Status writeFile(const char* filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
      return Status::CANCELLED;
    }

    auto& content = cache[std::string(filename)];

    ssize_t bytesWritten = write(fd, content.c_str(), content.size());
    if (bytesWritten == -1) {
        perror("Failed to write to file");
        close(fd);
        return Status::CANCELLED;
    }

    if (close(fd) == -1) {
        perror("Failed to close file");
        return Status::CANCELLED;
    }

    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  FileSystemServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();
  return 0;
}

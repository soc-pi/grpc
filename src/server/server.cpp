#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "common/generated/service.grpc.pb.h"

using example::Greeter;
using example::HelloReply;
using example::HelloRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    class GreeterServiceImpl final : public Greeter::Service {
        Status SayHello(ServerContext* context, const HelloRequest* request,
                        HelloReply* reply) override {
            std::string prefix("Hello ");
            reply->set_message(prefix + request->name());
            return Status::OK;
        }
    };

    GreeterServiceImpl service;
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

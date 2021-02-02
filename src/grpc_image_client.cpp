/*
 * Roboception GmbH
 * Munich, Germany
 * www.roboception.com
 *
 * Copyright (c) 2020 Roboception GmbH
 * All rights reserved
 *
 * Author: Felix Ruess
 */

#include "image_interface.grpc.pb.h"

#include <grpc++/grpc++.h>

#include <iomanip>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

class ImageInterfaceClient {
 public:
  ImageInterfaceClient(std::shared_ptr<Channel> channel)
      : stub_(ImageInterface::NewStub(channel)) {}

  // Assembles the client's payload, sends it and
  // prints ImageSet timestamp
  void StreamImageSets(bool left, bool right, bool disparity, bool confidence, bool disparity_error) {
    // Data we are sending to the server.
    ImageSetRequest request;
    request.set_left_enabled(left);
    request.set_right_enabled(right);
    request.set_disparity_enabled(disparity);
    request.set_confidence_enabled(confidence);
    request.set_disparity_error_enabled(disparity_error);

    std::cout << "Requesting images sets: [l: " << left << ", r: " << right << ", d: " << disparity
              << ", c: " << confidence << ", e: " << disparity_error << "]" << std::endl;

    // Container for the data we expect from the server.
    ImageSet image_set;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    std::unique_ptr<ClientReader<ImageSet>> reader(stub_->StreamImageSets(&context, request));
    while (reader->Read(&image_set))
    {
      std::cout << "ImageSet timestamp " << image_set.timestamp().sec() << "." << std::setfill('0') << std::setw(9) << image_set.timestamp().nsec()
                << " [l: " << image_set.has_left() << ", r: " << image_set.has_right() << ", d: " << image_set.has_disparity()
                << ", c: " << image_set.has_confidence() << ", e: " << image_set.has_disparity_error() << "]" << std::endl;
      /////////////////////////////////////////////
      // here you would actually process the images
      /////////////////////////////////////////////
    }
    Status status = reader->Finish();
    // Act upon its status.
    if (status.ok()) {
      std::cout << "StreamImageSets rpc succeded." << std::endl;
    } else {
      std::cerr << "StreamImageSets rpc failed with: code " << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }

 private:
  std::unique_ptr<ImageInterface::Stub> stub_;
};


void print_help(char** argv)
{
  std::cout << argv[0] << " -h | IP:port <options>" << std::endl;
  std::cout << std::endl;
  std::cout << "Stream ImageSets via gRPC." << std::endl;
  std::cout << std::endl;
  std::cout << "-h          Prints help information and exits" << std::endl;
  std::cout << "IP:port     Target IP:port to connect to (e.g: 172.17.0.1:50051)" << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "-l 0|1      Enable left image            (default: 1)" << std::endl;
  std::cout << "-r 0|1      Enable right image           (default: 0)" << std::endl;
  std::cout << "-d 0|1      Enable disparity image       (default: 1)" << std::endl;
  std::cout << "-c 0|1      Enable confidence image      (default: 0)" << std::endl;
  std::cout << "-e 0|1      Enable disparity error image (default: 0)" << std::endl;
}


int main(int argc, char** argv)
{
  std::string target_str = "172.17.0.1:50051";
  bool left = true;
  bool right = false;
  bool disparity = true;
  bool confidence = false;
  bool disparity_error = false;

  if (argc > 1 && std::string(argv[1]) != "-h" && argv[1][0] != '-')
  {
    int i=1;
    target_str = argv[i++];
    while (i < argc)
    {
      std::string p = argv[i++];
      if (p[0] == '-' && i >= argc)
      {
        print_help(argv);
        return 0;
      }
      if (p == "-l")
        left = std::string(argv[i++]) != "0";
      else if (p == "-r")
        right = std::string(argv[i++]) != "0";
      else if (p == "-d")
        disparity = std::string(argv[i++]) != "0";
      else if (p == "-c")
        confidence = std::string(argv[i++]) != "0";
      else if (p == "-e")
        disparity_error = std::string(argv[i++]) != "0";
    }
  }
  else
  {
    print_help(argv);
    return 0;
  }

  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint IP:port.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::cout << "Connecting to target " << target_str << std::endl;
  ImageInterfaceClient client(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  // call the StreamImageSets RPC with the desired images
  client.StreamImageSets(left, right, disparity, confidence, disparity_error);
  // streams forever until the client is stopped

  return 0;
}

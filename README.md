Example client for gRPC image server
====================================

The rc_cube provides synchronized image sets via [gRPC](https://grpc.io/) server side streams on port 5001.

This example client just prints the timestamps and type of received images.

The left, right, disparity, confidence and disparity_error images can be enabled separately.

Building
--------

This example uses the grpc++ library and grpc_cpp_plugin. pkg-config is used to find those.

On Debian/Ubuntuy systems this can be installed with

    sudo apt-get install libgrpc++-dev protobuf-compiler-grpc pgk-config

To build the example with cmake

    mkdir build && cd build
    cmake ..
    make

A debian package can be built with `make package`.

Running
-------

Required argument is the target (IP:port):

    grpc_image_client 172.17.0.1:50051

For all options see

    grcp_image_client -h

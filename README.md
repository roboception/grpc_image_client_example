Example client for gRPC image server
====================================

The rc_cube provides synchronized image sets via [gRPC](https://grpc.io/) server side streams on port 5001.

This example client prints the timestamps and type of received images and optionally saves them as png.

The left, right, disparity, confidence and disparity_error images can be enabled separately.

Building
--------

This example uses the grpc++ library and grpc_cpp_plugin. pkg-config is used to find those.

On Debian/Ubuntu systems this can be installed with

    sudo apt-get install libgrpc++-dev protobuf-compiler-grpc pgk-config

To build the example with cmake

    mkdir build && cd build
    cmake ..
    make

A debian package can be built with `make package`.

> **Note for Ubuntu 18.04 and lower**
>
> The gRPC packages in Ubuntu 18.04 and lower are not packaged correctly resulting in a segmentation fault.
> See https://bugs.launchpad.net/ubuntu/+source/grpc/+bug/1727528 and https://bugs.launchpad.net/ubuntu/+source/grpc/+bug/1797000
> Either build those from source, use a more recent Ubuntu version or the packages from Debian.

Running
-------

Required argument is the target (IP:port):

    grpc_image_client 172.17.0.1:50051

For all options see

    grcp_image_client -h

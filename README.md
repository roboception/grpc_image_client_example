Example client for gRPC image server
====================================

The rc_cube provides synchronized image sets via [gRPC](https://grpc.io/) server side streams on port 5001.

This example client just prints the timestamps and type of received images.

The left, right, disparity, confidence and disparity_error images can be enabled separately.

Running
-------

Required argument is the target (IP:port):

    grpc_image_client 172.17.0.1:50051

For all options see

    grcp_image_client -h

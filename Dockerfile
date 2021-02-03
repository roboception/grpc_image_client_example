FROM ubuntu:focal as builder

RUN DEBIAN_FRONTEND=noninteractive apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y cmake dpkg-dev file git g++ libgrpc++-dev lintian protobuf-compiler-grpc pkg-config

WORKDIR /workspace
COPY . .
RUN mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release .. \
    && make -j4 && make package


FROM ubuntu:focal

COPY --from=builder /workspace/build/*.deb /tmp
RUN DEBIAN_FRONTEND=noninteractive apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt install -y /tmp/*.deb \
    && rm -rf /var/lib/apt/lists/*

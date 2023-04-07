FROM ubuntu:bionic

RUN apt-get update && apt-get -y install \
    software-properties-common \
    make \
    perl \
    libssl-dev \
    liblua5.1-dev

# install GCC 11
RUN add-apt-repository ppa:ubuntu-toolchain-r/test \
    && apt-get update && apt-get -y install g++-11 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# install latest cmake (https://askubuntu.com/a/889453/434353, https://cmake.org/download/)
ADD https://github.com/Kitware/CMake/releases/download/v3.25.1/cmake-3.25.1-linux-x86_64.sh /cmake-linux-x86_64.sh
RUN mkdir /opt/cmake \
    && sh /cmake-linux-x86_64.sh --prefix=/opt/cmake --skip-license \
    && ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake

# install latest openssl (https://www.openssl.org/source/)
ADD https://www.openssl.org/source/openssl-3.0.7.tar.gz /openssl.tar.gz
RUN mkdir /opt/openssl \
    && tar xf /openssl.tar.gz -C /opt/openssl

WORKDIR /opt/openssl/openssl-3.0.7
RUN sh ./config --prefix=/opt/openssl --openssldir=/opt/openssl/ssl \
    && make \
    && make install
RUN echo '/opt/openssl/lib' > /etc/ld.so.conf.d/openssl.conf \
    && ldconfig
ENV CPATH=/opt/openssl/include

COPY . /app
WORKDIR /app

RUN export CC=/usr/bin/gcc-11; export CXX=/usr/bin/g++-11 \
    && cmake -DCMAKE_BUILD_TYPE=Release -S . -B build \
    && cmake --build build --target all
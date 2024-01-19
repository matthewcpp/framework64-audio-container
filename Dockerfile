# Build with: docker build -t matthewcpp/framework64-audio .
# On Apple silicon: docker build -t matthewcpp/framework64-audio --platform linux/amd64 .

from ubuntu:jammy as cpp_builder

ARG VCPKG_GITHUB_REPO="https://github.com/microsoft/vcpkg.git"
ARG VCPKG_DIR=/opt/vcpkg
ENV FW64_DIR="/opt/fw64_n64_libultra"

COPY . $FW64_DIR

RUN apt update && \
    DEBIAN_FRONTEND="noninteractive" apt install -y build-essential cmake git curl zip pkg-config && \

    cd /opt && git clone $VCPKG_GITHUB_REPO && \
    cd $VCPKG_DIR && ./bootstrap-vcpkg.sh && \

    mkdir $FW64_DIR/build && cd $FW64_DIR/build && \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake && \
    cmake --build . --target json2ins --config Release && \
    cmake --build . --target snd2aiff --config Release


from ubuntu:jammy

ARG NODE_MAJOR_VERSION=20
ARG CMAKE_VERSION=3.28.1
ARG NINJA_VERSION=v1.11.1

ENV FW64_DIR="/opt/fw64_n64_libultra"
ENV FW64_N64_LIBULTRA_CONTAINER=1

# Insall Wine
RUN dpkg --add-architecture i386 && apt update && \
    apt install -y wine wine32 wine64 libwine libwine:i386 fonts-wine nano wget p7zip && \
    wine --version && wineconsole --help

# Download and setup CMake
    cd /opt && \
    wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-linux-x86_64.tar.gz && \
    tar -xzvf cmake-$CMAKE_VERSION-linux-x86_64.tar.gz && \
    rm cmake-$CMAKE_VERSION-linux-x86_64.tar.gz && \
    ln -s /opt/cmake-$CMAKE_VERSION-linux-x86_64/bin/cmake /usr/bin/cmake && \

# Download and setup Ninja
    wget https://github.com/ninja-build/ninja/releases/download/$NINJA_VERSION/ninja-linux.zip && \
    unzip ninja-linux.zip -d ninja-$NINJA_VERSION && \
    rm ninja-linux.zip && \
    ln -s /opt/ninja-$NINJA_VERSION/ninja /usr/bin/ninja && \

# Insall Node
    apt install -y ca-certificates curl gnupg && \
    mkdir -p /etc/apt/keyrings && \
    curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg && \
    echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_${NODE_MAJOR_VERSION}.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list && \
    apt update && apt install -y nodejs && \

    mkdir /src /dest /job /fw64 && \
    rm -rf /var/lib/apt/lists/*


COPY --from=cpp_builder $FW64_DIR/build/json2ins/json2ins $FW64_DIR/build/snd2aiff/snd2aiff /fw64
COPY . $FW64_DIR

ENTRYPOINT ["/fw64/entrypoint.sh"]


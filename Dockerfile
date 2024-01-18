# Build with: docker build -t matthewcpp/framework64-audio .
# On Apple silicon: docker build -t matthewcpp/framework64-audio --platform linux/amd64 .

from ubuntu:jammy as cpp_builder

ENV VCPKG_GITHUB_REPO="https://github.com/microsoft/vcpkg.git"
ENV VCPKG_DIR=/opt/vcpkg
ENV SRC_DIR="/opt/fw64_n64_libultra"

COPY . $SRC_DIR

RUN apt update && \
    DEBIAN_FRONTEND="noninteractive" apt install -y build-essential cmake git curl zip pkg-config && \

    cd /opt && git clone $VCPKG_GITHUB_REPO && \
    cd $VCPKG_DIR && ./bootstrap-vcpkg.sh && \

    mkdir $SRC_DIR/build && cd $SRC_DIR/build && \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake && \
    cmake --build . --target json2ins --config Release && \
    cmake --build . --target snd2aiff --config Release


from ubuntu:jammy

ARG NODE_MAJOR_VERSION=20
ARG FW64_N64_LIBULTRA_CONTAINER=1

RUN dpkg --add-architecture i386 && apt update && \
    apt install -y wine wine32 wine64 libwine libwine:i386 fonts-wine nano p7zip && \
    wine --version && wineconsole --help && \
    mkdir /src /dest /job /fw64 && \

RUN apt install -y ca-certificates curl gnupg && \
    mkdir -p /etc/apt/keyrings && \
    curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg && \
    echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_${NODE_MAJOR_VERSION}.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list && \
    apt update && apt install -y nodejs && \
    
    rm -rf /var/lib/apt/lists/*


COPY --from=cpp_builder /src/build/json2ins/json2ins /src/build/snd2aiff/snd2aiff /fw64
COPY . /

ENTRYPOINT ["/fw64/entrypoint.sh"]


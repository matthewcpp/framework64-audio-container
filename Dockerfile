# Build with: docker build -t matthewcpp/framework64-audio .
# On Apple silicon: docker build -t matthewcpp/framework64-audio --platform linux/amd64 .

from ubuntu:jammy as cpp_builder

ENV VCPKG_GITHUB_REPO="https://github.com/microsoft/vcpkg.git"
ENV VCPKG_DIR=/lib/vcpkg

COPY src /src

RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y build-essential cmake git libsndfile-dev curl zip && \

    mkdir /lib && cd /lib && git clone $VCPKG_GITHUB_REPO && \
    cd $VCPKG_DIR && ./bootstrap

    cd /tools && mkdir build && cd build && ./bootstrap-vcpkg.sh \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake&& \
    cmake --build . --target json2ins --config Release && \
    cmake --build . --target snd2aiff --config Release


from ubuntu:jammy

ARG NODE_MAJOR_VERSION=20

RUN apt-get update && \
    dpkg --add-architecture i386 && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y wget gnupg software-properties-common unzip p7zip cabextract curl nano wine && \
    apt-get install -y wine32 && \
    wineconsole --help && \
    mkdir /src /dest /job /fw64 && \

    apt-get install -y ca-certificates curl gnupg && \
    mkdir -p /etc/apt/keyrings && \
    curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg && \
    echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_${NODE_MAJOR_VERSION}.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list && \
    apt-get update && apt-get install -y nodejs && \
    
    rm -rf /var/lib/apt/lists/*


COPY --from=cpp_builder /src/build/json2ins/json2ins /src/build/snd2aiff/snd2aiff /fw64/
COPY . /

ENTRYPOINT ["/fw64/entrypoint.sh"]


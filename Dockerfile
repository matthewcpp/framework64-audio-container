from ubuntu:focal as cpp_builder

COPY json2ins /json2ins

RUN apt-get update && apt-get upgrade -y && \
	DEBIAN_FRONTEND="noninteractive" apt-get install -y build-essential cmake git && \
	cd /json2ins && mkdir build && cd build && \
	cmake .. && cmake --build . --target json2ins --config Release


from ubuntu:focal

RUN apt-get update && apt-get upgrade -y && \
	DEBIAN_FRONTEND="noninteractive" apt-get install -y  --no-install-recommends wget gnupg software-properties-common unzip p7zip cabextract curl nano && \
	dpkg --add-architecture i386 && \
	wget -nc https://dl.winehq.org/wine-builds/winehq.key && \ 
	apt-key add winehq.key && \
	add-apt-repository 'deb https://dl.winehq.org/wine-builds/ubuntu/ focal main' && \
	apt update && apt install --install-recommends -y winehq-stable && \
	curl -fsSL https://deb.nodesource.com/setup_15.x | bash - && \
	apt-get install -y nodejs && \
	mkdir /src /dest /job /fw64

COPY --from=cpp_builder /json2ins/build/json2ins /fw64
COPY fw64 /fw64


FROM gcc:15

ENV DEBIAN_FRONTEND=noninteractive

# 1) Install CA certs so HTTPS works, ignore initial update failures
RUN apt-get update || true && \
    apt-get install -y --no-install-recommends ca-certificates && \
    rm -rf /var/lib/apt/lists/*

# 2) Replace APT sources with explicit HTTPS entries for Debian trixie
RUN printf 'deb https://deb.debian.org/debian trixie main\n\
deb https://deb.debian.org/debian trixie-updates main\n\
deb https://deb.debian.org/debian-security trixie-security main\n' \
    > /etc/apt/sources.list

# 3) Now update from HTTPS and install build deps
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        pkg-config \
        libcurl4-openssl-dev \
        nlohmann-json3-dev \
        git && \
    rm -rf /var/lib/apt/lists/*

# 4) Install cppcodec as header-only (no Debian package needed)
RUN git clone https://github.com/tplgy/cppcodec.git /tmp/cppcodec && \
    mkdir -p /usr/local/include && \
    cp -r /tmp/cppcodec/cppcodec /usr/local/include && \
    rm -rf /tmp/cppcodec

# Initialize the variable (it will be overwritten by the runtime value)
ENV OPENAI_API_KEY=""

WORKDIR /usr/src/

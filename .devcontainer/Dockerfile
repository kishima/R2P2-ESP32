ARG IDF_VER
FROM espressif/idf:$IDF_VER

RUN apt-get update && apt-get install -y \
    build-essential \
    ruby \
    ruby-dev \
    rake \
    git \
    curl \
    wget \
    bison \
    autoconf \
    libtool \
    pkg-config \
    flex \
    ca-certificates \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /project
CMD ["/bin/bash"]

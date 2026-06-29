# run Ubuntu 16.4 32bit version !!!
FROM i386/ubuntu:16.04

# add all from local directory
ADD . /assembly

RUN dpkg --add-architecture i386 && \
    apt-get update && apt-get install -y \
    build-essential &&\
    gcc &&\
    gdb &&\
    make &&\
    git &&\
    vim &&\
    python &&\
    curl &&\
    wget &&

WORKDIR /assembly


CMD ["/bin/bash"]


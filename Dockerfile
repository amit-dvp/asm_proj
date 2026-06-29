# run Ubuntu 16.4 32bit version !!!
FROM i386/ubuntu:16.04

# add all from local directory
ADD . /assembly

RUN apt-get update
RUN apt-get install -y lsb-release
RUN apt-get install build-essential -y
#RUN apt-get install gcc
#RUN apt-get install gdb
#RUN apt-get install make

WORKDIR /assembly


CMD ["/bin/bash"]


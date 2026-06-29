# run Ubuntu 16.4 32bit version !!!
FROM i386/ubuntu:16.04

# add all from local directory
ADD . /assembly

RUN apt-get update
RUN apt-get install -y lsb-release
RUN apt-get install build-essential -y
RUN apt-get install gcc -y
RUN apt-get install gdb -y
RUN apt-get install make -y

# Valgrind is a runtime analysis framework 
# that runs your program in a special instrumented 
# environment. It watches every memory access, 
# allocation, and deallocation.

RUN apt-get install -y valgrind -y


WORKDIR /assembly


CMD ["/bin/bash"]

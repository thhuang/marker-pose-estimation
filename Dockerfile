FROM ubuntu:16.04
LABEL maintainer="thhuang@caealumni.ce.ntu.edu.tw"

WORKDIR /tmp

RUN apt-get update
RUN apt-get install -y wget git build-essential 
RUN apt-get install -y libgtk2.0-dev libvtk5-dev libjpeg-dev
RUN apt-get install -y libtiff5-dev libjasper-dev libopenexr-dev 
RUN apt-get install -y libtbb-dev python-dev libeigen3-dev

# CMake 3.13.4
RUN wget https://github.com/Kitware/CMake/releases/download/v3.13.4/cmake-3.13.4-Linux-x86_64.tar.gz
RUN tar -xvf cmake-3.13.4-Linux-x86_64.tar.gz
RUN echo '#!/bin/bash\n/tmp/cmake-3.13.4-Linux-x86_64/bin/cmake "$@"' > /usr/bin/cmake && \
    chmod +x /usr/bin/cmake

# OpenCV 3.4.7
RUN git clone https://github.com/opencv/opencv.git && \
    cd opencv && \
    git checkout "3.4.7"
RUN git clone https://github.com/opencv/opencv_contrib.git && \
    cd opencv_contrib && \
    git checkout "3.4.7"
RUN mkdir opencv/build && cd opencv/build && \
    cmake \
    -D CMAKE_BUILD_TYPE=RELEASE \
    -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
    .. && \
    make -j2 && \
    make install
RUN rm -rf /var/lib/apt/lists/* 

WORKDIR /app

RUN echo "#!/bin/bash\nmkdir -p build && cd build && cmake .. && make -j" > /usr/bin/compile && \
    chmod +x /usr/bin/compile
RUN echo '#!/bin/bash\n/app/bin/"$@"' > /usr/bin/execute && \
    chmod +x /usr/bin/execute

CMD ["echo", "Please execute the program with \"docker-compose run [options] SERVICE [COMMAND] [ARGS...]\"\nFor more information: https://docs.docker.com/compose/reference/run"]
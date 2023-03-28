FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive
RUN sed -Ei 's/^# deb-src /deb-src /' /etc/apt/sources.list

COPY apt-requirements /
COPY python-requirements /

RUN apt-get update && cat apt-requirements | xargs apt-get install -y
RUN cat python-requirements | xargs pip3 install

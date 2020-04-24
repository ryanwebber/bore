FROM alpine

RUN apk add --no-cache --upgrade bash
RUN apk add readline-dev
RUN apk add build-base

RUN mkdir -p /tmp/lua
ADD http://www.lua.org/ftp/lua-5.3.5.tar.gz /tmp/lua

RUN cd /tmp/lua && \
    tar zxf lua-5.3.5.tar.gz && \
    cd lua-5.3.5 && \
    make linux test && \
    make install

COPY . /bore

RUN make -C /bore test


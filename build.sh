#!/bin/bash

docker build -t cpp-playground -f Dockerfile .
docker rm -f cpp-playground-1
docker create -i -t --name cpp-playground-1 cpp-playground
docker cp --quiet cpp-playground-1:/app/build/led/led_proxy_next .
docker rm -f cpp-playground-1
#!/bin/bash

ln -sf docker/Dockerfile.release Dockerfile
pushd ./web
ln -sf Dockerfile.release Dockerfile
popd 
ln -sf docker/stack.yml stack.yml

ls -l Dockerfile web/Dockerfile stack.yml

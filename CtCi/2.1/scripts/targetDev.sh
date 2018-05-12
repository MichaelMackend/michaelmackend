#!/bin/bash

if [ -e RELEASE ] ; then rm RELEASE; fi
touch DEVELOPMENT

ln -fs docker/Dockerfile.dev Dockerfile
pushd ./web
ln -fs Dockerfile.dev Dockerfile
popd
ln -fs docker/stack.dev.yml stack.yml

ls -l Dockerfile web/Dockerfile stack.yml

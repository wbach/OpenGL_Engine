#!/bin/bash

cd /home/baszek/Projects/prebuild/OpenGL_Engine/
echo "Used threads : "`nproc`
revision=`cat ../toCommit.revision`
git stash
git clean -fd
git pull
git reset --hard $revision
git rev-parse HEAD
git apply ../toCommit.diff

mkdir build
cd build/
cmake ../Solutions/CMake/ -DBUILD_ALL=ON -DCMAKE_BUILD_TYPE=Release
time make -j`nproc`
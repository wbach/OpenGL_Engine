#!/bin/bash
cd /home/baszek/Projects/prebuild/OpenGL_Engine/
revision=`cat ../toCommit.revision`
git stash
git clean -fd
git pull
git reset --hard $revision
git rev-parse HEAD
git apply ../toCommit.diff

cd /home/baszek/Projects/prebuild/OpenGL_Engine/build/
cmake ../Solutions/CMake/ -DBUILD_ALL=ON -DCMAKE_BUILD_TYPE=Release
make -j4
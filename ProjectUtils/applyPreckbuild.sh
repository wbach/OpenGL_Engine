#!/bin/bash

location="/srv/dev-disk-by-uuid-523ce0f3-9a7d-48d5-81ce-f8860be8f8f8/fpwork/prebuild"
cd $location'/OpenGL_Engine'

echo "Used threads : "`nproc`
revision=`cat ../toCommit.revision`
branch=`cat ../toCommit.branch`
git stash
git clean -fd
git pull
git checkout $branch
git reset --hard $revision
git rev-parse HEAD
git apply ../toCommit.diff

cd $location'/OpenGL_Engine/ProjectUtils'
echo `pwd`
getAllSourcesAndIncludes.sh
cd ..

mkdir build
cd build/
cmake ../Solutions/CMake/ -DBUILD_ALL=ON -DCMAKE_BUILD_TYPE=Release
time make -j`nproc`
#!/bin/bash

sshTarget=$1'@'$2

revision=`git rev-parse HEAD`

remoteApplyScript='/home/'$1'/Projects/prebuild/OpenGL_Engine/ProjectUtils/applyPreckbuild.sh'

mkdir ../tmp
echo $revision > ../tmp/toCommit.revision

git add .
git diff master > ../tmp/toCommit.diff
scp ../tmp/toCommit.diff $sshTarget:/home/$1/Projects/prebuild/
scp ../tmp/toCommit.revision $sshTarget:/home/$1/Projects/prebuild/
ssh -t $sshTarget $remoteApplyScript
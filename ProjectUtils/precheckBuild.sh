#!/bin/bash

sshTarget='baszek@'$1

revision=`git rev-parse HEAD`

remoteApplyScript='/home/baszek/Projects/prebuild/OpenGL_Engine/ProjectUtils/applyPreckbuild.sh'

mkdir ../tmp
echo $revision > ../tmp/toCommit.revision

git add .
git diff master > ../tmp/toCommit.diff
scp ../tmp/toCommit.diff $sshTarget:/home/baszek/Projects/prebuild/
scp ../tmp/toCommit.revision $sshTarget:/home/baszek/Projects/prebuild/
ssh -t $sshTarget $remoteApplyScript
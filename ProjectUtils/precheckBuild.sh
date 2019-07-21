#!/bin/bash

sshTarget='baszek@192.168.1.105'

revision=`git rev-parse HEAD`

remoteApplyScript='/home/baszek/Projects/prebuild/OpenGL_Engine/ProjectUtils/applyPreckbuild.sh'
echo $revision > ../toCommit.revision

git add .
git diff master > ../toCommit.diff
scp ../toCommit.diff $sshTarget:/home/baszek/Projects/prebuild/
scp ../toCommit.revision $sshTarget:/home/baszek/Projects/prebuild/
ssh $sshTarget $remoteApplyScript
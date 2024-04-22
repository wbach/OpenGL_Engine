#!/bin/bash

sshTarget=$1'@'$2

branch=`git rev-parse --abbrev-ref HEAD`
revision=`git rev-parse HEAD`

location="/srv/dev-disk-by-uuid-523ce0f3-9a7d-48d5-81ce-f8860be8f8f8/fpwork/prebuild/"
remoteApplyScript=$location'OpenGL_Engine/ProjectUtils/applyPreckbuild.sh'

mkdir ../tmp
echo $revision > ../tmp/toCommit.revision
echo $branch > ../tmp/toCommit.branch

git add .
git diff $branch > ../tmp/toCommit.diff
scp ../tmp/toCommit.diff $sshTarget:$location
scp ../tmp/toCommit.branch $sshTarget:$location
scp ../tmp/toCommit.revision $sshTarget:$location
ssh -t $sshTarget $remoteApplyScript
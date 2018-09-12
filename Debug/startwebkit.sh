#!/bin/bash
TMPDIR="/tmp/qtembedded-$USER"
if [ ! -d $TMPDIR ]
then
    mkdir -p $TMPDIR
fi

chmod 700 $TMPDIR
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/home/$USER/Webkit/WebKitBuild/Debug/lib

#to avoid "cannot connect to X Server" problem, you should start vncserver first
export DISPLAY=localhost:1 
xhost +
./webkit $1 $2 &

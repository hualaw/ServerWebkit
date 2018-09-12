#!/bin/bash
if [ ! -d /tmp/qtembedded-0 ]
then
    mkdir -p /tmp/qtembedded-0
fi

chmod 700 /tmp/qtembedded-0
./webkit $1 $2 &

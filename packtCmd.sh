#!/bin/sh

cd /browser/deployment/

now=`date "+%Y%m%d%H%M"`

tar jcvfh webkit_2.0_${now}.tar.gz  \
--exclude "*.log*" \
ServerWebkit/bin/webkit \
ServerWebkit/bin/libQtWebKit.so.4 \
ServerWebkit/bin/startwebkit.sh \
ServerWebkit/logs/ \
ServerWebkit/config/webkit.ini

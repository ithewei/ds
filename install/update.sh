#!/bin/sh
srcpath=`dirname $0`
srcpath=`(cd "$srcpath"; pwd)`
dstpath=/opt/anystreaming/transcoder/

cp -rfu $srcpath/expre/* /var/www/transcoder/Upload/
cp -rfu $srcpath/img/* $dstpath/img/
cp -rfu $srcpath/lib/* $dstpath/plugin/

echo "Install success!"

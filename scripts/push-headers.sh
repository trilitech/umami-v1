#!/bin/bash

SRCFILES=$(find ./src -type f -name "*.re")
HEADERFILE="./docs/standards/code-header.txt"
MARKER=$(head -1 $HEADERFILE)

echo $MARKER

for f in $SRCFILES
do
  if [ "$(head -1 $f)" == "$MARKER" ];
  then
    echo "$f: matching Header--NOOP"
  else
    echo "$f: missing header--applying"
    mv $f $f.tmp; cat $HEADERFILE $f.tmp > $f; rm $f.tmp
  fi
done

#!/bin/sh

#export PATH=$PATH:../server/teamtalk/protobuf/bin


SRC_DIR=./
DST_DIR=./gen

#C++
mkdir -p $DST_DIR/cpp
/usr/local/bin/protoc -I=$SRC_DIR --cpp_out=$DST_DIR/cpp/ $SRC_DIR/*.proto

#JAVA
mkdir -p $DST_DIR/java
/usr/local/bin/protoc -I=$SRC_DIR --java_out=$DST_DIR/java/ $SRC_DIR/*.proto

#OBJC
mkdir -p $DST_DIR/objc
/usr/local/bin/protoc -I=$SRC_DIR --objc_out=$DST_DIR/objc/ $SRC_DIR/*.proto

#PYTHON
mkdir -p $DST_DIR/python
/usr/local/bin/protoc -I=$SRC_DIR --python_out=$DST_DIR/python/ $SRC_DIR/*.proto

mkdir -p  $DST_DIR/go
/usr/local/bin/protoc -I=$SRC_DIR --go_out=:$DST_DIR/go/ $SRC_DIR/*.proto


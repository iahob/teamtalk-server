#!/bin/sh
CPP_DIR=../server/src/base/pb/protocol
DST_DIR=./gen

#C++
cp $DST_DIR/cpp/* $CPP_DIR/


#android
ANDROID_DIR=../android/app/src/main/java/com/mogujie/tt/protobuf
cp -f  $DST_DIR/java/com/mogujie/tt/protobuf/* $ANDROID_DIR/

ios
IOS_DIR=../ios/TeamTalk/Exist/GPB/objcpb
cp -f $DST_DIR/objc/* $IOS_DIR/



#rm -rf ./gen

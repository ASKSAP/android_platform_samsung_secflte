#!/bin/bash

################################################################################
#
# Build Script
#
# Copyright Samsung Electronics(C), 2010
#
################################################################################


################################################################################
# Useage
#   : ./build.sh [mod]
#
# Example
#   : ./build.sh user
################################################################################

VARIANT=$1
export CPU_NUMBER=16

if	[ "" = "$1" ]
then
	echo --------------------------------------------------------------------------------
	echo - Useage
	echo -   : ./build.sh [mod]
	echo - Set "user" [default]
	echo - 
	echo --------------------------------------------------------------------------------
	VARIANT=user
fi

export TARGET_BUILD_VARIANT=${VARIANT}

################################################################################
# Common Path
################################################################################
TOP_DIR=$PWD

# set root path
export PLATFORMPATH="$TOP_DIR"

cd $PLATFORMPATH 
make update-api
make -j$CPU_NUMBER PRODUCT-generic-$TARGET_BUILD_VARIANT

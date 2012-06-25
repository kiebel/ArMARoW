ARMAROW_DIR=../../

CXXFLAGS+=-Wall -Os -g -DCONTENT=\"ping\" -DSEND_PERIOD=100 #-DLOGGING_DISABLE
PLATFORM=${ARMAROW_DIR}/platform/deRCB128RFA1

BIN=./bin
SRC=./src
BUILD=./build
INC=./include

.PHONY: default

default: help

include ${ARMAROW_DIR}/make/example.mk

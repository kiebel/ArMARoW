.PHONY: debug

PLATFORM_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))
INCLUDES     += ${PLATFORM_DIR}/include

include ${ARMAROW_DIR}/make/avr.mk

PLATFORM    := ${HALIB_DIR}/platform/deRCB128RFA1
PM_SRC_DIRS +=${PLATFORM_DIR}/portmap

.PHONY: debug

PLATFORM_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

debug:
	@echo "This makefile is ${PLATFORM_DIR}/config.mk"

INCLUDES += ${PLATFORM_DIR}/include

include ${ARMAROW_DIR}/make/avr.mk

PLATFORM := ${HALIB_DIR}/platform/deRCB128RFA1

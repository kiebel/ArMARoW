include ../../config.mk
include ${AVRHALIB_DIR}/platforms/icstick/config.mk

INCLUDES+= $(dir $(lastword( ${MAKEFILE_LIST}))/include

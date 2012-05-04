include $(dir $(abspath $(lastword ${MAKEFILE_LIST})))/../config.mk
include ${PLATFORM}/config.mk

include ${HALIB_DIR}/make/singleObjectApp.mk
include ${HALIB_DIR}/make/clean.mk

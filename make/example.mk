include ${ARMAROWDIR}/make/general.mk
include ${ARMAROWDIR}/make/rules.mk

SOURCES=$(wildcard ${SRC}/*.cpp ${SRC}/*.cc ${SRC}/*.c ${SRC}/*.S)

OBJECTS=$(foreach OBJ, ${SOURCES}, ${BUILD}/$(notdir $(basename ${OBJ})).o)

EXAMPLES=$(notdir $(basename ${OBJECTS}))

.PHONY: help all  %.program ${EXAMPLES}

help:
	@echo "current examples: "
	@echo all ${EXAMPLES}

all: $(addprefix ${BIN}/, $(addsuffix .elf, ${EXAMPLES}))

${BIN}/%.elf: ${BUILD}/%.o | ${BIN}
	@echo "(LD    ) $(notdir $<) -> $(notdir $@)"
	@${CXX} $< -o $@ ${LDFLAGS}

${EXAMPLES}: %: ${BIN}/%.elf

clean:
	@echo "(CLEAN)"
	@rm -rf ${GARBAGE}

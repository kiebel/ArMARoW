PMGENBIN:=${HALIB}/tools/portmapgen/avr-halib-pmg
PMGEN=${PMGENBIN}

.PHONY:  %.program
.PRECIOUS: %.elf %_portmap.h


${BIN}/%.hex: ${BIN}/%.elf |${BIN}
	@echo "(OBJCP ) $(notdir $<) -> $(notdir $@)"
	${OBJCP} ${OBJCPFLAGS} -O ihex $< $@

${INC}/%_portmap.h: ${INC}/%.portmap | ${PMGENBIN}
	@echo "(PMGEN ) $(notdir $<) -> $(notdir $@)"
	${PMGEN} $< > $@

%.program: ${BIN}/%.hex
	@echo "(FLASH)  $(notdir $<) -> ${PORT} -> ${CHIP}"
	${FLASHER} ${FLASHOPTS} -P ${PORT} -p ${CHIP} -c ${PROGRAMMER} -U f:w:$<:i

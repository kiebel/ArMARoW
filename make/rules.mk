GENDIRS=${BIN} ${LIB} ${BUILD}

DEPS=$(wildcard ${BUILD}/*.d)

GARBAGE+=${GENDIRS}

.PHONY: %.size
.PRECIOUS: ${BUILD}/%.d

${GENDIRS}: %:
	@mkdir -p $@

${BUILD}/%.d: ${SRC}/%.cpp |${EXTERNAL_TARGETS} ${BUILD}
	@${CXX} -MT $@ -MG -MM ${CXXFLAGS} $< -MF $@ 

${BUILD}/%.d: ${SRC}/%.cc |${EXTERNAL_TARGETS} ${BUILD}
	@${CXX} -MT $@ -MG -MM ${CXXFLAGS} $< -MF $@ 

${BUILD}/%.d: ${SRC}/%.c |${EXTERNAL_TARGETS} ${BUILD}
	@${CC} -MT $@ -MG -MM ${CFLAGS} $< -MF $@ 

${BUILD}/%.d: ${SRC}/%.S |${EXTERNAL_TARGETS} ${BUILD}
	@${AS} -MT $@ -MG -MM ${ASMFLAGS} $< -MF $@

${BUILD}/%.o: ${SRC}/%.cpp ${BUILD}/%.d | ${EXTERNAL_DEPS} ${EXTERNAL_TARGETS} ${BUILD}
	@echo "(CXX   ) $(notdir $<) -> $(notdir $@)"
	@${CXX} -c ${CXXFLAGS} $< -o $@

${BUILD}/%.o: ${SRC}/%.cc ${BUILD}/%.d | ${EXTERNAL_DEPS} ${EXTERNAL_TARGETS} ${BUILD}
	@echo "(CXX   ) $(notdir $<) -> $(notdir $@)"
	@${CXX} -c ${CXXFLAGS} $< -o $@

${BUILD}/%.o: ${SRC}/%.c ${BUILD}/%.d | ${EXTERNAL_DEPS} ${EXTERNAL_TARGETS} ${BUILD}
	@echo "(CC    ) $(notdir $<) -> $(notdir $@)"
	@${CC} -c ${CFLAGS} $< -o $@

${BUILD}/%.o: ${SRC}/%.S ${BUILD}/%.d | ${EXTERNAL_DEPS} ${EXTERNAL_TARGETS} ${BUILD}
	@echo "(AS    ) $(notdir $<) -> $(notdir $@)"
	@${AS} -c ${ASMFLAGS} $< -o $@

%.size:	${BIN}/%.elf
	@${SIZE} $<

%.dump: ${BIN}/%.elf
	@echo "(OBJDMP) $(notdir $<) -> $@"
	@${OBJDUMP} -Cxd $< > $@

-include ${DEPS}
-include ${ARMAROWDIR}/make/${ARCH}/rules.mk

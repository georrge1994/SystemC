#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ALU.o \
	${OBJECTDIR}/Cache.o \
	${OBJECTDIR}/Command_decoder.o \
	${OBJECTDIR}/Memory_controller.o \
	${OBJECTDIR}/Top.o \
	${OBJECTDIR}/Virtual_memory.o \
	${OBJECTDIR}/io_module.o \
	${OBJECTDIR}/testbench.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/home/oem/Programs/lib -Wl,-rpath,'/home/oem/Programs/lib' -lsystemc

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mcp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mcp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mcp ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/ALU.o: ALU.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/home/oem/Programs/include -I/home/oem/Документы/netbeans_projects/MCP/MCP -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ALU.o ALU.cpp

${OBJECTDIR}/Cache.o: Cache.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/home/oem/Programs/include -I/home/oem/Документы/netbeans_projects/MCP/MCP -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Cache.o Cache.cpp

${OBJECTDIR}/Command_decoder.o: Command_decoder.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/home/oem/Programs/include -I/home/oem/Документы/netbeans_projects/MCP/MCP -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Command_decoder.o Command_decoder.cpp

${OBJECTDIR}/Memory_controller.o: Memory_controller.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/home/oem/Programs/include -I/home/oem/Документы/netbeans_projects/MCP/MCP -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Memory_controller.o Memory_controller.cpp

${OBJECTDIR}/Top.o: Top.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/home/oem/Programs/include -I/home/oem/Документы/netbeans_projects/MCP/MCP -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Top.o Top.cpp

${OBJECTDIR}/Virtual_memory.o: Virtual_memory.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/home/oem/Programs/include -I/home/oem/Документы/netbeans_projects/MCP/MCP -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Virtual_memory.o Virtual_memory.cpp

${OBJECTDIR}/io_module.o: io_module.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/home/oem/Programs/include -I/home/oem/Документы/netbeans_projects/MCP/MCP -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/io_module.o io_module.cpp

${OBJECTDIR}/testbench.o: testbench.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/home/oem/Programs/include -I/home/oem/Документы/netbeans_projects/MCP/MCP -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/testbench.o testbench.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

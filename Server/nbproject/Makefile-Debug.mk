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
	${OBJECTDIR}/_ext/96f4e057/NewsFeedService.o \
	${OBJECTDIR}/_ext/96f4e057/newsfeed_constants.o \
	${OBJECTDIR}/_ext/96f4e057/newsfeed_types.o \
	${OBJECTDIR}/server.o


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
LDLIBSOPTIONS=../../Poco/lib/libpoco.a `pkg-config --libs thrift` ../../KC/dist/Debug/GNU-Linux/libkc.a  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server: ../../Poco/lib/libpoco.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server: ../../KC/dist/Debug/GNU-Linux/libkc.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server ${OBJECTFILES} ${LDLIBSOPTIONS} -lpthread

${OBJECTDIR}/_ext/96f4e057/NewsFeedService.o: ../gen-cpp/NewsFeedService.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/96f4e057
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../Poco/Util/include -I../../Poco/Foundation/include `pkg-config --cflags thrift` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/96f4e057/NewsFeedService.o ../gen-cpp/NewsFeedService.cpp

${OBJECTDIR}/_ext/96f4e057/newsfeed_constants.o: ../gen-cpp/newsfeed_constants.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/96f4e057
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../Poco/Util/include -I../../Poco/Foundation/include `pkg-config --cflags thrift` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/96f4e057/newsfeed_constants.o ../gen-cpp/newsfeed_constants.cpp

${OBJECTDIR}/_ext/96f4e057/newsfeed_types.o: ../gen-cpp/newsfeed_types.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/96f4e057
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../Poco/Util/include -I../../Poco/Foundation/include `pkg-config --cflags thrift` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/96f4e057/newsfeed_types.o ../gen-cpp/newsfeed_types.cpp

${OBJECTDIR}/server.o: server.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../Poco/Util/include -I../../Poco/Foundation/include `pkg-config --cflags thrift` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/server.o server.cpp

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

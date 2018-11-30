#makefile taken from class github 

OSPL_LIBS = -lpthread -lddskernel -ldcpssacpp
LIBS=-L${OSPL_HOME}/lib ${OSPL_LIBS} -lboost_system -lboost_thread

CFLAGS = -Wall -O0 -g -I. \
         -I./include -I${OSPL_HOME}/include/dcps/C++/SACPP \
         -I${OSPL_HOME}/include/sys \
         -I${OSPL_HOME}/include

CXXFLAGS = -std=c++11

all: tsn


IDL_GENERATED_H= \
                 ccpp_tsn.h \
                 tsnDcps.h \
                 tsnDcps_impl.h \
                 tsn.h \
                 tsnSplDcps.h 

IDL_GENERATED_CPP=\
                 tsn.cpp \
                 tsnDcps.cpp \
                 tsnDcps_impl.cpp \
                 tsnSplDcps.cpp

IDL_GENERATED=${IDL_GENERATED_H} ${IDL_GENERATED_CPP}

${IDL_GENERATED}: idl/tsn.idl
	${OSPL_HOME}/bin/idlpp -l cpp idl/tsn.idl

COMMON_CPP= src/CheckStatus.cpp src/DDSEntityManager.cpp src/user.cpp src/post.cpp src/system.cpp src/view.cpp src/controller.cpp src/message.cpp

COMMON_CPP1= src/CheckStatus.cpp src/DDSEntityManager.cpp src/user.cpp src/post.cpp src/system1.cpp src/view.cpp src/controller.cpp src/message.cpp

COMMON_H= src/CheckStatus.h src/DDSEntityManager.h src/user.h src/post.h src/system.h src/view.h src/controller.h src/message.h


tsn: ${IDL_GENERATED_H} ${IDL_GENERATED_CPP} ${COMMON_H} ${COMMON_CPP} src/main.cpp
	g++ -o $@ ${CFLAGS} ${CXXFLAGS} $^ ${LIBS}

tsn1: ${IDL_GENERATED_H} ${IDL_GENERATED_CPP} ${COMMON_H} ${COMMON_CPP1} src/main.cpp
	g++ -o $@ ${CFLAGS} ${CXXFLAGS} $^ ${LIBS}

clean:
	-rm -f exe
	-rm -f ${IDL_GENERATED_H} ${IDL_GENERATED_CPP}
	-rm -f ospl-error.log ospl-info.log

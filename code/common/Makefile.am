noinst_LIBRARIES=libcommon.a

libcommon_a_SOURCES = Common/common.cpp
#libcommon_a_SOURCES += CenterServer/CenterServer.cpp

AUTOMAKE_OPTIONS=foreign

SUBDIRS = Common Daemon DataBase

libcommon_a_LDFLAGS = -static -std=c++0x
libcommon_a_LDADD = -lpthread
#centerserver_LDADD+= ../Share/llib/libdaemon.a

INCLUDES = -I@top_srcdir@/Share/Header/
INCLUDES += -I../Share/Header/
INCLUDES += -I../Share/Header/Base/

bin_PROGRAMS = centerserver

centerserver_SOURCES = CenterServer/Main.cpp
centerserver_SOURCES += CenterServer/CenterServer.cpp
centerserver_SOURCES += CenterServer/Config/CenterServerConfig.cpp
centerserver_SOURCES += CenterServer/Connection/AppConnection.cpp
centerserver_SOURCES += CenterServer/Connection/ClientConnection.cpp
centerserver_SOURCES += CenterServer/Connection/DataConnection.cpp
centerserver_SOURCES += CenterServer/Connection/WebConnection.cpp

SUBDIRS = CenterServer CenterServerLogic
centerserver_LDFLAGS = -static -std=c++0x
centerserver_LDADD = -lpthread
centerserver_LDADD+= ../Share/llib/libdaemon.a
centerserver_LDADD+= ../Share/llib/libfilelog.a
centerserver_LDADD+= ../Share/llib/libfilereader.a
centerserver_LDADD+= ../Share/llib/libnetwork.a
centerserver_LDADD+= ../Share/llib/libdatabase.a
centerserver_LDADD+= ../Share/llib/libringbuffer.a
centerserver_LDADD+= ../Share/llib/libcenterserverlogic.a
centerserver_LDADD+= ../Share/llib/libprotobuf.a
centerserver_LDADD+= ../Share/llib/libmysqlclient.a

INCLUDES = -I@top_srcdir@/Share/Header/
INCLUDES += -I./CenterServer/
INCLUDES += -I../Share/Header/
INCLUDES += -I../Share/Header/Base/
INCLUDES += -I../Share/Header/Server/
INCLUDES += -I../Share/Header/CenterServer/

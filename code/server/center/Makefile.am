bin_PROGRAMS = centerserver

centerserver_SOURCES = CenterServer/Main.cpp
centerserver_SOURCES += CenterServer/CenterServer.cpp
centerserver_SOURCES += CenterServer/Config/CenterServerConfig.cpp
centerserver_SOURCES += CenterServer/Connection/AppConnection.cpp
centerserver_SOURCES += CenterServer/Connection/ClientConnection.cpp
centerserver_SOURCES += CenterServer/Connection/DataConnection.cpp
centerserver_SOURCES += CenterServer/Connection/WebConnection.cpp

SUBDIRS = CenterServerLogic
centerserver_LDFLAGS = -static -std=c++0x
centerserver_LDADD = -lpthread
centerserver_LDADD+= ../../share/llib/libcommon.a
centerserver_LDADD+= ../../share/llib/libcenterserverlogic.a
centerserver_LDADD+= ../../share/llib/libprotobuf.a
centerserver_LDADD+= ../../share/llib/libmysqlclient.a

INCLUDES = -I@top_srcdir@/share/header/
INCLUDES += -I./CenterServer/
INCLUDES += -I../../share/header/
INCLUDES += -I../../share/header/Base/
INCLUDES += -I../../share/header/Server/
INCLUDES += -I../../share/header/CenterServer/

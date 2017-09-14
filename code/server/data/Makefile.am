bin_PROGRAMS = dataserver

dataserver_SOURCES = data_server/Main.cpp
dataserver_SOURCES += data_server/DataServer.cpp
dataserver_SOURCES += data_server/Config/DataServerConfig.cpp
dataserver_SOURCES += data_server/Connection/ClientConnection.cpp
dataserver_SOURCES += data_server/Connection/DataConnection.cpp

SUBDIRS = data_logic
dataserver_LDFLAGS = -static -std=c++0x
dataserver_LDADD = -lpthread
dataserver_LDADD+= ../../share/llib/libcommon.a
dataserver_LDADD+= ../../share/llib/libdataserverlogic.a
dataserver_LDADD+= ../../share/llib/libprotobuf.a
dataserver_LDADD+= ../../share/llib/libmysqlclient.a

INCLUDES = -I@top_srcdir@/share/header/
INCLUDES += -I./CenterServer/
INCLUDES += -I../../share/header/
INCLUDES += -I../../share/header/Base/
INCLUDES += -I../../share/header/Server/
INCLUDES += -I../../share/header/DataServer/

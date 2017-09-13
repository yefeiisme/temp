noinst_LIBRARIES=libcommon.a

libcommon_a_SOURCES = Common/common.cpp
libcommon_a_SOURCES += Daemon/Daemon.cpp
libcommon_a_SOURCES += DataBase/Mysql/MysqlQuery.cpp
libcommon_a_SOURCES += DataBase/Mysql/MysqlResult.cpp
libcommon_a_SOURCES += DataBase/Mysql/MysqlResultSet.cpp
libcommon_a_SOURCES += DataBase/Mysql/ProcedureSqlObject.cpp
libcommon_a_SOURCES += FileLog/FileLog.cpp
libcommon_a_SOURCES += FileReader/KDebug.cpp
libcommon_a_SOURCES += FileReader/KFile.cpp
libcommon_a_SOURCES += FileReader/KFilePath.cpp
libcommon_a_SOURCES += FileReader/KIniFile.cpp
libcommon_a_SOURCES += FileReader/KLinkArray.cpp
libcommon_a_SOURCES += FileReader/KMemBase.cpp
libcommon_a_SOURCES += FileReader/KMemClass.cpp
libcommon_a_SOURCES += FileReader/KMemStack.cpp
libcommon_a_SOURCES += FileReader/KPakFile.cpp
libcommon_a_SOURCES += FileReader/KStrBase.cpp
libcommon_a_SOURCES += FileReader/KTabFile.cpp
libcommon_a_SOURCES += Network/Client/client_network_select.cpp
libcommon_a_SOURCES += Network/conn_info/client_conn_info.cpp
libcommon_a_SOURCES += Network/conn_info/conn_info.cpp
libcommon_a_SOURCES += Network/conn_info/server_conn_info.cpp
libcommon_a_SOURCES += Network/Server/server_network_epoll.cpp
libcommon_a_SOURCES += RingBuffer/RingBuffer.cpp

AUTOMAKE_OPTIONS=foreign

#SUBDIRS = Common Daemon DataBase

libcommon_a_LDFLAGS = -static -std=c++0x

INCLUDES = -I@top_srcdir@/Share/Header/
INCLUDES += -I../share/header/
INCLUDES += -I../share/header/Base/
INCLUDES += -I../share/header/mysql/linux/
SET(LIBCENTERSERVERLOGIC_SRC
CenterServerLogic.cpp
Config/CenterServerLogicConfig.cpp
Client/App_Server_Protocol.pb.cc
Client/AppClient.cpp
Client/Client.cpp
Client/DataClient.cpp
Client/Web_Server_Protocol.pb.cc
Client/WebClient.cpp
SensorDB/SensorDBConn.cpp
)
include_directories (
	"./"
	"../../../share/header"
	"../../../share/header/base"
	"../../../share/header/CenterServer"
)
ADD_LIBRARY(centerserverlogic STATIC ${LIBCENTERSERVERLOGIC_SRC})
INSTALL(TARGETS centerserverlogic
        ARCHIVE DESTINATION lib)

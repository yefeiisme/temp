SET(LIBGAMESERVERLOGIC_SRC
GameServerLogic.cpp
Player/Player.cpp
Player/PlayerProtocol.cpp
)
include_directories (
	"./"
	"../../share/header"
	"../../share/header/base"
	"../../share/header/GameServer"
)
ADD_LIBRARY(gameserverlogic STATIC ${LIBGAMESERVERLOGIC_SRC})
INSTALL(TARGETS gameserverlogic
        ARCHIVE DESTINATION lib)

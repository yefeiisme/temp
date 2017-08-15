SET(LIBDATABASE_SRC
Mysql/MysqlQuery.cpp
Mysql/MysqlResult.cpp
)
include_directories (
	"./"
	"../../Share/Header"
	"../../Share/Header/Base"
)
ADD_LIBRARY(database STATIC ${LIBDATABASE_SRC})
INSTALL(TARGETS database
        ARCHIVE DESTINATION lib)

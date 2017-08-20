SET(LIBDATABASE_SRC
Mysql/MysqlQuery.cpp
Mysql/MysqlResult.cpp
Mysql/ProcedureSqlObject.cpp
)
include_directories (
	"./"
	"../../Share/Header"
	"../../Share/Header/Base"
	"../../Share/Header/mysql/linux"
)
ADD_LIBRARY(database STATIC ${LIBDATABASE_SRC})
INSTALL(TARGETS database
        ARCHIVE DESTINATION lib)

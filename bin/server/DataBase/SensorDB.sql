SET FOREIGN_KEY_CHECKS=0;

DROP DATABASE IF EXISTS `SensorDB`;

CREATE DATABASE `SensorDB` CHARACTER SET 'utf8' COLLATE 'utf8_general_ci';

USE `SensorDB`;

CREATE TABLE `sensor` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `SceneID` smallint unsigned NOT NULL DEFAULT '0',
  `Type` tinyint unsigned NOT NULL,
  `Value1` double NOT NULL DEFAULT '0',
  `Value2` double NOT NULL DEFAULT '0',
  `Value3` double NOT NULL DEFAULT '0',
  `Value4` double NOT NULL DEFAULT '0',
  `AvgValue1` double DEFAULT NULL,
  `AvgValue2` double DEFAULT NULL,
  `AvgValue3` double DEFAULT NULL,
  `OffsetValue1` double DEFAULT NULL,
  `OffsetValue2` double DEFAULT NULL,
  `OffsetValue3` double DEFAULT NULL,
  `AlarmState` int NOT NULL DEFAULT '0',
  `SlopeID` smallint unsigned NOT NULL,
  `DataTime` datetime NOT NULL,
  `Longitude` double NOT NULL,
  `Latitude` double NOT NULL,
  `VideoUrl` mediumtext,
  `Description` mediumtext,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `sensor_data` (
  `ID` int unsigned NOT NULL,
  `SceneID` smallint unsigned NOT NULL DEFAULT '0',
  `Type` tinyint unsigned NOT NULL,
  `Value1` double NOT NULL DEFAULT '0',
  `Value2` double NOT NULL DEFAULT '0',
  `Value3` double NOT NULL DEFAULT '0',
  `Value4` double NOT NULL DEFAULT '0',
  `AverageValue1` double NOT NULL DEFAULT '0',
  `AverageValue2` double NOT NULL DEFAULT '0',
  `AverageValue3` double NOT NULL DEFAULT '0',
  `OffsetValue1` double NOT NULL DEFAULT '0',
  `OffsetValue2` double NOT NULL DEFAULT '0',
  `OffsetValue3` double NOT NULL DEFAULT '0',
  `AlarmState` int NOT NULL DEFAULT '0',
  `DataTime` datetime NOT NULL,
  KEY `ID` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `sensor_type` (
  `Type` tinyint unsigned NOT NULL,
  `SlopeID` smallint unsigned NOT NULL,
  `Name` varchar(64) DEFAULT NULL,
  `Value1` double DEFAULT NULL,
  `Value2` double DEFAULT NULL,
  `Value3` double DEFAULT NULL,
  `AlarmValue1` double DEFAULT NULL,
  `AlarmValue2` double DEFAULT NULL,
  `AlarmValue3` double DEFAULT NULL,
  `AlarmValue4` double DEFAULT NULL,
  KEY `Type`(`Type`),
  KEY `SlopeID`(`SlopeID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `server` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `IP` varchar(16) CHARACTER SET latin1 NOT NULL,
  `Port` int unsigned DEFAULT NULL,
  `ClientType` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `slope` (
  `ID` smallint unsigned NOT NULL AUTO_INCREMENT,
  `SceneID` smallint unsigned NOT NULL DEFAULT '0',
  `Type` tinyint unsigned NOT NULL,
  `Name` varchar(64) DEFAULT NULL,
  `Longitude` double NOT NULL DEFAULT '0',
  `Latitude` double NOT NULL DEFAULT '0',
  `OwnerID` int unsigned NOT NULL,
  `State` int unsigned NOT NULL DEFAULT '0',
  `VideoUrl` mediumtext,
  `Description` mediumtext,
  `LastDataTime` datetime,
  PRIMARY KEY (`ID`),
  KEY `SlopeIndex` (`SlopeIndex`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `user` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `Account` varchar(64) NOT NULL,
  `Password` varchar(64) NOT NULL,
  `Name` varchar(64) NOT NULL,
  `Addr` varchar(40) DEFAULT NULL,
  `TelNum` varchar(32) DEFAULT NULL,
  `GroupID` int unsigned NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `Account` (`Account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `user_group` (
  `GroupID` smallint unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(20) DEFAULT NULL,
  `ServerID` smallint unsigned DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `authority` (
  `AuthorityID` smallint unsigned NOT NULL AUTO_INCREMENT,
  `ParentID` smallint unsigned NOT NULL DEFAULT '0',
  `Url` mediumtext,
  `IconUrl` mediumtext,
  `Description` mediumtext,
  PRIMARY KEY (`AuthorityID`),
  KEY `ParentID` (`ParentID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `group_authority` (
  `GroupID` smallint unsigned NOT NULL DEFAULT '0',
  `AuthorityID` smallint unsigned NOT NULL DEFAULT '0',
  KEY `GroupID` (`GroupID`),
  KEY `AuthorityID` (`AuthorityID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

delimiter // ;

DROP PROCEDURE IF EXISTS `AppLogin`;
CREATE PROCEDURE `AppLogin`(IN paramAccount VARCHAR(64), IN paramPasword VARCHAR(64))
BEGIN
	select ID,count(*) from user where Account=paramAccount and Password=md5(paramPasword);
    select ID,IP,Port from server where ClientType=1;
END;

DROP PROCEDURE IF EXISTS `LoadAllList`;
CREATE PROCEDURE `LoadAllList`(IN paramAccount INTEGER UNSIGNED)
BEGIN
	select ID,SceneID,Type,Name,Longitude,Latitude,State,Description,UNIX_TIMESTAMP(LastDataTime) from slope where OwnerID=paramAccount;
	select ID,Type,Value1,Value2,Value3,Value4,AvgValue1,AvgValue2,AvgValue3,OffsetValue1,OffsetValue2,OffsetValue3,AlarmState,SlopeID,Longitude,Latitude,VideoUrl,Description from sensor where 
	SlopeID in (select ID from slope where OwnerID=paramAccount);
END;

DROP PROCEDURE IF EXISTS `LoadSensorHistory`;
CREATE PROCEDURE `LoadSensorHistory`(IN paramSensorID INTEGER UNSIGNED, IN paramBeginTime INTEGER, IN paramEndTime INTEGER)
BEGIN
	DECLARE _Interval int default 0;
	DECLARE _RecordCount int default 0;
	DECLARE _MinDateTime int default 0;
	DECLARE _MaxDateTime int default 0;
	DECLARE _AvgValue1 double default 0;
	DECLARE _AvgValue2 double default 0;
	DECLARE _AvgValue3 double default 0;
	
	select count(*),min(UNIX_TIMESTAMP(DataTime)),max(UNIX_TIMESTAMP(DataTime)),AVG(Value1),AVG(Value2),AVG(Value3) into _RecordCount,_MinDateTime,_MaxDateTime,_AvgValue1,_AvgValue2,_AvgValue3 from sensor_data where ID=paramSensorID and UNIX_TIMESTAMP(DataTime) between paramBeginTime and paramEndTime;
	
	if _RecordCount > 200 then
		set _RecordCount	= 200;
	end if;
	
	set _Interval	= (_MaxDateTime - _MinDateTime) DIV _RecordCount;

	select ID,Longitude,Latitude,_MinDateTime,_MaxDateTime,_Interval,_AvgValue1,_AvgValue2,_AvgValue3 from sensor where ID=paramSensorID;
	select min(Value1),min(Value2),min(Value3),MAX(Value1),MAX(Value2),max(Value3),min(OffsetValue1),min(OffsetValue2),min(OffsetValue3),MAX(OffsetValue1),MAX(OffsetValue2),max(OffsetValue3) from sensor_data where ID=paramSensorID and UNIX_TIMESTAMP(DataTime) between _MinDateTime and _MaxDateTime group by UNIX_TIMESTAMP(DataTime)-UNIX_TIMESTAMP(DataTime)%_Interval;
END;

DROP PROCEDURE IF EXISTS `LoadSensorList`;
CREATE PROCEDURE `LoadSensorList`(IN paramSlopeID INTEGER UNSIGNED)
BEGIN
	select ID,SceneID,Type,Value1,Value2,Value3,Value4,AvgValue1,AvgValue2,AvgValue3,OffsetValue1,OffsetValue2,OffsetValue3,AlarmState,SlopeID,Longitude,Latitude,VideoUrl,Description from sensor where SlopeID=paramSlopeID order by Type;
END;

DROP PROCEDURE IF EXISTS `LoadSensorListByType`;
CREATE PROCEDURE `LoadSensorListByType`(IN paramSlopeID INTEGER UNSIGNED, IN paramSensorType INTEGER UNSIGNED)
BEGIN
	select ID,SceneID,Type,Value1,Value2,Value3,Value4,AvgValue1,AvgValue2,AvgValue3,OffsetValue1,OffsetValue2,OffsetValue3,AlarmState,SlopeID,Longitude,Latitude,VideoUrl,Description from sensor where SlopeID=paramSlopeID and Type=paramSensorType order by Type;
END;

DROP PROCEDURE IF EXISTS `LoadSlopeList`;
CREATE PROCEDURE `LoadSlopeList`(IN paramAccount INTEGER UNSIGNED, IN paramServerID INTEGER UNSIGNED)
BEGIN
	select ID,SceneID,Type,Name,Longitude,Latitude,State,VideoUrl,Description,UNIX_TIMESTAMP(LastDataTime) from slope;
END;

DROP PROCEDURE IF EXISTS `LoadSlopeByID`;
CREATE PROCEDURE `LoadSlopeByID`(IN paramAccount INTEGER UNSIGNED, IN paramServerID INTEGER UNSIGNED, IN paramSlopeID INTEGER UNSIGNED)
BEGIN
	select ID,SceneID,Type,Name,Longitude,Latitude,State,VideoUrl,Description,UNIX_TIMESTAMP(LastDataTime) from slope where ID=paramSlopeID;
END;

DROP PROCEDURE IF EXISTS `WebLogin`;
CREATE PROCEDURE `WebLogin`(IN paramAccount VARCHAR(64), IN paramPasword VARCHAR(64))
BEGIN
	select ID,count(*) from user where Account=paramAccount and Password=md5(paramPasword);
    select ID,IP,Port from server where ClientType=2;
END;

DROP PROCEDURE IF EXISTS `LoadAllAuthority`;
CREATE PROCEDURE `LoadAllAuthority`(IN paramAccountID INTEGER UNSIGNED)
BEGIN
	DECLARE nGroupID INTEGER UNSIGNED default 0;
	select GroupID into nGroupID from user where ID=paramAccountID;
    select authority.AuthorityID,authority.ParentID,authority.Url,authority.IconUrl,authority.Description from authority,group_authority
	where group_authority.GroupID=nGroupID and authority.AuthorityID=group_authority.AuthorityID;
END;

DROP PROCEDURE IF EXISTS `LoadAuthorityByID`;
CREATE PROCEDURE `LoadAuthorityByID`(IN paramAccountID INTEGER UNSIGNED,IN paramAuthorityID INTEGER UNSIGNED)
BEGIN
	DECLARE nGroupID INTEGER UNSIGNED default 0;
	select GroupID into nGroupID from user where ID=paramAccountID;
    select authority.AuthorityID,authority.ParentID,authority.Url,authority.IconUrl,authority.Description from authority,group_authority
	where group_authority.GroupID=nGroupID and authority.AuthorityID=group_authority.AuthorityID and authority.ParentID =paramAuthorityID;
END;

DROP PROCEDURE IF EXISTS `ModifyPassword`;
CREATE PROCEDURE `ModifyPassword`(IN paramAccount VARCHAR(64), IN paramPasword VARCHAR(64))
BEGIN
	update user set Password=md5(paramPasword) where Account=paramAccount;
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
END;

DROP PROCEDURE IF EXISTS `AddSlope`;
CREATE PROCEDURE `AddSlope`(IN paramSceneID INTEGER UNSIGNED,IN paramType INTEGER UNSIGNED,IN paramName VARCHAR(64),IN paramOwnerID INTEGER UNSIGNED,IN paramUrl mediumtext,IN paramDesc mediumtext)
BEGIN
	insert into slope(SceneID,Type,Name,OwnerID,VideoUrl,Description) value(paramSceneID,paramType,paramName,paramOwnerID,paramUrl,paramDesc);
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
	
	select LAST_INSERT_ID(),paramSceneID,paramType,paramName,0,0,paramOwnerID,paramUrl,paramDesc;
END;

DROP PROCEDURE IF EXISTS `DeleteSlope`;
CREATE PROCEDURE `DeleteSlope`(IN paramSlopeID INTEGER UNSIGNED)
BEGIN
	delete from slope where ID=paramSlopeID;
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
END;

DROP PROCEDURE IF EXISTS `UpdateSlope`;
CREATE PROCEDURE `UpdateSlope`(IN paramSlopeID INTEGER UNSIGNED,IN paramSceneID INTEGER UNSIGNED,IN paramType INTEGER UNSIGNED,IN paramName VARCHAR(64),IN paramLongitude DOUBLE,IN paramLatitude DOUBLE,IN paramOwnerID INTEGER UNSIGNED,IN paramUrl mediumtext,IN paramDesc mediumtext)
BEGIN
	update slope set SceneID=paramSceneID,Type=paramType,Name=paramName,Longitude=paramLongitude,Latitude=paramLatitude,OwnerID=paramOwnerID,VideoUrl=paramUrl,Description=paramDesc where ID=paramSlopeID;
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
	
	select paramSlopeID,paramSceneID,paramType,paramName,paramLongitude,paramLatitude,paramOwnerID,paramUrl,paramDesc;
END;

DROP PROCEDURE IF EXISTS `AddSensor`;
CREATE PROCEDURE `AddSensor`(IN paramSceneID INTEGER UNSIGNED,IN paramType INTEGER UNSIGNED, IN paramSlopeID INTEGER UNSIGNED, IN paramLongitude DOUBLE, IN paramLatitude DOUBLE, IN paramUrl mediumtext, IN paramDesc mediumtext)
BEGIN
	insert into sensor(SceneID,Type,SlopeID,Longitude,Latitude,VideoUrl,Description) value(paramSceneID,paramType,paramSlopeID,paramLongitude,paramLatitude,paramUrl,paramDesc);
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
	
	select LAST_INSERT_ID(),paramSceneID,paramType,paramSlopeID,paramLongitude,paramLatitude,paramUrl,paramDesc;
END;

DROP PROCEDURE IF EXISTS `DeleteSensor`;
CREATE PROCEDURE `DeleteSensor`(IN paramSensorID INTEGER UNSIGNED)
BEGIN
	delete from sensor where ID=paramSensorID;
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
END;

DROP PROCEDURE IF EXISTS `UpdateSensor`;
CREATE PROCEDURE `UpdateSensor`(IN paramSensorID INTEGER UNSIGNED,IN paramSceneID INTEGER UNSIGNED,IN paramType INTEGER UNSIGNED,IN paramSlopeID INTEGER UNSIGNED,IN paramLongitude DOUBLE,IN paramLatitude DOUBLE,IN paramUrl mediumtext,IN paramDesc mediumtext)
BEGIN
	update sensor set SceneID=paramSceneID,Type=paramType,SlopeID=paramSlopeID,Longitude=paramLongitude,Latitude=paramLatitude,VideoUrl=paramUrl,Description=paramDesc where ID=paramSensorID;
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
	
	select paramSensorID,paramSceneID,paramType,paramSlopeID,paramLongitude,paramLatitude,paramUrl,paramDesc;
END;

DROP PROCEDURE IF EXISTS `FindSensor`;
CREATE PROCEDURE `FindSensor`(IN paramSlopeID INTEGER UNSIGNED, IN paramName VARCHAR(64), IN paramSensorID INTEGER UNSIGNED, IN paramSensorType INTEGER UNSIGNED)
BEGIN
	set @strSql = 'select sensor.ID,sensor.SceneID,sensor.Type,sensor.Value1,sensor.Value2,sensor.Value3,sensor.Value4,sensor.AvgValue1,sensor.AvgValue2,sensor.AvgValue3,sensor.OffsetValue1,sensor.OffsetValue2,sensor.OffsetValue3,sensor.AlarmState,sensor.SlopeID,sensor.Longitude,sensor.Latitude,sensor.VideoUrl,sensor.Description,slope.Name from sensor left join slope on sensor.SlopeID = slope.id';
	set @nCount	= 0;

	if paramSlopeID > 0 then
		set @strSql = concat(@strSql, ' where sensor.SlopeID=', paramSlopeID);
		set @nCount = @nCount + 1;
	end if;

	if paramSensorID > 0 then
		if @nCount = 0 then
			set @strSql = concat(@strSql, ' where sensor.ID=', paramSensorID);
		else
			set @strSql = concat(@strSql, ' and sensor.ID=', paramSensorID);
		end if;
		set @nCount = @nCount + 1;
	end if;

	if paramSensorType > 0 then
		if @nCount = 0 then
			set @strSql = concat(@strSql, ' where sensor.Type=', paramSensorType);
	else
			set @strSql = concat(@strSql, ' and sensor.Type=', paramSensorType);
		end if;
		set @nCount = @nCount + 1;
	end if;

	if paramName <> '' then
		if @nCount = 0 then
			set @strSql = concat(@strSql, ' where sensor.SlopeID IN (select slope.ID from slope where slope.Name like \'%', paramName,'%\'', ' ESCAPE \'/\')');
		else
			set @strSql = concat(@strSql, ' and sensor.SlopeID IN (select slope.ID from slope where slope.Name like \'%', paramName,'%\'', ' ESCAPE \'/\')');
		end if;
	end if;

	PREPARE stmt FROM @strSql;
	EXECUTE stmt;
	deallocate prepare stmt;
END;

DROP PROCEDURE IF EXISTS `FindSlope`;
CREATE PROCEDURE `FindSlope`(IN paramID INTEGER UNSIGNED,IN paramName varchar(64))
BEGIN
	set @strSql = 'select ID,SceneID,Type,Name,Longitude,Latitude,State,VideoUrl,Description,UNIX_TIMESTAMP(LastDataTime) from slope';
    set @nCount	= 0;
    
    if paramID > 0 then
    	set @strSql = concat(@strSql, ' where ID=', paramID);
    	set @nCount = @nCount + 1;
    end if;
    
    if paramName <> '' then
    	if @nCount = 0 then
    		set @strSql = concat(@strSql, ' where Name like \'%', paramName,'%\'', ' ESCAPE \'/\'');
        else
    		set @strSql = concat(@strSql, ' and Name like \'%', paramName,'%\'', ' ESCAPE \'/\'');
        end if;
    end if;
    
    PREPARE stmt FROM @strSql;
    EXECUTE stmt;
    deallocate prepare stmt;
END;

DROP PROCEDURE IF EXISTS `LoadUserList`;
CREATE PROCEDURE `LoadUserList`()
BEGIN
	select ID,Name,GroupID from user;
END;

DROP PROCEDURE IF EXISTS `CreateUser`;
CREATE PROCEDURE `CreateUser`(IN paramAccount varchar(64),IN paramPassword varchar(64),IN paramName varchar(64),IN paramGroupID INTEGER UNSIGNED)
BEGIN
	insert into user(Account,Password,Name,GroupID) value(paramAccount,md5(paramPassword),paramName,paramGroupID);
	select ID,Name,GroupID from user where ID=LAST_INSERT_ID();
END;

DROP PROCEDURE IF EXISTS `ModifyUser`;
CREATE PROCEDURE `ModifyUser`(IN paramID INTEGER UNSIGNED,IN paramGroupID INTEGER UNSIGNED,IN paramPassword varchar(64))
BEGIN
	update user set GroupID=paramGroupID,Password=md5(paramPassword) where ID=paramID;
	select paramID,paramGroupID;
END;

DROP PROCEDURE IF EXISTS `RemoveUser`;
CREATE PROCEDURE `RemoveUser`(IN paramID INTEGER UNSIGNED)
BEGIN
	delete from user where ID=paramID;
	select paramID;
END;

DROP PROCEDURE IF EXISTS `LoadGroupList`;
CREATE PROCEDURE `LoadGroupList`()
BEGIN
	select GroupID,Name from user_group;
END;

DROP PROCEDURE IF EXISTS `AddSensorData`;
CREATE PROCEDURE `AddSensorData`(
	IN paramSlopeSceneID INTEGER UNSIGNED,
	IN paramSlopeType INTEGER UNSIGNED,
	IN paramTime INTEGER,
	IN paramLongitude double,
	IN paramLatitude double,
	IN paramSensorSceneID INTEGER UNSIGNED,
	IN paramSensorType INTEGER UNSIGNED,
	IN paramValue1 double,
	IN paramValue2 double,
	IN paramValue3 double,
	IN paramValue4 double)
BEGIN
	DECLARE _SlopeID smallint UNSIGNED default 0;
	DECLARE _SensorID int UNSIGNED default 0;
	DECLARE _AvgValue1 double default 0;
	DECLARE _AvgValue2 double default 0;
	DECLARE _AvgValue3 double default 0;
	DECLARE _OffsetValue1 double default 0;
	DECLARE _OffsetValue2 double default 0;
	DECLARE _OffsetValue3 double default 0;
	DECLARE _AlarmValue1 double default 0;
	DECLARE _AlarmValue2 double default 0;
	DECLARE _AlarmValue3 double default 0;
	DECLARE _AlarmValue4 double default 0;
	DECLARE _AlarmState1 int UNSIGNED default 0;
	DECLARE _AlarmState2 int UNSIGNED default 0;
	DECLARE _AlarmState3 int UNSIGNED default 0;
	DECLARE _AlarmState int UNSIGNED default 0;

	select ID into _SlopeID from slope where SceneID=paramSlopeSceneID and Type=paramSlopeType;
	select ID into _SensorID from sensor where SceneID=paramSensorSceneID and Type=paramSensorType and SlopeID=_SlopeID;
	select AvgValue1,AvgValue2,AvgValue3 into _AvgValue1,_AvgValue2,_AvgValue3 from sensor where ID=_SensorID;
	select AlarmValue1,AlarmValue2,AlarmValue3,AlarmValue4 into _AlarmValue1,_AlarmValue2,_AlarmValue3,_AlarmValue4 from sensor_type where Type=paramSensorType and SlopeID=_SlopeID;

	if paramSensorType = 2 then
		set _OffsetValue1 =(_AvgValue1-paramValue1)*100000000;
		set _OffsetValue2 =(_AvgValue2-paramValue2)*100000000;
		set _OffsetValue3 =(_AvgValue3-paramValue3)*1000;
	else
		set _OffsetValue1 =_AvgValue1-paramValue1;
		set _OffsetValue2 =_AvgValue2-paramValue2;
		set _OffsetValue3 =_AvgValue3-paramValue3;
	end if;
	
	if _OffsetValue1 < AlarmValue1 then
		set @_AlarmState1 = 1;
	elseif _OffsetValue1 < AlarmValue2 then
		set @_AlarmState1 = 2;
	elseif _OffsetValue1 < AlarmValue3 then
		set @_AlarmState1 = 3;
	elseif _OffsetValue1 < AlarmValue4 then
		set @_AlarmState1 = 4;
	else
		set @_AlarmState1 = 5;
	end if;

	if _OffsetValue2 < AlarmValue1 then
		set @_AlarmState2 = 1;
	elseif _OffsetValue2 < AlarmValue2 then
		set @_AlarmState2 = 2;
	elseif _OffsetValue2 < AlarmValue3 then
		set @_AlarmState2 = 3;
	elseif _OffsetValue2 < AlarmValue4 then
		set @_AlarmState2 = 4;
	else
		set @_AlarmState2 = 5;
	end if;

	if _OffsetValue3 < AlarmValue1 then
		set @_AlarmState3 = 1;
	elseif _OffsetValue3 < AlarmValue2 then
		set @_AlarmState3 = 2;
	elseif _OffsetValue3 < AlarmValue3 then
		set @_AlarmState3 = 3;
	elseif _OffsetValue3 < AlarmValue4 then
		set @_AlarmState3 = 4;
	else
		set @_AlarmState3 = 5;
	end if;
	
	set @_AlarmState	= _AlarmState1;
	
	if _AlarmState > _AlarmState2 then
		set @_AlarmState	= _AlarmState2;
	end if;

	if _AlarmState > _AlarmState3 then
		set @_AlarmState	= _AlarmState3;
	end if;

	update slope set Longitude=paramLongitude,Latitude=paramLatitude,LastDataTime=FROM_UNIXTIME(paramTime) where ID=_SlopeID;
	update sensor set Value1=paramValue1,Value2=paramValue2,Value3=paramValue3,Value4=paramValue4,OffsetValue1=_OffsetValue1,OffsetValue2=_OffsetValue2,OffsetValue3=_OffsetValue3,AlarmState=_AlarmState where ID=_SensorID;
	insert into sensor_data(ID,SceneID,Type,Value1,Value2,Value3,Value4,DataTime,DataTime1,OffsetValue1,OffsetValue2,OffsetValue3,AlarmState) value(_SensorID,paramSensorSceneID,paramSensorType,paramValue1,paramValue2,paramValue3,paramValue4,FROM_UNIXTIME(paramTime),paramTime,_OffsetValue1,_OffsetValue2,_OffsetValue3,_AlarmState);
END;

DROP PROCEDURE IF EXISTS `ModifyAlarmValue`;
CREATE PROCEDURE `ModifyAlarmValue`(IN paramSensorType tinyint UNSIGNED,IN paramSlopeID smallint UNSIGNED,IN paramValue1 double,IN paramValue2 double,IN paramValue3 double,IN paramValue4 double)
BEGIN
	update sensor_type set AlarmValue1=paramValue1,AlarmValue2=paramValue2,AlarmValue3=paramValue3,AlarmValue4=paramValue4 where Type=paramSensorType and SlopeID=paramSlopeID;
	select paramSensorType,paramSlopeID,paramValue1,paramValue2,paramValue3,paramValue4;
END;

DROP PROCEDURE IF EXISTS `LoadAlarmValue`;
CREATE PROCEDURE `LoadAlarmValue`(IN paramSlopeID smallint UNSIGNED,IN paramSensorType tinyint UNSIGNED)
BEGIN
	select Type,SlopeID,AlarmValue1,AlarmValue2,AlarmValue3,AlarmValue4 from sensor_type where SlopeID=paramSlopeID and Type=paramSensorType;
END;

DROP PROCEDURE IF EXISTS `StartSlope`;
CREATE PROCEDURE `StartSlope`(IN paramSlopeID smallint UNSIGNED,IN paramStartType tinyint UNSIGNED,IN paramClearHistory tinyint UNSIGNED)
BEGIN
	DECLARE _StartResult int UNSIGNED default 0;
	DECLARE _AvgValue1 double default 0;
	DECLARE _AvgValue2 double default 0;
	DECLARE _AvgValue3 double default 0;
	
	if paramStartType = 0 then
		update sensor set AvgValue1=Value1,AvgValue2=Value2,AvgValue3=Value3 where SlopeID=paramSlopeID;
	else
		update sensor set AvgValue1=Value1,AvgValue2=Value2,AvgValue3=Value3 where SlopeID=paramSlopeID;
	end if;
	
	if paramClearHistory <> 0 then
		delete from sensor_data where ID in (select ID from sensor where SlopeID=paramSlopeID);
	end if;
	
	set @_StartResult = 1;
	select paramSlopeID,_StartResult;
END;

//
delimiter ; //

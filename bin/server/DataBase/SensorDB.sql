SET FOREIGN_KEY_CHECKS=0;

DROP DATABASE IF EXISTS `SensorDB`;

CREATE DATABASE `SensorDB` CHARACTER SET 'utf8' COLLATE 'utf8_general_ci';

USE `SensorDB`;

CREATE TABLE `sensor` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `Type` tinyint unsigned NOT NULL,
  `Value1` double NOT NULL DEFAULT '0',
  `Value2` double NOT NULL DEFAULT '0',
  `Value3` double NOT NULL DEFAULT '0',
  `AvgValue1` double DEFAULT NULL,
  `AvgValue2` double DEFAULT NULL,
  `AvgValue3` double DEFAULT NULL,
  `OffsetValue1` double DEFAULT NULL,
  `OffsetValue2` double DEFAULT NULL,
  `OffsetValue3` double DEFAULT NULL,
  `AlarmState` int NOT NULL DEFAULT '0',
  `SlopeID` int unsigned NOT NULL,
  `DataTime` datetime NOT NULL,
  `Longitude` double NOT NULL,
  `Latitude` double NOT NULL,
  `VideoUrl` mediumtext,
  `Description` mediumtext,
  KEY `ID` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `sensor_data` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `Type` tinyint unsigned NOT NULL,
  `Value1` double NOT NULL DEFAULT '0',
  `Value2` double NOT NULL DEFAULT '0',
  `Value3` double NOT NULL DEFAULT '0',
  `AverageValue1` double NOT NULL DEFAULT '0',
  `AverageValue2` double NOT NULL DEFAULT '0',
  `AverageValue3` double NOT NULL DEFAULT '0',
  `OffsetValue1` double NOT NULL DEFAULT '0',
  `OffsetValue2` double NOT NULL DEFAULT '0',
  `OffsetValue3` double NOT NULL DEFAULT '0',
  `AlarmState` int NOT NULL DEFAULT '0',
  `SlopeID` int unsigned NOT NULL,
  `DataTime` datetime NOT NULL,
  `Longitude` double NOT NULL,
  `Latitude` double NOT NULL,
  KEY `ID` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `sensor_type` (
  `ID` tinyint unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(64) DEFAULT NULL,
  `Value1` double DEFAULT NULL,
  `Value2` double DEFAULT NULL,
  `Value3` double DEFAULT NULL,
  PRIMARY KEY (`ID`)
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
  `Type` tinyint unsigned NOT NULL,
  `Name` varchar(64) DEFAULT NULL,
  `Longitude` double NOT NULL,
  `Latitude` double NOT NULL,
  `OwnerID` int unsigned NOT NULL,
  `State` int unsigned NOT NULL DEFAULT '0',
  `VideoUrl` mediumtext,
  PRIMARY KEY (`ID`)
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
	select ID,Type,Name,Longitude,Latitude,State from slope where OwnerID=paramAccount;
	select ID,Type,Value1,Value2,Value3,AvgValue1,AvgValue2,AvgValue3,OffsetValue1,OffsetValue2,OffsetValue3,AlarmState,SlopeID,Longitude,Latitude,VideoUrl,Description from sensor where 
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
	select ID,Type,Value1,Value2,Value3,AvgValue1,AvgValue2,AvgValue3,OffsetValue1,OffsetValue2,OffsetValue3,AlarmState,SlopeID,Longitude,Latitude,VideoUrl,Description from sensor where SlopeID=paramSlopeID order by Type;
END;

DROP PROCEDURE IF EXISTS `LoadSensorListByType`;
CREATE PROCEDURE `LoadSensorListByType`(IN paramSlopeID INTEGER UNSIGNED, IN paramSensorType INTEGER UNSIGNED)
BEGIN
	select ID,Type,Value1,Value2,Value3,AvgValue1,AvgValue2,AvgValue3,OffsetValue1,OffsetValue2,OffsetValue3,AlarmState,SlopeID,Longitude,Latitude,VideoUrl,Description from sensor where SlopeID=paramSlopeID and Type=paramSensorType order by Type;
END;

DROP PROCEDURE IF EXISTS `LoadSlopeList`;
CREATE PROCEDURE `LoadSlopeList`(IN paramAccount INTEGER UNSIGNED, IN paramServerID INTEGER UNSIGNED)
BEGIN
	select ID,Type,Name,Longitude,Latitude,State,VideoUrl from slope where OwnerID=paramAccount;
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
	update user set Password=md5(paramPasword);
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
END;

DROP PROCEDURE IF EXISTS `AddSlope`;
CREATE PROCEDURE `AddSlope`(IN paramType INTEGER UNSIGNED, IN paramName VARCHAR(64), IN paramLongitude DOUBLE, IN paramLatitude DOUBLE, IN paramOwnerID INTEGER UNSIGNED, IN paramUrl mediumtext)
BEGIN
	insert into slope(Type,Name,Longitude,Latitude,OwnerID,VideoUrl) value(paramType,paramName,paramLongitude,paramLatitude,paramOwnerID,paramUrl);
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
	
	select LAST_INSERT_ID(),paramType,paramName,paramLongitude,paramLatitude,paramOwnerID,paramUrl;
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
CREATE PROCEDURE `UpdateSlope`(IN paramSlopeID INTEGER UNSIGNED,IN paramType INTEGER UNSIGNED,IN paramName VARCHAR(64),IN paramLongitude DOUBLE,IN paramLatitude DOUBLE,IN paramOwnerID INTEGER UNSIGNED,IN paramUrl mediumtext)
BEGIN
	update slope set Type=paramType,Name=paramName,Longitude=paramLongitude,Latitude=paramLatitude,OwnerID=paramOwnerID,VideoUrl=paramUrl where ID=paramSlopeID;
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
	
	select paramSlopeID,paramType,paramName,paramLongitude,paramLatitude,paramOwnerID,paramUrl;
END;

DROP PROCEDURE IF EXISTS `AddSensor`;
CREATE PROCEDURE `AddSensor`(IN paramType INTEGER UNSIGNED, IN paramSlopeID INTEGER UNSIGNED, IN paramLongitude DOUBLE, IN paramLatitude DOUBLE, IN paramUrl mediumtext, IN paramDesc mediumtext)
BEGIN
	insert into sensor(Type,SlopeID,Longitude,Latitude,VideoUrl,Description) value(paramType,paramSlopeID,paramLongitude,paramLatitude,paramUrl,paramDesc);
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
	
	select LAST_INSERT_ID(),paramType,paramSlopeID,paramLongitude,paramLatitude,paramUrl,paramDesc;
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
CREATE PROCEDURE `UpdateSensor`(IN paramSensorID INTEGER UNSIGNED,IN paramType INTEGER UNSIGNED, IN paramSlopeID INTEGER UNSIGNED, IN paramLongitude DOUBLE, IN paramLatitude DOUBLE, IN paramUrl mediumtext, IN paramDesc mediumtext)
BEGIN
	update sensor set Type=paramType,SlopeID=paramSlopeID,Longitude=paramLongitude,Latitude=paramLatitude,VideoUrl=paramUrl,Description=paramDesc where ID=paramSensorID;
	
	if ROW_COUNT() > 0 then
		select 0;
	else
		select 1;
	end if;
	
	select paramSensorID,paramType,paramSlopeID,paramLongitude,paramLatitude,paramUrl,paramDesc;
END;

DROP PROCEDURE IF EXISTS `FindSensor`;
CREATE PROCEDURE `FindSensor`(IN paramSlopeID INTEGER UNSIGNED, IN paramName VARCHAR(64), IN paramSensorID INTEGER UNSIGNED, IN paramSensorType INTEGER UNSIGNED)
BEGIN
	set @strSql = 'select ID,Type,Value1,Value2,Value3,AvgValue1,AvgValue2,AvgValue3,OffsetValue1,OffsetValue2,OffsetValue3,AlarmState,SlopeID,Longitude,Latitude,VideoUrl,Description from sensor';
    set @nCount	= 0;
    
    if paramSlopeID > 0 then
    	set @strSql = concat(@strSql, ' where SlopeID=', paramSlopeID);
    	set @nCount = @nCount + 1;
    end if;
    
    if paramSensorID > 0 then
    	if @nCount = 0 then
    		set @strSql = concat(@strSql, ' where ID=', paramSensorID);
        else
    		set @strSql = concat(@strSql, ' and ID=', paramSensorID);
        end if;
    	set @nCount = @nCount + 1;
    end if;
    
    if paramSensorType > 0 then
    	if @nCount = 0 then
    		set @strSql = concat(@strSql, ' where Type=', paramSensorType);
        else
    		set @strSql = concat(@strSql, ' and Type=', paramSensorType);
        end if;
    	set @nCount = @nCount + 1;
    end if;
    
    if paramName <> '' then
    	if @nCount = 0 then
    		set @strSql = concat(@strSql, ' where SlopeID IN (select ID from slope where Name like \'%', paramName,'%\'', ' ESCAPE \'/\')');
        else
    		set @strSql = concat(@strSql, ' and SlopeID IN (select ID from slope where Name like \'%', paramName,'%\'', ' ESCAPE \'/\')');
        end if;
    end if;

    PREPARE stmt FROM @strSql;
    EXECUTE stmt;
    deallocate prepare stmt;
END;

DROP PROCEDURE IF EXISTS `FindSlope`;
CREATE PROCEDURE `FindSlope`(IN paramID INTEGER UNSIGNED,IN paramName varchar(64))
BEGIN
	set @strSql = 'select ID,Type,Name,Longitude,Latitude,State,VideoUrl from slope';
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
CREATE PROCEDURE `CreateUser`(IN paramAccount varchar(64),IN paramPasword varchar(64),IN paramName varchar(64))
BEGIN
	insert into user(Account,Password,Name) value(paramAccount,paramPasword,paramName);
	select ID,Name,GroupID from user where ID=LAST_INSERT_ID();
END;

DROP PROCEDURE IF EXISTS `ModifyUser`;
CREATE PROCEDURE `ModifyUser`(IN paramID INTEGER UNSIGNED,IN paramName varchar(64))
BEGIN
	update user set Name=paramName where ID=paramID;
	select paramID,paramName;
END;

DROP PROCEDURE IF EXISTS `RemoveUser`;
CREATE PROCEDURE `RemoveUser`(IN paramID INTEGER UNSIGNED)
BEGIN
	delete from user where ID=paramID;
	select paramID;
END;

//
delimiter ; //

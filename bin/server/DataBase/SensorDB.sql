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
  `Addr` varchar(40) DEFAULT NULL,
  `TelNum` varchar(32) DEFAULT NULL,
  `GroupID` int unsigned NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `Account` (`Account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `user_group` (
  `ID` smallint unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(20) DEFAULT NULL,
  `ServerID` smallint unsigned DEFAULT NULL,
  PRIMARY KEY (`ID`)
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
	
	select count(*),min(UNIX_TIMESTAMP(DataTime)),max(UNIX_TIMESTAMP(DataTime)) into _RecordCount,_MinDateTime,_MaxDateTime from sensor_data where ID=paramSensorID and UNIX_TIMESTAMP(DataTime) between paramBeginTime and paramEndTime;
	
	if _RecordCount > 100 then
		set _RecordCount	= 100;
	end if;
	
	set _Interval	= (_MaxDateTime - _MinDateTime) DIV _RecordCount;

	select ID,Longitude,Latitude,_MinDateTime,_MaxDateTime,_Interval from sensor where ID=paramSensorID;
	select min(OffsetValue1),min(OffsetValue2),min(OffsetValue3),MAX(OffsetValue1),MAX(OffsetValue2),max(OffsetValue3) from sensor_data where ID=paramSensorID and UNIX_TIMESTAMP(DataTime) between _MinDateTime and _MaxDateTime group by UNIX_TIMESTAMP(DataTime)-UNIX_TIMESTAMP(DataTime)%_Interval;
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

//
delimiter ; //

﻿SET FOREIGN_KEY_CHECKS=0;

DROP DATABASE IF EXISTS `SensorDB`;

CREATE DATABASE `SensorDB`
    CHARACTER SET 'utf8'
    COLLATE 'utf8_general_ci';

USE `SensorDB`;

CREATE TABLE `sensor` (
  `ID` int(4) unsigned NOT NULL AUTO_INCREMENT,
  `Type` tinyint(4) unsigned NOT NULL,
  `Value1` double NOT NULL DEFAULT '0',
  `Value2` double NOT NULL DEFAULT '0',
  `Value3` double NOT NULL DEFAULT '0',
  `AvgValue1` double DEFAULT NULL,
  `AvgValue2` double DEFAULT NULL,
  `AvgValue3` double DEFAULT NULL,
  `OffsetValue1` double DEFAULT NULL,
  `OffsetValue2` double DEFAULT NULL,
  `OffsetValue3` double DEFAULT NULL,
  `AlarmState` int(11) NOT NULL DEFAULT '0',
  `SlopeID` int(2) unsigned NOT NULL,
  `DataTime` datetime NOT NULL,
  `Longitude` double NOT NULL,
  `Latitude` double NOT NULL,
  `VideoUrl` mediumtext,
  `Description` mediumtext,
  KEY `ID` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `sensor_data` (
  `ID` int(4) unsigned NOT NULL AUTO_INCREMENT,
  `Type` tinyint(4) unsigned NOT NULL,
  `Value1` double NOT NULL DEFAULT '0',
  `Value2` double NOT NULL DEFAULT '0',
  `Value3` double NOT NULL DEFAULT '0',
  `AverageValue1` double NOT NULL DEFAULT '0',
  `AverageValue2` double NOT NULL DEFAULT '0',
  `AverageValue3` double NOT NULL DEFAULT '0',
  `OffsetValue1` double NOT NULL DEFAULT '0',
  `OffsetValue2` double NOT NULL DEFAULT '0',
  `OffsetValue3` double NOT NULL DEFAULT '0',
  `AlarmState` int(11) NOT NULL DEFAULT '0',
  `SlopeID` int(2) unsigned NOT NULL,
  `DataTime` datetime NOT NULL,
  `Longitude` double NOT NULL,
  `Latitude` double NOT NULL,
  KEY `ID` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `sensor_type` (
  `ID` tinyint(4) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(64) DEFAULT NULL,
  `Value1` double DEFAULT NULL,
  `Value2` double DEFAULT NULL,
  `Value3` double DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `server`;

CREATE TABLE `server` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `IP` varchar(16) CHARACTER SET latin1 NOT NULL,
  `Port` int(2) unsigned DEFAULT NULL,
  `ClientType` tinyint(4) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB ADEFAULT CHARSET=utf8;

CREATE TABLE `slope` (
  `ID` smallint(6) unsigned NOT NULL AUTO_INCREMENT,
  `Type` tinyint(4) unsigned NOT NULL,
  `Name` varchar(64) DEFAULT NULL,
  `Longitude` double NOT NULL,
  `Latitude` double NOT NULL,
  `OwnerID` int(4) unsigned NOT NULL,
  `State` int(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

CREATE TABLE `user` (
  `ID` int(4) unsigned NOT NULL AUTO_INCREMENT,
  `Account` varchar(64) NOT NULL,
  `Password` varchar(64) NOT NULL,
  `Addr` varchar(40) DEFAULT NULL,
  `TelNum` varchar(32) DEFAULT NULL,
  `GroupID` int(2) unsigned NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `Account` (`Account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `user_group` (
  `ID` smallint(6) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(20) DEFAULT NULL,
  `ServerID` smallint(6) unsigned DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

delimiter // ;

DROP PROCEDURE IF EXISTS `AppLogin`;

CREATE PROCEDURE `AppLogin`(IN paramAccount VARCHAR(64), IN paramPasword VARCHAR(64))
BEGIN
	select ID,count(*) from user where Account=paramAccount and Password=md5(paramPasword);
    select ID,IP,Port from server where ClientType=1;
END;

DROP PROCEDURE IF EXISTS `LoadSensorHistory`;

CREATE PROCEDURE `LoadSensorHistory`(IN paramAccount INTEGER UNSIGNED, IN paramSensorID INTEGER UNSIGNED, IN paramBeginTime INTEGER, IN paramEndTime INTEGER)
BEGIN
	DECLARE _AvgValue1 double default 0;
	DECLARE _AvgValue2 double default 0;
	DECLARE _AvgValue3 double default 0;
    
    select AVG(Value1),AVG(Value2),(Value3) into _AvgValue1,_AvgValue2,_AvgValue3 from sensor
    where SensorID=paramSensorID and DataTime between paramBeginTime and paramEndTime;
    
	select ID,Longitude,Latitude,paramBeginTime,paramEndTime,_AvgValue1,_AvgValue2,_AvgValue3,'' from sensor where SensorID=paramSensorID;
    
    select AlarmState,Value1,Value2,Value3,Value1-_AvgValue1,Value2-_AvgValue2,Value3-_AvgValue3,UNIX_TIMESTAMP(DataTime) from sensor
    where SensorID=paramSensorID and DataTime between paramBeginTime and paramEndTime group by UNIX_TIMESTAMP(DataTime)-UNIX_TIMESTAMP(DataTime)%3600;
END;

DROP PROCEDURE IF EXISTS `LoadSensorList`;

CREATE PROCEDURE `LoadSensorList`(IN paramAccount INTEGER UNSIGNED, IN paramSlopeID INTEGER UNSIGNED)
BEGIN
	select 
    ID,
    Type,
    Value1,
    Value2,
    Value3,
    AverageValue1,
    AverageValue2,
    AverageValue3,
    AlarmState,
    SlopeID,
    Longitude,
    Latitude
    
    from sensor where SlopeID=paramSlopeID;
END;

DROP PROCEDURE IF EXISTS `LoadSensorListByType`;

CREATE PROCEDURE `LoadSensorListByType`(IN paramAccount INTEGER UNSIGNED, IN paramSlopeID INTEGER UNSIGNED, IN paramSensorType INTEGER UNSIGNED)
BEGIN
	select 
    ID,
    Type,
    Value1,
    Value2,
    Value3,
    AverageValue1,
    AverageValue2,
    AverageValue3,
    AlarmState,
    SlopeID,
    Longitude,
    Latitude
    
    from sensor where SlopeID=paramSlopeID and Type=paramSensorType;
END;

DROP PROCEDURE IF EXISTS `LoadSlopeList`;

CREATE PROCEDURE `LoadSlopeList`(IN paramAccount INTEGER UNSIGNED, IN paramServerID INTEGER UNSIGNED)
BEGIN
	select ID,Type,Name,Longitude,Latitude,State from slope where OwnerID=paramAccount;
END;

DROP PROCEDURE IF EXISTS `WebLogin`;

CREATE PROCEDURE `WebLogin`(IN paramAccount VARCHAR(64), IN paramPasword VARCHAR(64))
BEGIN
	select ID,count(*) from user where Account=paramAccount and Password=md5(paramPasword);
    select ID,IP,Port from server where ClientType=2;
END;

//
delimiter ; //

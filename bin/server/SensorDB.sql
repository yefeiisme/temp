CREATE DATABASE `SensorDB`
    CHARACTER SET 'latin1'
    COLLATE 'latin1_swedish_ci';

USE `SensorDB`;

DROP TABLE IF EXISTS `sensor`;

CREATE TABLE `sensor` (
  `ID` int(4) unsigned NOT NULL AUTO_INCREMENT,
  `Type` int(1) unsigned NOT NULL,
  `ReadValue` tinyblob,
  `Value1` double(8,3) NOT NULL,
  `Value2` double(8,3) NOT NULL,
  `Value3` double(8,3) NOT NULL,
  `SlopeID` int(2) unsigned NOT NULL,
  `OwnerID` int(4) unsigned NOT NULL,
  `DataTime` datetime NOT NULL,
  `Longitude` double(8,3) NOT NULL,
  `Latitude` double(8,3) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `sensor_type`;

CREATE TABLE `sensor_type` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `Value1` double(8,3) DEFAULT NULL,
  `Value2` double(8,3) DEFAULT NULL,
  `Value3` double(8,3) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `server`;

CREATE TABLE `server` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `IP` varchar(16) NOT NULL,
  `Port` int(2) unsigned DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `slope`;

CREATE TABLE `slope` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `Type` int(1) unsigned NOT NULL,
  `Name` varchar(64) DEFAULT NULL,
  `Longitude` double(8,3) NOT NULL,
  `Latitude` double(8,3) NOT NULL,
  `OwnerID` int(4) unsigned NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `user`;

CREATE TABLE `user` (
  `ID` int(4) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(64) DEFAULT NULL,
  `Addr` varchar(40) DEFAULT NULL,
  `TelNum` int(11) DEFAULT NULL,
  `GroupID` int(2) unsigned NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `user_group`;

CREATE TABLE `user_group` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(20) DEFAULT NULL,
  `ServerID` int(2) unsigned DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

# SQL Manager 2007 for MySQL 4.1.2.1
# ---------------------------------------
# Host     : localhost
# Port     : 3306
# Database : SensorDB


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

SET FOREIGN_KEY_CHECKS=0;

DROP DATABASE IF EXISTS `SensorDB`;

CREATE DATABASE `SensorDB`
    CHARACTER SET 'utf8'
    COLLATE 'utf8_general_ci';

USE `SensorDB`;

#
# Structure for the `sensor` table : 
#

CREATE TABLE `sensor` (
  `ID` int(4) unsigned NOT NULL AUTO_INCREMENT,
  `Type` int(1) unsigned NOT NULL,
  `Value1` varchar(32) DEFAULT NULL,
  `Value2` varchar(32) DEFAULT NULL,
  `Value3` varchar(32) DEFAULT NULL,
  `AverageValue1` double(15,3) NOT NULL DEFAULT '0.000',
  `AverageValue2` double(15,3) NOT NULL DEFAULT '0.000',
  `AverageValue3` double(15,3) NOT NULL DEFAULT '0.000',
  `OffsetValue1` double(15,3) NOT NULL DEFAULT '0.000',
  `OffsetValue2` double(15,3) NOT NULL DEFAULT '0.000',
  `OffsetValue3` double(15,3) NOT NULL DEFAULT '0.000',
  `AlarmState` int(11) NOT NULL DEFAULT '0',
  `SlopeID` int(2) unsigned NOT NULL,
  `DataTime` datetime NOT NULL,
  `Longitude` double(8,3) NOT NULL,
  `Latitude` double(8,3) NOT NULL,
  KEY `ID` (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=55 DEFAULT CHARSET=utf8;

#
# Structure for the `sensor_type` table : 
#

CREATE TABLE `sensor_type` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `Value1` double(8,3) DEFAULT NULL,
  `Value2` double(8,3) DEFAULT NULL,
  `Value3` double(8,3) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Structure for the `server` table : 
#

CREATE TABLE `server` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `IP` varchar(16) CHARACTER SET latin1 NOT NULL,
  `Port` int(2) unsigned DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Structure for the `slope` table : 
#

CREATE TABLE `slope` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `Type` int(1) unsigned NOT NULL,
  `Name` varchar(64) DEFAULT NULL,
  `Longitude` double(8,3) NOT NULL,
  `Latitude` double(8,3) NOT NULL,
  `OwnerID` int(4) unsigned NOT NULL,
  `State` int(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Structure for the `user` table : 
#

CREATE TABLE `user` (
  `ID` int(4) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(64) DEFAULT NULL,
  `Addr` varchar(40) DEFAULT NULL,
  `TelNum` int(11) DEFAULT NULL,
  `GroupID` int(2) unsigned NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Structure for the `user_group` table : 
#

CREATE TABLE `user_group` (
  `ID` int(2) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(20) DEFAULT NULL,
  `ServerID` int(2) unsigned DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
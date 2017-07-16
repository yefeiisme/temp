#ifndef __ROLE_DB_SQL_H_
#define __ROLE_DB_SQL_H_

//#define COMMIT_TRANSACTION_SQL			"commit"
//#define ROLL_BACK_TRANSACTION_SQL			"rollback"
#define CREATE_DATABASE_SQL					"create database if not exists %s CHARACTER SET 'utf8' COLLATE 'utf8_general_ci';"

#define ACCOUNT_TABLE_SQL					"create table if not exists useraccount "\
											"( "\
											"Account varchar(64) not null,"\
											"Password varchar(16) not null,"\
											"primary key (Account),"\
											"key Password(Password)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define PAYMENT_LIST_TABLE_SQL				"create table if not exists orderlist "\
											"( "\
											"OrderNum varchar(128) not null,"\
											"SerialNum varchar(128) not null,"\
											"ChannelID varchar(20) not null,"\
											"Account varchar(64) not null,"\
											"ServerID int(4) unsigned not null default '0',"\
											"PayTime datetime not null default '0000-00-00 00:00:00',"\
											"CompletePayTime datetime not null default '0000-00-00 00:00:00',"\
											"ReceiveTime datetime not null default '0000-00-00 00:00:00',"\
											"Money int(4) not null,"\
											"Result int(1) unsigned not null,"\
											"AwardIndex int(1) unsigned not null default 0,"\
											"primary key (OrderNum),"\
											"key SerialNum(SerialNum),"\
											"key Account(Account)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define ROLE_TABLE_SQL						"create table if not exists role "\
											"( "\
											"PlayerID int(4) unsigned not null auto_increment,"\
											"ServerID int(2) unsigned not null default '0',"\
											"ChannelID varchar(20) not null,"\
											"Account varchar(64) not null,"\
											"RoleName varchar(24) not null,"\
											"CreateTime datetime null default '0000-00-00 00:00:00',"\
											"OnlineTime datetime null default '0000-00-00 00:00:00',"\
											"OfflineTime datetime null default '0000-00-00 00:00:00',"\
											"LoginTimes int(4) unsigned not null default 0,"\
											"Robot int(1) unsigned not null default 0,"\
											"GameMaster int(1) unsigned not null default 0,"\
											"GuildID int(2) unsigned not null default 0,"\
											"GuildJob int(1) unsigned not null default 0,"\
											"Power int(4) not null default '0',"\
											"ImageID int(1) unsigned not null default '0',"\
											"HeroCount int(1) unsigned not null default '0',"\
											"HeroData blob default null,"\
											"Level int(1) unsigned not null default '1',"\
											"HeroExpPool int(4) signed not null default '0',"\
											"Diamond int(4) signed not null default '0',"\
											"RaffleTicket int(4) unsigned not null default 0,"\
											"TDMoney int(4) unsigned not null default 0,"\
											"Gold int(4) signed not null default '0',"\
											"PvPCoin int(4) signed not null default '0',"\
											"ExpCoin int(4) signed not null default '0',"\
											"LingShi int(4) signed not null default '0',"\
											"GodSouls int(4) signed not null default '0',"\
											"Energy int(2) signed not null default '0',"\
											"Exp int(4) signed not null default '0',"\
											"ItemCount int(1) unsigned not null default '0',"\
											"ItemData blob default null,"\
											"ResetTimes int(1) unsigned not null default '0',"\
											"MapData blob default null,"\
											"MissionData blob default null,"\
											"LotteryScore int(4) signed not null default '0',"\
											"SkillPoint int(2) unsigned not null default '0',"\
											"NextSkillPointTime int(4) signed not null default '0',"\
											"BuySkillPointTimes int(1) unsigned not null default '0',"\
											"HeroEmbattleInfo blob default null,"\
											"BuyEnergyTimes int(1) unsigned not null default '0',"\
											"BuyGoldTimes int(1) unsigned not null default '0',"\
											"VipLevel int(1) unsigned not null default '0',"\
											"VipGiftSaleState blob default null,"\
											"RechargeMoney int(4) unsigned not null default '0',"\
											"TotalRechargeDiamond int(4) unsigned not null default '0',"\
											"TotalRechargeTimes int(4) unsigned not null default '0',"\
											"FirstRecharge int(1) unsigned not null default 0,"\
											"LastResetTime int(4) unsigned not null default '0',"\
											"LastPerDayResetTime int(4) unsigned not null default '0',"\
											"WeekCardDays int(4) unsigned not null default '0',"\
											"MonthCardDays int(4) unsigned not null default '0',"\
											"PVPDefendPower int(4) unsigned not null default '0',"\
											"PVPWinTimes int(2) unsigned not null default '0',"\
											"Ranking int(2) unsigned not null default '0',"\
											"LastRanking int(2) unsigned not null default '0',"\
											"NextPVPTime int(4) not null default '0',"\
											"ChallengeTimes int(1) unsigned not null default '0',"\
											"DefendHeroCount int(1) unsigned not null default '0',"\
											"DefendLineup blob default null,"\
											"PVPDefendAddInfo blob default null,"\
											"AttackHeroCount int(1) unsigned not null default '0',"\
											"AttackLineup blob default null,"\
											"PvpAttackAddInfo blob default null,"\
											"AchievementData blob default null,"\
											"AchieveValue int(2) unsigned not null default '0',"\
											"AchieveAwardCount int(1) unsigned not null default '0',"\
											"GoldLotteryTime int(4) unsigned not null default '0',"\
											"DiamondLotteryTime int(4) unsigned not null default '0',"\
											"GoldLotteryTimes int(1) unsigned not null default '0',"\
											"DiamondLotteryTenTimes int(4) unsigned not null default '0',"\
											"DailyTask blob default null,"\
											"DailyTastNextResetTime int(4) unsigned not null default '0',"\
											"DailyActive int(4) unsigned not null default '0',"\
											"DailyAwardCount int(1) unsigned not null default '0',"\
											"InductStep int(1) unsigned not null default '0',"\
											"InductTask blob default null,"\
											"MagicNum int(1) unsigned not null default '0',"\
											"MagicWeapon blob default null,"\
											"StoreInfo blob default null,"\
											"Expedition blob default null,"\
											"Friend blob default null,"\
											"PVPRecord blob default null,"\
											"HeroPalace blob default null,"\
											"CaveInfo blob default null,"\
											"SignInTab blob default null,"\
											"FeatureState blob default null,"\
											"MagicMatrix blob default null,"\
											"TowerDefence blob default null,"\
											"TrailTomb blob default null,"\
											"ClimbingTower blob default null,"\
											"CTRanking int(4) unsigned not null default 0,"\
											"OnlinePVPPoint int(4) unsigned not null default '0',"\
											"OnlinePVPLastRanking int(4) unsigned not null default '0',"\
											"OnlinePVPFightCount int(4) unsigned not null default '0',"\
											"OnlinePVPWinCount int(4) unsigned not null default '0',"\
											"OnlinePVPLastTitle int(1) unsigned not null default '0',"\
											"OnlinePVP blob default null,"\
											"CustomData blob default null,"\
											"RechargeTimes blob default null,"\
											"MainLineId int(2) unsigned not null default '0',"\
											"MainLineDoTimes int(1) unsigned not null default '0',"\
											"MainLineState blob default null,"\
											"primary key (PlayerID),"\
											"key ChannelID (ChannelID),"\
											"key ServerID (ServerID),"\
											"key Account (Account),"\
											"unique key RoleName (RoleName)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define FREE_ROLE_NAME_TABLE_SQL			"create table if not exists freerolename "\
											"( "\
											"RoleName varchar(24) not null,"\
											"ServerID int(2) unsigned not null default 0,"\
											"primary key (RoleName),"\
											"unique key PlayerID (RoleName),"\
											"key ServerID (ServerID)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define ROLE_LOGIN_TABLE_SQL				"create table if not exists rolelogin "\
											"( "\
											"PlayerID int(4) unsigned not null default 0,"\
											"ServerID int(2) unsigned not null default 0,"\
											"LoginTime datetime not null default '0000-00-00 00:00:00',"\
											"CreateTime datetime not null default '0000-00-00 00:00:00'"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define GUILD_TABLE_SQL						"create table if not exists guild "\
											"( "\
											"GuildID int(2) unsigned not null auto_increment,"\
											"ServerID int(2) unsigned not null default 0,"\
											"OwnerID int(4) unsigned not null,"\
											"Name varchar(32) not null,"\
											"Announcement varchar(512) default '',"\
											"Level int(1) unsigned not null default 1,"\
											"MemberCount int(1) unsigned not null default 0,"\
											"MaxMemberCount int(1) unsigned not null,"\
											"GodPoint int(4) unsigned not null default 0,"\
											"JoinPoint int(4) unsigned not null,"\
											"JoinType int(1) unsigned not null,"\
											"primary key (GuildID),"\
											"unique key Name (Name)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define APPLY_JOIN_GUILD_TABLE_SQL			"create table if not exists applyjoinguild "\
											"( "\
											"PlayerID int(4) unsigned not null,"\
											"GuildID int(2) unsigned not null,"\
											"ApplyTime datetime null default '0000-00-00 00:00:00',"\
											"key PlayerID (PlayerID),"\
											"key ApplyTime (ApplyTime)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define FRIEND_LOG_TABLE_SQL				"create table if not exists friendlog "\
											"( "\
											"PlayerID int(4) unsigned not null,"\
											"FriendID int(4) unsigned not null,"\
											"Operation int(1) unsigned not null default 0,"\
											"ExpireTime datetime not null default '0000-00-00 00:00:00',"\
											"key PlayerID (PlayerID),"\
											"key FriendID (FriendID)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define FRIEND_RED_ENVELOPE_TABLE_SQL		"create table if not exists friendredenvelope "\
											"( "\
											"ID int(4) unsigned not null,"\
											"ServerID int(2) unsigned not null,"\
											"PlayerID int(4) unsigned not null,"\
											"PlayerName varchar(24) not null,"\
											"ImageID int(1) unsigned not null,"\
											"MinDiamond int(4) unsigned not null,"\
											"MaxDiamond int(4) unsigned not null,"\
											"ExpireTime datetime not null default '0000-00-00 00:00:00',"\
											"Data blob default null,"\
											"key ID (ID),"\
											"key ServerID (ServerID),"\
											"key PlayerID (PlayerID),"\
											"key ExpireTime (ExpireTime)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define FRIEND_WHISPER_TABLE_SQL			"create table if not exists friendwhisper "\
											"( "\
											"SenderID int(4) unsigned not null,"\
											"ReceiverID int(4) unsigned not null,"\
											"ExpireTime datetime not null default '0000-00-00 00:00:00',"\
											"Content varchar(256) not null,"\
											"key SenderID (SenderID),"\
											"key ReceiverID (ReceiverID),"\
											"key ExpireTime (ExpireTime)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define MAIL_TABLE_SQL						"create table if not exists mails "\
											"( "\
											"ID int(4) unsigned not null auto_increment,"\
											"ReceiverID int(4) unsigned not null,"\
											"PostTime datetime not null default '0000-00-00 00:00:00',"\
											"ExpireTime datetime not null default '0000-00-00 00:00:00',"\
											"Type int(1) unsigned not null default '0',"\
											"State int(1) unsigned not null default '0',"\
											"Pickup int(1) unsigned not null default '0',"\
											"Title varchar(32) not null,"\
											"Content varchar(1024) not null default \'\',"\
											"MailData tinyblob default null,"\
											"primary key (ID),"\
											"key ReceiverID (ReceiverID)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define GLOBAL_MAIL_TABLE_SQL				"create table if not exists globalmails "\
											"( "\
											"ID int(4) unsigned not null auto_increment,"\
											"PostTime datetime not null default '0000-00-00 00:00:00',"\
											"ExpireTime datetime not null default '0000-00-00 00:00:00',"\
											"Title varchar(32) not null,"\
											"Content varchar(1024) not null default \'\',"\
											"MailData tinyblob default null,"\
											"primary key (ID),"\
											"key PostTime (PostTime)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define GAME_SERVER_TABLE_SQL				"create table if not exists gameserver "\
											"( "\
											"ID int(4) unsigned not null,"\
											"Name varchar(32) not null,"\
											"IP int(4) unsigned not null,"\
											"Last5ClockResetTime int(4) unsigned not null default '0',"\
											"LastPerDayResetTime int(4) unsigned not null default '0',"\
											"LastPVPAwardTime int(4) unsigned not null default '0',"\
											"PlayerCount int(2) unsigned not null,"\
											"OnlinePVPRanking blob default null,"\
											"primary key (ID)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define GLOBAL_DATA_TABLE_SQL				"create table if not exists globaldata "\
											"( "\
											"RankingData blob default null"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define ACTIVITY_SERVER_TABLE_SQL			"create table if not exists activityserver "\
											"( "\
											"ID int(4) unsigned not null,"\
											"Name varchar(32) not null,"\
											"IP int(4) unsigned not null,"\
											"LastPVPAwardTime int(4) unsigned not null default '0',"\
											"primary key (ID)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define GAME_MASTER_TABLE_SQL				"create table if not exists gamemaster "\
											"( "\
											"Account varchar(32) not null,"\
											"Password varchar(32) not null,"\
											"primary key (Account)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define GM_LOG_TABLE_SQL					"create table if not exists gmlog "\
											"( "\
											"Account varchar(32) not null,"\
											"IP varchar(16) not null,"\
											"Operation varchar(32) not null,"\
											"OpTime datetime null default '0000-00-00 00:00:00',"\
											"Param1 int(4) unsigned not null,"\
											"Param2 int(4) unsigned not null,"\
											"Param3 int(4) unsigned not null,"\
											"String1 varchar(32),"\
											"String2 varchar(32),"\
											"String3 varchar(32),"\
											"key Account (Account)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define GIFT_CODE_TABLE_SQL					"create table if not exists giftpack "\
											"( "\
											"GiftID int(4) unsigned not null,"\
											"GiftCode varchar(32) not null,"\
											"StartTime datetime not null,"\
											"ExpiredTime datetime not null,"\
											"Account varchar(32) default '',"\
											"Gift tinyblob default null,"\
											"primary key (GiftCode),"\
											"key GiftID (GiftID)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define EXCHANGE_GIFT_CODE_TABLE_SQL		"create table if not exists exchangedgiftcode "\
											"( "\
											"GiftID int(4) unsigned not null,"\
											"Account varchar(32) not null,"\
											"ExchangeTime datetime not null,"\
											"key GiftID (GiftID),"\
											"key Account (Account)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define CLIMB_TOWER_TABLE_SQL				"create table if not exists climbtower "\
											"( "\
											"MissionID int(2) unsigned not null,"\
											"ServerID int(2) unsigned not null,"\
											"CompleteCount int(4) unsigned not null default 0,"\
											"key MissionID (MissionID),"\
											"key ServerID (ServerID)"\
											")ENGINE=MyISAM DEFAULT CHARSET=utf8;"

#define DROP_PROCEDURE_CREATE_ROLE_SQL		"DROP PROCEDURE IF EXISTS CreateRole"
#define DROP_PROCEDURE_CREATE_FREE_NAME		"DROP PROCEDURE IF EXISTS CreateFreeName"
#define DROP_PROCEDURE_CREATE_MAIL			"DROP PROCEDURE IF EXISTS CreateMail"
#define DROP_PROCEDURE_EXCHANGE_GIFT_CODE	"DROP PROCEDURE IF EXISTS ExchangeGiftPack\n"
#define DROP_PROCEDURE_QUERY_COMMON_REMAIN	"DROP PROCEDURE IF EXISTS QueryCommonRemain\n"
#define DROP_PROCEDURE_ROLE_RENAME			"DROP PROCEDURE IF EXISTS RoleRename\n"
#define DROP_PROCEDURE_CREATE_GUILD			"DROP PROCEDURE IF EXISTS CreateGuild\n"
#define DROP_PROCEDURE_APPLY_JOIN_GUILD		"DROP PROCEDURE IF EXISTS ApplyJoinGuild\n"
#define DROP_PROCEDURE_REFUSE_JOIN_GUILD	"DROP PROCEDURE IF EXISTS RefuseJoinGuild\n"
#define DROP_PROCEDURE_APPROVE_JOIN_GUILD	"DROP PROCEDURE IF EXISTS ApproveJoinGuild\n"
#define DROP_PROCEDURE_PLAYER_JOIN_GUILD	"DROP PROCEDURE IF EXISTS PlayerJoinGuild\n"
#define DROP_PROCEDURE_PLAYER_CLIMB_TOWER	"DROP PROCEDURE IF EXISTS PlayerClimbTower\n"
#define DROP_PROCEDURE_LOAD_FRIEND_LOG		"DROP PROCEDURE IF EXISTS LoadFriendLog\n"
#define DROP_PROCEDURE_UPDATE_FRIEND_DATA	"DROP PROCEDURE IF EXISTS UpdateFriendData\n"
#define DROP_PROCEDURE_ADD_FRIEND_LOG		"DROP PROCEDURE IF EXISTS AddFriendLog\n"
#define DROP_PROCEDURE_DELETE_FRIEND_LOG	"DROP PROCEDURE IF EXISTS DeleteFriendLog\n"
#define DROP_PROCEDURE_CREATE_RED_ENVELOPE	"DROP PROCEDURE IF EXISTS CreateRedEnvelope\n"
#define DROP_PROCEDURE_UPDATE_RED_ENVELOPE	"DROP PROCEDURE IF EXISTS UpdateRedEnvelope\n"
#define DROP_PROCEDURE_DELETE_RED_ENVELOPE	"DROP PROCEDURE IF EXISTS DeleteRedEnvelope\n"
#define DROP_PROCEDURE_ADD_FRIEND_WHISPER	"DROP PROCEDURE IF EXISTS AddFriendWhisper\n"
#define DROP_PROCEDURE_LOAD_FRIEND_WHISPER	"DROP PROCEDURE IF EXISTS LoadFriendWhisper\n"

#define PROCEDURE_CREATE_FREE_NAME			"CREATE PROCEDURE CreateFreeName\n"\
											"(\n"\
											"in paramServerIDString varchar(4),\n"\
											"in paramServerID int(2) unsigned,\n"\
											"in paramCount int(4) unsigned\n"\
											")\n"\
											"begin\n"\
											"DECLARE _index int default 0;\n"\
											"DECLARE _value varchar(32) default \'\';\n"\
											"while(_index < paramCount) do\n"\
											"set _value = CONCAT(now(),_index);\n"\
											"insert into freerolename (RoleName,ServerID) values(CONCAT(paramServerIDString,SUBSTRING(md5(_value),9,10)),paramServerID);\n"\
											"set _index = _index + 1;\n"\
											"end while;\n"\
											"end"

#define PROCEDURE_CREATE_ROLE_SQL			"CREATE PROCEDURE CreateRole\n"\
											"(\n"\
											"in paramChannelID varchar(20),\n"\
											"in paramAccount varchar(64),\n"\
											"in paramServerID int(2) unsigned\n"\
											")\n"\
											"begin\n"\
											"DECLARE _roleCount int default 0;\n"\
											"DECLARE _roleNameCount int default 0;\n"\
											"DECLARE _roleName varchar(32) default \'\';\n"\
											"DECLARE _timeNow datetime;\n"\
											"set @result = 0;\n"\
											"set _timeNow = now();\n"\
											"select count(*) into _roleCount from role where Account=paramAccount and ServerID=paramServerID;\n"\
											"if _roleCount = 1 then\n"\
											"	set @result = 1;\n"\
											"else\n"\
											"	select count(*),RoleName into _roleNameCount,_roleName from freerolename where ServerID=paramServerID limit 0,1;\n"\
											"	insert into role (ChannelID,Account,RoleName,ServerID,CreateTime,OnlineTime,OfflineTime) values(paramChannelID,paramAccount,_roleName,paramServerID,_timeNow,_timeNow,_timeNow);\n"\
											"	if ROW_COUNT() <> 1 then\n"\
											"		set @result = 2;\n"\
											"	end if;\n"\
											"	delete from freerolename where ServerID=paramServerID and RoleName=_roleName;\n"\
											"end if;\n"\
											"if (@result <> 0) then\n"\
											"	select @result;\n"\
											"else\n"\
											"select \n"\
											"@result,\n"\
											"_roleName,\n"\
											"PlayerID,\n"\
											"UNIX_TIMESTAMP(CreateTime),\n"\
											"UNIX_TIMESTAMP(OnlineTime),\n"\
											"UNIX_TIMESTAMP(OfflineTime),\n"\
											"LoginTimes,\n"\
											"Robot,\n"\
											"GameMaster,\n"\
											"GuildID,\n"\
											"GuildJob,\n"\
											"Power,\n"\
											"ImageID,\n"\
											"HeroCount,\n"\
											"HeroData,\n"\
											"Level,\n"\
											"HeroExpPool,\n"\
											"Diamond,\n"\
											"RaffleTicket,\n"\
											"TDMoney,\n"\
											"Gold,\n"\
											"PvPCoin,\n"\
											"ExpCoin,\n"\
											"LingShi,\n"\
											"GodSouls,\n"\
											"Energy,\n"\
											"Exp,\n"\
											"ItemCount,\n"\
											"ItemData,\n"\
											"ResetTimes,\n"\
											"MapData,\n"\
											"MissionData,\n"\
											"LotteryScore,\n"\
											"SkillPoint,\n"\
											"NextSkillPointTime,\n"\
											"BuySkillPointTimes,\n"\
											"HeroEmbattleInfo,\n"\
											"BuyEnergyTimes,\n"\
											"BuyGoldTimes,\n"\
											"VipLevel,\n"\
											"VipGiftSaleState,\n"\
											"RechargeMoney,\n"\
											"TotalRechargeDiamond,\n"\
											"TotalRechargeTimes,\n"\
											"FirstRecharge,\n"\
											"LastResetTime,\n"\
											"LastPerDayResetTime,\n"\
											"WeekCardDays,\n"\
											"MonthCardDays,\n"\
											"PVPDefendPower,\n"\
											"PVPWinTimes,\n"\
											"Ranking,\n"\
											"LastRanking,\n"\
											"NextPVPTime,\n"\
											"ChallengeTimes,\n"\
											"DefendHeroCount,\n"\
											"DefendLineup,\n"\
											"PVPDefendAddInfo,\n"\
											"AttackHeroCount,\n"\
											"AttackLineup,\n"\
											"PvpAttackAddInfo,\n"\
											"AchievementData,\n"\
											"AchieveValue,\n"\
											"AchieveAwardCount,\n"\
											"GoldLotteryTime,\n"\
											"DiamondLotteryTime,\n"\
											"GoldLotteryTimes,\n"\
											"DiamondLotteryTenTimes,\n"\
											"DailyTask,\n"\
											"DailyTastNextResetTime,\n"\
											"DailyActive,\n"\
											"DailyAwardCount,\n"\
											"InductStep,\n"\
											"InductTask,\n"\
											"MagicNum,\n"\
											"MagicWeapon,\n"\
											"StoreInfo,\n"\
											"Expedition,\n"\
											"Friend,\n"\
											"PVPRecord,\n"\
											"HeroPalace,\n"\
											"CaveInfo,\n"\
											"SignInTab,\n"\
											"FeatureState,\n"\
											"MagicMatrix,\n"\
											"TowerDefence,\n"\
											"TrailTomb,\n"\
											"ClimbingTower,\n"\
											"CTRanking,\n"\
											"OnlinePVPPoint,\n"\
											"OnlinePVPLastRanking,\n"\
											"OnlinePVPFightCount,\n"\
											"OnlinePVPWinCount,\n"\
											"OnlinePVPLastTitle,\n"\
											"OnlinePVP,\n"\
											"CustomData,\n"\
											"RechargeTimes,\n"\
											"MainLineId,\n"\
											"MainLineDoTimes,\n"\
											"MainLineState \n"\
											"from role where Account=paramAccount and ServerID=paramServerID;\n"\
											"end if;\n"\
											"end"

#define PROCEDURE_LOAD_ALL_ROLE_SQL			"select \n"\
											"PlayerID,\n"\
											"ServerID,\n"\
											"Account,\n"\
											"RoleName,\n"\
											"UNIX_TIMESTAMP(CreateTime),\n"\
											"UNIX_TIMESTAMP(OnlineTime),\n"\
											"UNIX_TIMESTAMP(OfflineTime),\n"\
											"LoginTimes,\n"\
											"Robot,\n"\
											"GameMaster,\n"\
											"GuildID,\n"\
											"GuildJob,\n"\
											"Power,\n"\
											"ImageID,\n"\
											"HeroCount,\n"\
											"HeroData,\n"\
											"Level,\n"\
											"HeroExpPool,\n"\
											"Diamond,\n"\
											"RaffleTicket,\n"\
											"TDMoney,\n"\
											"Gold,\n"\
											"PvPCoin,\n"\
											"ExpCoin,\n"\
											"LingShi,\n"\
											"GodSouls,\n"\
											"Energy,\n"\
											"Exp,\n"\
											"ItemCount,\n"\
											"ItemData,\n"\
											"ResetTimes,"\
											"MapData,\n"\
											"MissionData,\n"\
											"LotteryScore,\n"\
											"SkillPoint,\n"\
											"NextSkillPointTime,\n"\
											"BuySkillPointTimes,\n"\
											"HeroEmbattleInfo,"\
											"BuyEnergyTimes,\n"\
											"BuyGoldTimes,\n"\
											"VipLevel,\n"\
											"VipGiftSaleState,\n"\
											"RechargeMoney,\n"\
											"TotalRechargeDiamond,\n"\
											"TotalRechargeTimes,\n"\
											"FirstRecharge,\n"\
											"LastResetTime,\n"\
											"LastPerDayResetTime,\n"\
											"WeekCardDays,\n"\
											"MonthCardDays,\n"\
											"PVPDefendPower,\n"\
											"PVPWinTimes,\n"\
											"Ranking,\n"\
											"LastRanking,\n"\
											"NextPVPTime,\n"\
											"ChallengeTimes,\n"\
											"DefendHeroCount,\n"\
											"DefendLineup,\n"\
											"PVPDefendAddInfo,\n"\
											"AttackHeroCount,\n"\
											"AttackLineup,\n"\
											"PvpAttackAddInfo,\n"\
											"AchievementData,\n"\
											"AchieveValue,\n"\
											"AchieveAwardCount,\n"\
											"GoldLotteryTime,\n"\
											"DiamondLotteryTime,\n"\
											"GoldLotteryTimes,\n"\
											"DiamondLotteryTenTimes,\n"\
											"DailyTask,\n"\
											"DailyTastNextResetTime,\n"\
											"DailyActive,\n"\
											"DailyAwardCount,\n"\
											"InductStep,\n"\
											"InductTask,\n"\
											"MagicNum,\n"\
											"MagicWeapon,\n"\
											"StoreInfo,\n"\
											"Expedition,\n"\
											"Friend,\n"\
											"PVPRecord,\n"\
											"HeroPalace,\n"\
											"CaveInfo,\n"\
											"SignInTab,\n"\
											"FeatureState,\n"\
											"MagicMatrix,\n"\
											"TowerDefence,\n"\
											"TrailTomb,\n"\
											"ClimbingTower,\n"\
											"CTRanking,\n"\
											"OnlinePVPPoint,\n"\
											"OnlinePVPLastRanking,\n"\
											"OnlinePVPFightCount,\n"\
											"OnlinePVPWinCount,\n"\
											"OnlinePVPLastTitle,\n"\
											"OnlinePVP,\n"\
											"CustomData,\n"\
											"RechargeTimes,\n"\
											"MainLineId,\n"\
											"MainLineDoTimes,\n"\
											"MainLineState \n"\
											"from role"

#define PROCEDURE_CREATE_MAIL				"CREATE PROCEDURE CreateMail\n"\
											"(\n"\
											"in paramPostTime datetime,\n"\
											"in paramExpireTime datetime,\n"\
											"in paramType int,\n"\
											"in paramReceiverID int,\n"\
											"in paramTitle varchar(32),\n"\
											"in paramContent varchar(1024),\n"\
											"in paramMailData tinyblob\n"\
											")\n"\
											"begin\n"\
											"declare exit handler for sqlexception\n"\
											"begin\n"\
											"set @result = 0;\n"\
											"select @result;\n"\
											"end;\n"\
											"insert into mails (ReceiverID,PostTime,ExpireTime,Type,Title,Content,MailData) values(paramReceiverID,paramPostTime,paramExpireTime,paramType,paramTitle,paramContent,paramMailData);\n"\
											"set @result = 1;\n"\
											"select @result;\n"\
											"end"

#define PROCEDURE_EXCHANGE_GIFT_CODE		"CREATE PROCEDURE ExchangeGiftPack\n"\
											"(\n"\
											"in paramGiftCode varchar(32),\n"\
											"in paramAccount varchar(32)\n"\
											")\n"\
											"begin\n"\
											"DECLARE _value int default 0;\n"\
											"DECLARE _GiftCodeCount int default 0;\n"\
											"DECLARE _giftID int default 0;\n"\
											"DECLARE _exchangedAccount varchar(32) default '';\n"\
											"DECLARE _startTime datetime default '0000-00-00 00:00:00';\n"\
											"DECLARE _expiredTime datetime default '0000-00-00 00:00:00';\n"\
											"DECLARE _giftItem tinyblob default null;\n"\
											"set @result = 0;\n"\
											"select count(GiftID),GiftID,StartTime,ExpiredTime,Account,Gift into _GiftCodeCount,_giftID,_startTime,_expiredTime,_exchangedAccount,_giftItem from giftpack where GiftCode = paramGiftCode;\n"\
											"if (_GiftCodeCount < 1) then\n"\
											"	set @result = 1;\n"\
											"elseif (now() < _startTime) then\n"\
											"	set @result = 2;\n"\
											"elseif (now() > _expiredTime) then\n"\
											"	set @result = 3;\n"\
											"elseif (_exchangedAccount <> '') then\n"\
											"	set @result = 4;\n"\
											"else\n"\
											"	select count(*) from exchangedgiftcode where GiftID = _giftID and Account = paramAccount into _value;\n"\
											"	if (_value <> 0) then\n"\
											"		set @result = 5;\n"\
											"	else\n"\
											"		insert into exchangedgiftcode(GiftID,Account,ExchangeTime) values(_giftID,paramAccount,now());\n"\
											"		update giftpack set Account = paramAccount where GiftCode = paramGiftCode;\n"\
											"	end if;\n"\
											"end if;\n"\
											"if (@result = 0) then\n"\
											"	select @result,_giftItem;\n"\
											"else\n"\
											"	select @result,null;\n"\
											"end if;\n"\
											"end"

#define PROCEDURE_QUERY_COMMON_REMAIN		"CREATE PROCEDURE QueryCommonRemain\n"\
											"(\n"\
											"in paramDate int unsigned\n"\
											")\n"\
											"begin\n"\
											"DECLARE _Dau int default 0;\n"\
											"DECLARE _Dnu int default 0;\n"\
											"DECLARE _SecondRemain int default 0;\n"\
											"DECLARE _ThirdRemain int default 0;\n"\
											"DECLARE _SeventhRemain int default 0;\n"\
											"DECLARE _ThirtiethRemain int default 0;\n"\
											"DECLARE _QueryTime datetime;\n"\
											"set _QueryTime	= from_unixtime(paramDate);\n"\
											"select count(DISTINCT PlayerID) into _Dau from rolelogin where TO_DAYS(LoginTime) = TO_DAYS(_QueryTime);\n"\
											"select count(*) into _Dnu from role where TO_DAYS(CreateTime) = TO_DAYS(_QueryTime);\n"\
											"select count(DISTINCT PlayerID) into _SecondRemain from rolelogin where TO_DAYS(CreateTime)=TO_DAYS(_QueryTime) and DATEDIFF(LoginTime,CreateTime)=1;\n"\
											"select count(DISTINCT PlayerID) into _ThirdRemain from rolelogin where TO_DAYS(CreateTime)=TO_DAYS(_QueryTime) and DATEDIFF(LoginTime,CreateTime)=2;\n"\
											"select count(DISTINCT PlayerID) into _SeventhRemain from rolelogin where TO_DAYS(CreateTime)=TO_DAYS(_QueryTime) and DATEDIFF(LoginTime,CreateTime)=7;\n"\
											"select count(DISTINCT PlayerID) into _ThirtiethRemain from rolelogin where TO_DAYS(CreateTime)=TO_DAYS(_QueryTime) and DATEDIFF(LoginTime,CreateTime)=29;\n"\
											"select _Dau,_Dnu,_SecondRemain,_ThirdRemain,_SeventhRemain,_ThirtiethRemain;\n"\
											"end"

#define PROCEDURE_ROLE_RENAME				"CREATE PROCEDURE RoleRename\n"\
											"(\n"\
											"in paramRoleName varchar(32),\n"\
											"in paramPlayerID int(4) unsigned\n"\
											")\n"\
											"begin\n"\
											"DECLARE _OldName varchar(32) default '';\n"\
											"DECLARE _ServerID int(2) unsigned default 0;\n"\
											"set @result=0;\n"\
											"if exists (select PlayerID from role where RoleName=paramRoleName) then\n"\
											"	set @result=1;\n"\
											"elseif not exists (select PlayerID from role where PlayerID=paramPlayerID) then\n"\
											"	set @result=3;\n"\
											"else\n"\
											"	select RoleName,ServerID into _OldName,_ServerID from role where PlayerID=paramPlayerID;\n"\
											"	update role set RoleName=paramRoleName where PlayerID=paramPlayerID;\n"\
											"	if ROW_COUNT() <> 1 then\n"\
											"		set @result=2;\n"\
											"	else\n"\
											"		delete from freerolename where RoleName=paramRoleName;\n"\
											"		insert into freerolename(RoleName,ServerID) values(_OldName,_ServerID);\n"\
											"	end if;\n"\
											"end if;\n"\
											"if (@result = 0) then\n"\
											"	select @result,paramRoleName;\n"\
											"else\n"\
											"	select @result,'';\n"\
											"end if;\n"\
											"end"

#define PROCEDURE_CREATE_GUILD				"CREATE PROCEDURE CreateGuild\n"\
											"(\n"\
											"in paramOwnerID int(4) unsigned,\n"\
											"in paramGuildName varchar(32),\n"\
											"in paramAnnouncement varchar(512),\n"\
											"in paramLevel int(1) unsigned,\n"\
											"in paramMaxMemberCount int(1) unsigned,\n"\
											"in paramJoinPoint int(4) unsigned,\n"\
											"in paramJoinType int(1) unsigned\n"\
											")\n"\
											"begin\n"\
											"DECLARE _GuildID int(2) unsigned default 0;\n"\
											"set @result=0;\n"\
											"if exists (select GuildID from guild where Name=paramGuildName) then\n"\
											"	set @result=1;\n"\
											"else\n"\
											"	select GuildID into _GuildID from role where PlayerID=paramOwnerID;\n"\
											"	if (_GuildID <> 0) then\n"\
											"		set @result=2;\n"\
											"	else\n"\
											"		insert into guild(OwnerID,Name,Announcement,Level,MemberCount,MaxMemberCount,JoinPoint,JoinType) values(paramOwnerID,paramGuildName,paramAnnouncement,paramLevel,1,paramMaxMemberCount,paramJoinPoint,paramJoinType);\n"\
											"		if ROW_COUNT() <> 1 then\n"\
											"			set @result=3;\n"\
											"		end if;\n"\
											"		select GuildID into _GuildID from guild where Name=paramGuildName;\n"\
											"		update role set GuildID=_GuildID where PlayerID=paramOwnerID;\n"\
											"	end if;\n"\
											"end if;\n"\
											"if (@result = 0) then\n"\
											"	select @result,_GuildID;\n"\
											"else\n"\
											"	select @result,0;\n"\
											"end if;\n"\
											"end"

#define PROCEDURE_APPLY_JOIN_GUILD			"CREATE PROCEDURE ApplyJoinGuild\n"\
											"(\n"\
											"in paramPlayerID int(4) unsigned,\n"\
											"in paramGuildID int(2) unsigned,\n"\
											"in paramMaxApplyCount int(2) unsigned\n"\
											")\n"\
											"begin\n"\
											"DECLARE _ApplyCount int(2) unsigned default 0;\n"\
											"DECLARE _GuildID int(2) unsigned default 0;\n"\
											"DECLARE _MemberCount int(2) unsigned default 0;\n"\
											"DECLARE _MaxMemberCount int(2) unsigned default 0;\n"\
											"set @result=0;\n"\
											"if exists (select PlayerID from applyjoinguild where PlayerID=paramPlayerID and GuildID=paramGuildID) then\n"\
											"	set @result=1;\n"\
											"else\n"\
											"	select GuildID into _GuildID from role where PlayerID=paramPlayerID;\n"\
											"	if (_GuildID <> 0) then\n"\
											"		set @result=2;\n"\
											"	else\n"\
											"		select count(PlayerID) into _ApplyCount from applyjoinguild where GuildID=paramGuildID;\n"\
											"		if _ApplyCount() >= paramMaxApplyCount then\n"\
											"			set @result=3;\n"\
											"		else\n"\
											"			select MemberCount,MaxMemberCount into _MemberCount,_MaxMemberCount from guild where GuildID=paramGuildID;\n"\
											"			if (_MemberCount >= _MaxMemberCount) then\n"\
											"				set @result=4;\n"\
											"			else\n"\
											"				insert into applyjoinguild(PlayerID,GuildID,ApplyTime) values(paramPlayerID,paramGuildID,now());\n"\
											"				if ROW_COUNT() <> 1 then\n"\
											"					set @result=5;\n"\
											"				end if;\n"\
											"			end if;\n"\
											"		end if;\n"\
											"	end if;\n"\
											"end if;\n"\
											"select @result;\n"\
											"end"

#define PROCEDURE_REFUSE_JOIN_GUILD			"CREATE PROCEDURE RefuseJoinGuild\n"\
											"(\n"\
											"in paramPlayerID int(4) unsigned,\n"\
											"in paramGuildID int(2) unsigned\n"\
											")\n"\
											"begin\n"\
											"set @result=0;\n"\
											"delete from applyjoinguild where PlayerID=paramPlayerID and GuildID=paramGuildID;\n"\
											"if ROW_COUNT() <> 1 then\n"\
											"	set @result=1;\n"\
											"end if;\n"\
											"select @result;\n"\
											"end"

#define PROCEDURE_APPROVE_JOIN_GUILD		"CREATE PROCEDURE ApproveJoinGuild\n"\
											"(\n"\
											"in paramPlayerID int(4) unsigned,\n"\
											"in paramGuildID int(2) unsigned\n"\
											")\n"\
											"begin\n"\
											"DECLARE _GuildID int(2) unsigned default 0;\n"\
											"DECLARE _MemberCount int(2) unsigned default 0;\n"\
											"DECLARE _MaxMemberCount int(2) unsigned default 0;\n"\
											"set @result=0;\n"\
											"if not exists (select PlayerID from applyjoinguild where PlayerID=paramPlayerID and GuildID=paramGuildID) then\n"\
											"	set @result=1;\n"\
											"else\n"\
											"	select GuildID into _GuildID from role where PlayerID=paramPlayerID;\n"\
											"	if (_GuildID <> 0) then\n"\
											"		set @result=2;\n"\
											"	else\n"\
											"		select MemberCount,MaxMemberCount into _MemberCount,_MaxMemberCount from guild where GuildID=paramGuildID;\n"\
											"		if (_MemberCount >= _MaxMemberCount) then\n"\
											"			set @result=3;\n"\
											"		else\n"\
											"			delete from applyjoinguild where PlayerID=paramPlayerID;\n"\
											"			update role set GuildID=paramGuildID where PlayerID=paramPlayerID;\n"\
											"			update guild set MemberCount=_MemberCount+1 where GuildID=paramGuildID;\n"\
											"		end if;\n"\
											"	end if;\n"\
											"end if;\n"\
											"select @result;\n"\
											"end"

#define PROCEDURE_PLAYER_JOIN_GUILD			"CREATE PROCEDURE PlayerJoinGuild\n"\
											"(\n"\
											"in paramPlayerID int(4) unsigned,\n"\
											"in paramGuildID int(2) unsigned\n"\
											")\n"\
											"begin\n"\
											"DECLARE _GuildID int(2) unsigned default 0;\n"\
											"DECLARE _MemberCount int(2) unsigned default 0;\n"\
											"DECLARE _MaxMemberCount int(2) unsigned default 0;\n"\
											"set @result=0;\n"\
											"select GuildID into _GuildID from role where PlayerID=paramPlayerID;\n"\
											"if (_GuildID <> 0) then\n"\
											"	set @result=1;\n"\
											"else\n"\
											"	select MemberCount,MaxMemberCount into _MemberCount,_MaxMemberCount from guild where GuildID=paramGuildID;\n"\
											"	if (_MemberCount >= _MaxMemberCount) then\n"\
											"		set @result=2;\n"\
											"	else\n"\
											"		delete from applyjoinguild where PlayerID=paramPlayerID;\n"\
											"		update role set GuildID=paramGuildID where PlayerID=paramPlayerID;\n"\
											"		update guild set MemberCount=_MemberCount+1 where GuildID=paramGuildID;\n"\
											"	end if;\n"\
											"end if;\n"\
											"select @result;\n"\
											"end"

#define PROCEDURE_PLAYER_CLIMB_TOWER		"CREATE PROCEDURE PlayerClimbTower\n"\
											"(\n"\
											"in paramPlayerID int(4) unsigned,\n"\
											"in paramServerID int(2) unsigned,\n"\
											"in paramMissionID int(2) unsigned,\n"\
											"in paramClimbCount int(4) unsigned\n"\
											")\n"\
											"begin\n"\
											"update climbtower set CompleteCount=paramClimbCount where MissionID=paramMissionID and ServerID=paramServerID;\n"\
											"update role set CTRanking=paramClimbCount where PlayerID=paramPlayerID;\n"\
											"end"

#define PROCEDURE_LOAD_FRIEND_LOG			"CREATE PROCEDURE LoadFriendLog\n"\
											"(\n"\
											"in paramPlayerID int(4) unsigned\n"\
											")\n"\
											"begin\n"\
											"select PlayerID,Operation from friendlog where FriendID=paramPlayerID;\n"\
											"end"

#define PROCEDURE_UPDATE_FRIEND_DATA		"CREATE PROCEDURE UpdateFriendData\n"\
											"(\n"\
											"in paramPlayerID int(4) unsigned,\n"\
											"in paramFriendData blob\n"\
											")\n"\
											"begin\n"\
											"update role set Friend=paramFriendData where PlayerID=paramPlayerID;\n"\
											"end"

#define PROCEDURE_ADD_FRIEND_LOG			"CREATE PROCEDURE AddFriendLog\n"\
											"(\n"\
											"in paramPlayerID int(4) unsigned,\n"\
											"in paramFriendID int(4) unsigned,\n"\
											"in paramOperation int(1) unsigned,\n"\
											"in paramExpireTime int(4)\n"\
											")\n"\
											"begin\n"\
											"insert into friendlog(PlayerID,FriendID,Operation,ExpireTime) values(paramPlayerID,paramFriendID,paramOperation,from_unixtime(UNIX_TIMESTAMP()+paramExpireTime));\n"\
											"end"

#define PROCEDURE_DELETE_FRIEND_LOG			"CREATE PROCEDURE DeleteFriendLog()\n"\
											"begin\n"\
											"delete from friendlog where now() > ExpireTime;\n"\
											"end"

#define PROCEDURE_CREATE_RED_ENVELOPE		"CREATE PROCEDURE CreateRedEnvelope\n"\
											"(\n"\
											"in paramID int(4) unsigned,\n"\
											"in paramServerID int(2) unsigned,\n"\
											"in paramPlayerID int(4) unsigned,\n"\
											"in paramPlayerName varchar(24),"\
											"in paramImageID int(1) unsigned,"\
											"in paramMinDiamond int(4) unsigned,\n"\
											"in paramMaxDiamond int(4) unsigned,\n"\
											"in paramExpireTime int(4) unsigned\n"\
											")\n"\
											"begin\n"\
											"insert into friendredenvelope(ID,ServerID,PlayerID,PlayerName,ImageID,MinDiamond,MaxDiamond,ExpireTime) values(paramID,paramServerID,paramPlayerID,paramPlayerName,paramImageID,paramMinDiamond,paramMaxDiamond,from_unixtime(UNIX_TIMESTAMP()+paramExpireTime));\n"\
											"end"

#define PROCEDURE_UPDATE_RED_ENVELOPE		"CREATE PROCEDURE UpdateRedEnvelope\n"\
											"(\n"\
											"in paramID int(4) unsigned,\n"\
											"in paramServerID int(2) unsigned,\n"\
											"in paramData blob\n"\
											")\n"\
											"begin\n"\
											"update friendredenvelope set Data=paramData where ID=paramID and ServerID=paramServerID;\n"\
											"end"

#define PROCEDURE_DELETE_RED_ENVELOPE		"CREATE PROCEDURE DeleteRedEnvelope\n"\
											"(\n"\
											"in paramID int(4) unsigned,\n"\
											"in paramServerID int(2) unsigned\n"\
											")\n"\
											"begin\n"\
											"delete from friendredenvelope where ID=paramID and ServerID=paramServerID;\n"\
											"end"

#define PROCEDURE_ADD_FRIEND_WHISPER		"CREATE PROCEDURE AddFriendWhisper\n"\
											"(\n"\
											"in paramSenderID int(4) unsigned,\n"\
											"in paramReceiverID int(4) unsigned,\n"\
											"in paramExpireTime int(4) unsigned,\n"\
											"in paramContent varchar(256)"\
											")\n"\
											"begin\n"\
											"insert into friendwhisper(SenderID,ReceiverID,ExpireTime,Content) values(paramSenderID,paramReceiverID,from_unixtime(UNIX_TIMESTAMP()+paramExpireTime),paramContent);\n"\
											"end"

#define PROCEDURE_LOAD_FRIEND_WHISPER		"CREATE PROCEDURE LoadFriendWhisper\n"\
											"(\n"\
											"in paramSenderID int(4) unsigned,\n"\
											"in paramReceiverID int(4) unsigned\n"\
											")\n"\
											"begin\n"\
											"select SenderID,Content from friendwhisper where (SenderID=paramSenderID and ReceiverID=paramReceiverID) or (SenderID=paramReceiverID and ReceiverID=paramSenderID) order by ExpireTime desc limit 20;\n"\
											"end"

#define PROCEDURE_LOAD_MAIL_LIST			"select ID,ReceiverID,Type,State,Pickup,UNIX_TIMESTAMP(PostTime),UNIX_TIMESTAMP(ExpireTime),Title,Content,MailData from mails where ReceiverID=%u"
#define PROCEDURE_LOAD_NEW_MAIL				"select ID,ReceiverID,Type,State,Pickup,UNIX_TIMESTAMP(PostTime),UNIX_TIMESTAMP(ExpireTime),Title,Content,MailData from mails where ReceiverID=%u and PostTime=from_unixtime(%d)"
#define PROCEDURE_LOAD_ALL_GLOBAL_MAIL		"select ID,UNIX_TIMESTAMP(PostTime),Title,Content,MailData,UNIX_TIMESTAMP(ExpireTime) from globalmails"
#define PROCEDURE_LOAD_NEW_GLOBAL_MAIL		"select ID,UNIX_TIMESTAMP(PostTime),Title,Content,MailData,UNIX_TIMESTAMP(ExpireTime) from globalmails where PostTime=from_unixtime(%d)"
#define PROCEDURE_LOAD_ALL_GAME_SERVER		"select ID,Name,IP,Last5ClockResetTime,LastPerDayResetTime,LastPVPAwardTime,PlayerCount from gameserver"
#define PROCEDURE_LOAD_GAME_SERVER			"select ID,Name,IP,Last5ClockResetTime,LastPerDayResetTime,LastPVPAwardTime,PlayerCount from gameserver where ID=%hu"
#define PROCEDURE_LOAD_ALL_ACTIVITY			"select ID,Name,IP,LastPVPAwardTime from activityserver"
#define PROCEDURE_LOAD_ACTIVITY				"select ID,Name,IP,LastPVPAwardTime from activityserver where ID=%hhu"
#define PROCEDURE_LOAD_ALL_GAME_MASTER		"select Account,Password from gamemaster"
#define PROCEDURE_LOAD_GAME_MASTER			"select Account,Password from gamemaster where Account=%s"
#define PROCEDURE_DELETE_PLAYER_MAIL		"delete from mails where ID=%u"
#define PROCEDURE_DELETE_PLAYER_EXPIRE_MAIL	"delete from mails where now() > ExpireTime"
#define PROCEDURE_DELETE_GLOBAL_EXPIRE_MAIL	"delete from globalmails where now() > ExpireTime"
#define PROCEDURE_LOAD_ALL_GIFT_CODE		"select GiftID,GiftCode,UNIX_TIMESTAMP(StartTime),UNIX_TIMESTAMP(ExpiredTime),Account,Gift from giftcode"
#define PROCEDURE_LOAD_ALL_RED_ENVELOPE		"select ID,ServerID,PlayerID,PlayerName,ImageID,MinDiamond,MaxDiamond,UNIX_TIMESTAMP(ExpireTime),Data from friendredenvelope"


#endif

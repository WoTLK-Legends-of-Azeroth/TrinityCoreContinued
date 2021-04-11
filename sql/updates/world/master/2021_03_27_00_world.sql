UPDATE `version` SET `db_version`='TDB 837.01', `cache_id`=1 LIMIT 1;
UPDATE `updates` SET `state`='ARCHIVED',`speed`=0;
REPLACE INTO `updates_include` (`path`, `state`) VALUES ('$/sql/old/8.x/world', 'ARCHIVED');
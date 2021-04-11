UPDATE `world_safe_locs` SET `MapID` = 1190 WHERE `ID` IN (4726, 4724);

DELETE FROM `creature` WHERE `guid` IN (2001144, 2001145);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnDifficulties`, `phaseUseFlags`, `PhaseId`, `PhaseGroup`, `terrainSwapMap`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `ScriptName`, `VerifiedBuild`) VALUES
('2001144','88206','0','4','4','0','0','0','0','-1','0','0','-10760.9','-2956.68','58.2953','3.10319','300','0','0','17503','0','0','0','0','0','0','0','','0'),
('2001145','88206','1190','4','4','0','0','0','0','-1','0','0','-10760.9','-2956.68','58.2953','3.10319','300','0','0','17503','0','0','0','0','0','0','0','','0');

UPDATE `creature` SET `ScriptName` = 'npc_zidormi_blasted_lands' WHERE `guid` IN (2001144, 2001145);
UPDATE `creature` SET `ScriptName` = 'npc_archmage_khadgar_blasted_lands' WHERE `guid` = 2001090;

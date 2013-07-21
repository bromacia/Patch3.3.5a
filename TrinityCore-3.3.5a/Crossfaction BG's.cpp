src/server/game/Battlegrounds/Battleground.cpp

@@ -624,7 +624,7 @@ inline Player* Battleground::_GetPlayerForTeam(uint32 teamId, BattlegroundPlayer
     {
         uint32 team = itr->second.Team;
         if (!team)
-            team = player->GetBGTeam();
+            team = player->GetTeam();

@@ -987,7 +987,6 @@ void Battleground::EndBattleground(uint32 winner)
 
         player->ResetAllPowers();
         player->CombatStopWithPets(true);
-        player->setFactionForRace(player->getRace());

@@ -1074,7 +1073,7 @@ void Battleground::RemovePlayerAtLeave(uint64 guid, bool Transport, bool SendPac
         {
             player->ClearAfkReports();
 
-            if (!team) team = player->GetBGTeam();
+            if (!team) team = player->GetTeam();

// if arena, remove the specific arena auras

@@ -1204,34 +1203,10 @@ void Battleground::AddPlayer(Player* player)
     if (player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK))
         player->ToggleAFK();
 
-    uint32 team;
-    uint64 guid;
-  
     // score struct must be created in inherited class
-    if (/* sWorld->getBoolConfig(CONFIG_BG_CROSSFRACTION) == 1 && */!isArena())
-    {
-        uint32 hCount = GetPlayersCountByTeam(HORDE);
-        uint32 aCount = GetPlayersCountByTeam(ALLIANCE);
-        guid = player->GetGUID();
             if (isArena())

-        if (aCount >= hCount)
-        {
-            team = HORDE;
-            player->SetBGTeam(HORDE);
-            player->setFaction(2);
-        }
-        else
-        {
-            team = ALLIANCE;
-            player->SetBGTeam(ALLIANCE);
-            player->setFaction(1);
-        }
-    }
-    else
-    {
-        guid = player->GetGUID();
-        team = player->GetBGTeam();
-    } 
+    uint64 guid = player->GetGUID();
+    uint32 team = player->GetBGTeam();

BattlegroundPlayer bp;
     bp.OfflineRemoveTime = 0;
@@ -1254,14 +1229,14 @@ void Battleground::AddPlayer(Player* player)
         player->RemoveArenaEnchantments(TEMP_ENCHANTMENT_SLOT);
         if (team == ALLIANCE)                                // gold
         {
-            if (player->GetBGTeam() == HORDE)
+            if (player->GetTeam() == HORDE)
                 player->CastSpell(player, SPELL_HORDE_GOLD_FLAG, true);
             else
                 player->CastSpell(player, SPELL_ALLIANCE_GOLD_FLAG, true);
         }
         else                                                // green
         {
-            if (player->GetBGTeam() == HORDE)
+            if (player->GetTeam() == HORDE)
                 player->CastSpell(player, SPELL_HORDE_GREEN_FLAG, true);
             else
                 player->CastSpell(player, SPELL_ALLIANCE_GREEN_FLAG, true);
@@ -1922,7 +1897,7 @@ void Battleground::HandleKillPlayer(Player* victim, Player* killer)
             if (!creditedPlayer || creditedPlayer == killer)
                 continue;

-            if (creditedPlayer->GetBGTeam() == killer->GetBGTeam() && creditedPlayer->IsAtGroupRewardDistance(victim))
+            if (creditedPlayer->GetTeam() == killer->GetTeam() && creditedPlayer->IsAtGroupRewardDistance(victim))
                 UpdatePlayerScore(creditedPlayer, SCORE_HONORABLE_KILLS, 1);
         }
     }

@@ -2040,7 +2015,7 @@ void Battleground::SetBgRaid(uint32 TeamID, Group* bg_raid)
 
 WorldSafeLocsEntry const* Battleground::GetClosestGraveYard(Player* player)
 {
-    return sObjectMgr->GetClosestGraveYard(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetBGTeam());
+    return sObjectMgr->GetClosestGraveYard(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetTeam());
 }
 
 void Battleground::StartTimedAchievement(AchievementCriteriaTimedTypes type, uint32 entry)

src/server/game/Battlegrounds/Battleground.h

@@ -155,10 +155,8 @@ enum BattlegroundStatus
 
 struct BattlegroundPlayer
 {
-    time_t OfflineRemoveTime; // for tracking and removing offline players from queue after 5 minutes
-    uint32 hCount;
-    uint32 aCount;
-    uint32 Team; // Player's team
+    time_t  OfflineRemoveTime;                              // for tracking and removing offline players from queue after 5 minutes
+    uint32  Team;                                           // Player's team
 };
 
 struct BattlegroundObjectInfo

src/server/game/Battlegrounds/BattlegroundMgr.cpp
@@ -21,7 +21,7 @@
 #include "ArenaTeamMgr.h"
 #include "World.h"
 #include "WorldPacket.h"
-#include "Battleground.h"
+
 #include "ArenaTeam.h"
 #include "BattlegroundMgr.h"
 #include "BattlegroundAV.h"
@@ -45,7 +45,6 @@
 #include "Formulas.h"
 #include "DisableMgr.h"
 #include "Opcodes.h"
-#include "Player.h"
 
@@ -281,7 +280,9 @@ void BattlegroundMgr::BuildPvpLogDataPacket(WorldPacket* data, Battleground* bg)
         else
         {
             Player* player = ObjectAccessor::FindPlayer(itr2->first);
-            uint32 team = player->GetBGTeam();
+            uint32 team = bg->GetPlayerTeam(itr2->first);
+            if (!team && player)
+                team = player->GetBGTeam();
             *data << uint8(team == ALLIANCE ? 1 : 0); // green or yellow
         }
         *data << uint32(itr2->second->DamageDone);              // damage done
@@ -887,22 +888,11 @@ void BattlegroundMgr::SendToBattleground(Player* player, uint32 instanceId, Batt
 {
     if (Battleground* bg = GetBattleground(instanceId, bgTypeId))
     {
-        uint32 team;
-        if (/* sWorld->getBoolConfig(CONFIG_BG_CROSSFRACTION) == 1 && */!bg->isArena())
-        {
-            uint32 hCount = bg->GetPlayersCountByTeam(HORDE);
-            uint32 aCount = bg->GetPlayersCountByTeam(ALLIANCE);
-
-            if (aCount >= hCount)
-                team = HORDE;
-            else
-                team = ALLIANCE;
-        }
-        else
-            team = player->GetBGTeam();
-
         float x, y, z, O;
         uint32 mapid = bg->GetMapId();
+        uint32 team = player->GetBGTeam();
+        if (team == 0)
+            team = player->GetTeam();
 
         bg->GetTeamStartLoc(team, x, y, z, O);
         sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BattlegroundMgr::SendToBattleground: Sending %s to map %u, X %f, Y %f, Z %f, O %f (bgType %u)", player->GetName().c_str(), mapid, x, y, z, O, bgTypeId);
@@ -1217,47 +1207,3 @@ void BattlegroundMgr::RemoveBattleground(BattlegroundTypeId bgTypeId, uint32 ins
 {
     bgDataStore[bgTypeId].m_Battlegrounds.erase(instanceId);
 }
-
-bool BattlegroundMgr::HasBattleground(Battleground *_bg)
-{
-    BattlegroundContainer::iterator itr, next;
-    for (uint32 i = BATTLEGROUND_TYPE_NONE; i < MAX_BATTLEGROUND_TYPE_ID; ++i)
-    {
-        itr = m_Battlegrounds[i].begin();
-        // skip updating battleground template
-        if (itr != m_Battlegrounds[i].end())
-            ++itr;
-        for (; itr != m_Battlegrounds[i].end(); itr = next)
-        {
-            next = itr;
-            ++next;
-            Battleground* bg = itr->second;
-            if (bg == _bg)
-                return true;
-        }
-    }
-
-    return false;
-}
-
-void BattlegroundMgr::HandleCrossfactionSendToBattle(Player* player, Battleground* bg, uint32 InstanceID, BattlegroundTypeId bgTypeId)
-{
-    if (!player || !bg)
-       return;
-
-    if (/* sWorld->getBoolConfig(CONFIG_BG_CROSSFRACTION) == 1 && */!bg->isArena())
-    {
-            if (bg->GetPlayersCountByTeam(HORDE) < bg->GetMaxPlayersPerTeam() && bg->GetPlayersCountByTeam(HORDE) <= bg->GetPlayersCountByTeam(ALLIANCE))
-                player->SetBGTeam(HORDE);
-            else
-                if (bg->GetPlayersCountByTeam(ALLIANCE) < bg->GetMaxPlayersPerTeam())
-                    player->SetBGTeam(ALLIANCE);
-
-        if (player->GetBGTeam() == HORDE)
-            player->setFaction(2); // Horde Faction
-        else
-            if (player->GetBGTeam() == ALLIANCE)
-                player->setFaction(1); // Alliance Faction
-    }
-    bg->UpdatePlayersCountByTeam(player->GetBGTeam(), false); // Add here instead of in AddPlayer, because AddPlayer is not made until loading screen is finished. Which can cause unbalance in the system.
-}

src/server/game/Battlegrounds/BattlegroundMgr.h

@@ -90,25 +90,17 @@ class BattlegroundMgr
         Battleground* GetBattlegroundTemplate(BattlegroundTypeId bgTypeId);
         Battleground* CreateNewBattleground(BattlegroundTypeId bgTypeId, PvPDifficultyEntry const* bracketEntry, uint8 arenaType, bool isRated);
 
-        BattlegroundContainer GetBattlegroundsByType(BattlegroundTypeId bgTypeId) { return m_Battlegrounds[bgTypeId]; }
-
         void AddBattleground(Battleground* bg);
         void RemoveBattleground(BattlegroundTypeId bgTypeId, uint32 instanceId);
         void AddToBGFreeSlotQueue(BattlegroundTypeId bgTypeId, Battleground* bg);
         void RemoveFromBGFreeSlotQueue(BattlegroundTypeId bgTypeId, uint32 instanceId);
         BGFreeSlotQueueContainer& GetBGFreeSlotQueueStore(BattlegroundTypeId bgTypeId);
 
-        virtual void HandleCrossfactionSendToBattle(Player* player, Battleground* bg, uint32 InstanceID, BattlegroundTypeId bgTypeId);
-
         void CreateInitialBattlegrounds();
         void DeleteAllBattlegrounds();
 
         void SendToBattleground(Player* player, uint32 InstanceID, BattlegroundTypeId bgTypeId);
 
-        void SendToBattleground(Player* player, Battleground* bg);
-
-        bool HasBattleground(Battleground *_bg);
-
         /* Battleground queues */
         BattlegroundQueue& GetBattlegroundQueue(BattlegroundQueueTypeId bgQueueTypeId) { return m_BattlegroundQueues[bgQueueTypeId]; }
         void ScheduleQueueUpdate(uint32 arenaMatchmakerRating, uint8 arenaType, BattlegroundQueueTypeId bgQueueTypeId, BattlegroundTypeId bgTypeId, BattlegroundBracketId bracket_id);
@@ -121,7 +113,6 @@ class BattlegroundMgr
 
         bool isArenaTesting() const { return m_ArenaTesting; }
         bool isTesting() const { return m_Testing; }
-        bool IsArenaType(BattlegroundTypeId bgTypeId);
 
         static BattlegroundQueueTypeId BGQueueTypeId(BattlegroundTypeId bgTypeId, uint8 arenaType);
         static BattlegroundTypeId BGTemplateId(BattlegroundQueueTypeId bgQueueTypeId);
@@ -146,11 +137,11 @@ class BattlegroundMgr
     private:
         bool CreateBattleground(CreateBattlegroundData& data);
         uint32 CreateClientVisibleInstanceId(BattlegroundTypeId bgTypeId, BattlegroundBracketId bracket_id);
+        static bool IsArenaType(BattlegroundTypeId bgTypeId);
         BattlegroundTypeId GetRandomBG(BattlegroundTypeId id);
 
         typedef std::map<BattlegroundTypeId, BattlegroundData> BattlegroundDataContainer;
         BattlegroundDataContainer bgDataStore;
-        BattlegroundContainer m_Battlegrounds[MAX_BATTLEGROUND_TYPE_ID];
 
         BattlegroundQueue m_BattlegroundQueues[MAX_BATTLEGROUND_QUEUE_TYPES];
 
src/server/game/Battlegrounds/BattlegroundQueue.cpp

@@ -140,7 +140,7 @@ GroupQueueInfo* BattlegroundQueue::AddGroup(Player* leader, Group* grp, Battlegr
     ginfo->IsInvitedToBGInstanceGUID = 0;
     ginfo->JoinTime                  = getMSTime();
     ginfo->RemoveInviteTime          = 0;
-    ginfo->Team                      = leader->GetBGTeam();
+    ginfo->Team                      = leader->GetTeam();
     ginfo->ArenaTeamRating           = ArenaRating;
     ginfo->ArenaMatchmakerRating     = MatchmakerRating;
     ginfo->OpponentsTeamRating       = 0;

 src/server/game/Battlegrounds/Zones/BattlegroundAB.cpp
@@ -219,13 +219,13 @@ void BattlegroundAB::HandleAreaTrigger(Player* player, uint32 trigger)
     switch (trigger)
     {
         case 3948:                                          // Arathi Basin Alliance Exit.
-            if (player->GetBGTeam() != ALLIANCE)
+            if (player->GetTeam() != ALLIANCE)
                 player->GetSession()->SendAreaTriggerMessage("Only The Alliance can use that portal");
             else
                 player->LeaveBattleground();
             break;
         case 3949:                                          // Arathi Basin Horde Exit.
-            if (player->GetBGTeam() != HORDE)
+            if (player->GetTeam() != HORDE)
                 player->GetSession()->SendAreaTriggerMessage("Only The Horde can use that portal");
             else
                 player->LeaveBattleground();
@@ -442,7 +442,7 @@ void BattlegroundAB::EventPlayerClickedOnFlag(Player* source, GameObject* /*targ
         return;
     }
 
-    TeamId teamIndex = GetTeamIndexByTeamId(source->GetBGTeam());
+    TeamId teamIndex = GetTeamIndexByTeamId(source->GetTeam());
 
     // Check if player really could use this banner, not cheated
     if (!(m_Nodes[node] == 0 || teamIndex == m_Nodes[node]%2))
@@ -656,7 +656,7 @@ void BattlegroundAB::EndBattleground(uint32 winner)
 
 WorldSafeLocsEntry const* BattlegroundAB::GetClosestGraveYard(Player* player)
 {
-    TeamId teamIndex = GetTeamIndexByTeamId(player->GetBGTeam());
+    TeamId teamIndex = GetTeamIndexByTeamId(player->GetTeam());
 
     // Is there any occupied node for this team?
     std::vector<uint8> nodes;

src/server/game/Battlegrounds/Zones/BattlegroundAV.cpp
@@ -54,7 +54,7 @@ void BattlegroundAV::HandleKillPlayer(Player* player, Player* killer)
         return;
 
     Battleground::HandleKillPlayer(player, killer);
-    UpdateScore(player->GetBGTeam(), -1);
+    UpdateScore(player->GetTeam(), -1);
 }
 
 void BattlegroundAV::HandleKillUnit(Creature* unit, Player* killer)
@@ -129,17 +129,17 @@ void BattlegroundAV::HandleKillUnit(Creature* unit, Player* killer)
         DelCreature(AV_CPLACE_TRIGGER18);
     }
     else if (entry == BG_AV_CreatureInfo[AV_NPC_N_MINE_N_4][0] || entry == BG_AV_CreatureInfo[AV_NPC_N_MINE_A_4][0] || entry == BG_AV_CreatureInfo[AV_NPC_N_MINE_H_4][0])
-        ChangeMineOwner(AV_NORTH_MINE, killer->GetBGTeam());
+        ChangeMineOwner(AV_NORTH_MINE, killer->GetTeam());
     else if (entry == BG_AV_CreatureInfo[AV_NPC_S_MINE_N_4][0] || entry == BG_AV_CreatureInfo[AV_NPC_S_MINE_A_4][0] || entry == BG_AV_CreatureInfo[AV_NPC_S_MINE_H_4][0])
-        ChangeMineOwner(AV_SOUTH_MINE, killer->GetBGTeam());
+        ChangeMineOwner(AV_SOUTH_MINE, killer->GetTeam());
 }
 
 void BattlegroundAV::HandleQuestComplete(uint32 questid, Player* player)
 {
     if (GetStatus() != STATUS_IN_PROGRESS)
         return;//maybe we should log this, cause this must be a cheater or a big bug
-    uint8 team = GetTeamIndexByTeamId(player->GetBGTeam());
-    //TODO add reputation, events (including quest not available anymore, next quest availabe, go/npc de/spawning)and maybe honor
+    uint8 team = GetTeamIndexByTeamId(player->GetTeam());
+    /// @todo add reputation, events (including quest not available anymore, next quest availabe, go/npc de/spawning)and maybe honor
     sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BG_AV Quest %i completed", questid);
     switch (questid)
     {
@@ -152,7 +152,7 @@ void BattlegroundAV::HandleQuestComplete(uint32 questid, Player* player)
             {
                 sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BG_AV Quest %i completed starting with unit upgrading..", questid);
                 for (BG_AV_Nodes i = BG_AV_NODES_FIRSTAID_STATION; i <= BG_AV_NODES_FROSTWOLF_HUT; ++i)
-                    if (m_Nodes[i].Owner == player->GetBGTeam() && m_Nodes[i].State == POINT_CONTROLED)
+                    if (m_Nodes[i].Owner == player->GetTeam() && m_Nodes[i].State == POINT_CONTROLED)
                     {
                         DePopulateNode(i);
                         PopulateNode(i);
@@ -163,21 +163,21 @@ void BattlegroundAV::HandleQuestComplete(uint32 questid, Player* player)
         case AV_QUEST_A_COMMANDER1:
         case AV_QUEST_H_COMMANDER1:
             m_Team_QuestStatus[team][1]++;
-            RewardReputationToTeam(team, 1, player->GetBGTeam());
+            RewardReputationToTeam(team, 1, player->GetTeam());
             if (m_Team_QuestStatus[team][1] == 30)
                 sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BG_AV Quest %i completed (need to implement some events here", questid);
             break;
         case AV_QUEST_A_COMMANDER2:
         case AV_QUEST_H_COMMANDER2:
             m_Team_QuestStatus[team][2]++;
-            RewardReputationToTeam(team, 1, player->GetBGTeam());
+            RewardReputationToTeam(team, 1, player->GetTeam());
             if (m_Team_QuestStatus[team][2] == 60)
                 sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BG_AV Quest %i completed (need to implement some events here", questid);
             break;
         case AV_QUEST_A_COMMANDER3:
         case AV_QUEST_H_COMMANDER3:
             m_Team_QuestStatus[team][3]++;
-            RewardReputationToTeam(team, 1, player->GetBGTeam());
+            RewardReputationToTeam(team, 1, player->GetTeam());
             if (m_Team_QuestStatus[team][3] == 120)
                 sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BG_AV Quest %i completed (need to implement some events here", questid);
             break;
@@ -520,13 +520,13 @@ void BattlegroundAV::HandleAreaTrigger(Player* player, uint32 trigger)
     {
         case 95:
         case 2608:
-            if (player->GetBGTeam() != ALLIANCE)
+            if (player->GetTeam() != ALLIANCE)
                 player->GetSession()->SendAreaTriggerMessage("Only The Alliance can use that portal");
             else
                 player->LeaveBattleground();
             break;
         case 2606:
-            if (player->GetBGTeam() != HORDE)
+            if (player->GetTeam() != HORDE)
                 player->GetSession()->SendAreaTriggerMessage("Only The Horde can use that portal");
             else
                 player->LeaveBattleground();
@@ -910,9 +910,9 @@ void BattlegroundAV::EventPlayerDefendsPoint(Player* player, uint32 object)
     BG_AV_Nodes node = GetNodeThroughObject(object);
 
     uint32 owner = m_Nodes[node].Owner; //maybe should name it prevowner
-    uint32 team = player->GetBGTeam();
+    uint32 team = player->GetTeam();
 
-    if (owner == player->GetBGTeam() || m_Nodes[node].State != POINT_ASSAULTED)
+    if (owner == player->GetTeam() || m_Nodes[node].State != POINT_ASSAULTED)
         return;
     if (m_Nodes[node].TotalOwner == AV_NEUTRAL_TEAM)
     { //until snowfall doesn't belong to anyone it is better handled in assault-code
@@ -982,7 +982,7 @@ void BattlegroundAV::EventPlayerAssaultsPoint(Player* player, uint32 object)
 
     BG_AV_Nodes node = GetNodeThroughObject(object);
     uint32 owner = m_Nodes[node].Owner; //maybe name it prevowner
-    uint32 team  = player->GetBGTeam();
+    uint32 team  = player->GetTeam();
     sLog->outDebug(LOG_FILTER_BATTLEGROUND, "bg_av: player assaults point object %i node %i", object, node);
     if (owner == team || team == m_Nodes[node].TotalOwner)
         return; //surely a gm used this object
@@ -1196,11 +1196,11 @@ WorldSafeLocsEntry const* BattlegroundAV::GetClosestGraveYard(Player* player)
 
     player->GetPosition(x, y);
 
-    pGraveyard = sWorldSafeLocsStore.LookupEntry(BG_AV_GraveyardIds[GetTeamIndexByTeamId(player->GetBGTeam())+7]);
+    pGraveyard = sWorldSafeLocsStore.LookupEntry(BG_AV_GraveyardIds[GetTeamIndexByTeamId(player->GetTeam())+7]);
     minDist = (pGraveyard->x - x)*(pGraveyard->x - x)+(pGraveyard->y - y)*(pGraveyard->y - y);
 
     for (uint8 i = BG_AV_NODES_FIRSTAID_STATION; i <= BG_AV_NODES_FROSTWOLF_HUT; ++i)
-        if (m_Nodes[i].Owner == player->GetBGTeam() && m_Nodes[i].State == POINT_CONTROLED)
+        if (m_Nodes[i].Owner == player->GetTeam() && m_Nodes[i].State == POINT_CONTROLED)
         {
             entry = sWorldSafeLocsStore.LookupEntry(BG_AV_GraveyardIds[i]);
             if (entry)

src/server/game/Battlegrounds/Zones/BattlegroundEY.cpp

@@ -208,7 +208,7 @@ void BattlegroundEY::CheckSomeoneLeftPoint()
                 else
                 {
                     //player is neat flag, so update count:
-                    m_CurrentPointPlayersCount[2 * i + GetTeamIndexByTeamId(player->GetBGTeam())]++;
+                    m_CurrentPointPlayersCount[2 * i + GetTeamIndexByTeamId(player->GetTeam())]++;
                     ++j;
                 }
             }
@@ -251,16 +251,16 @@ void BattlegroundEY::UpdatePointStatuses()
                 if (pointOwnerTeamId != m_PointOwnedByTeam[point])
                 {
                     //point was uncontrolled and player is from team which captured point
-                    if (m_PointState[point] == EY_POINT_STATE_UNCONTROLLED && player->GetBGTeam() == pointOwnerTeamId)
+                    if (m_PointState[point] == EY_POINT_STATE_UNCONTROLLED && player->GetTeam() == pointOwnerTeamId)
                         this->EventTeamCapturedPoint(player, point);
 
                     //point was under control and player isn't from team which controlled it
-                    if (m_PointState[point] == EY_POINT_UNDER_CONTROL && player->GetBGTeam() != m_PointOwnedByTeam[point])
+                    if (m_PointState[point] == EY_POINT_UNDER_CONTROL && player->GetTeam() != m_PointOwnedByTeam[point])
                         this->EventTeamLostPoint(player, point);
                 }
 
                 /// @workaround The original AreaTrigger is covered by a bigger one and not triggered on client side.
-                if (point == FEL_REAVER && m_PointOwnedByTeam[point] == player->GetBGTeam())
+                if (point == FEL_REAVER && m_PointOwnedByTeam[point] == player->GetTeam())
                     if (m_FlagState && GetFlagPickerGUID() == player->GetGUID())
                         if (player->GetDistance(2044.0f, 1729.729f, 1190.03f) < 3.0f)
                             EventPlayerCapturedFlag(player, BG_EY_OBJECT_FLAG_FEL_REAVER);
@@ -387,22 +387,22 @@ void BattlegroundEY::HandleAreaTrigger(Player* player, uint32 trigger)
     switch (trigger)
     {
         case TR_BLOOD_ELF_POINT:
-            if (m_PointState[BLOOD_ELF] == EY_POINT_UNDER_CONTROL && m_PointOwnedByTeam[BLOOD_ELF] == player->GetBGTeam())
+            if (m_PointState[BLOOD_ELF] == EY_POINT_UNDER_CONTROL && m_PointOwnedByTeam[BLOOD_ELF] == player->GetTeam())
                 if (m_FlagState && GetFlagPickerGUID() == player->GetGUID())
                     EventPlayerCapturedFlag(player, BG_EY_OBJECT_FLAG_BLOOD_ELF);
             break;
         case TR_FEL_REAVER_POINT:
-            if (m_PointState[FEL_REAVER] == EY_POINT_UNDER_CONTROL && m_PointOwnedByTeam[FEL_REAVER] == player->GetBGTeam())
+            if (m_PointState[FEL_REAVER] == EY_POINT_UNDER_CONTROL && m_PointOwnedByTeam[FEL_REAVER] == player->GetTeam())
                 if (m_FlagState && GetFlagPickerGUID() == player->GetGUID())
                     EventPlayerCapturedFlag(player, BG_EY_OBJECT_FLAG_FEL_REAVER);
             break;
         case TR_MAGE_TOWER_POINT:
-            if (m_PointState[MAGE_TOWER] == EY_POINT_UNDER_CONTROL && m_PointOwnedByTeam[MAGE_TOWER] == player->GetBGTeam())
+            if (m_PointState[MAGE_TOWER] == EY_POINT_UNDER_CONTROL && m_PointOwnedByTeam[MAGE_TOWER] == player->GetTeam())
                 if (m_FlagState && GetFlagPickerGUID() == player->GetGUID())
                     EventPlayerCapturedFlag(player, BG_EY_OBJECT_FLAG_MAGE_TOWER);
             break;
         case TR_DRAENEI_RUINS_POINT:
-            if (m_PointState[DRAENEI_RUINS] == EY_POINT_UNDER_CONTROL && m_PointOwnedByTeam[DRAENEI_RUINS] == player->GetBGTeam())
+            if (m_PointState[DRAENEI_RUINS] == EY_POINT_UNDER_CONTROL && m_PointOwnedByTeam[DRAENEI_RUINS] == player->GetTeam())
                 if (m_FlagState && GetFlagPickerGUID() == player->GetGUID())
                     EventPlayerCapturedFlag(player, BG_EY_OBJECT_FLAG_DRAENEI_RUINS);
             break;
@@ -621,7 +621,7 @@ void BattlegroundEY::EventPlayerDroppedFlag(Player* Source)
     UpdateWorldState(NETHERSTORM_FLAG_STATE_HORDE, BG_EY_FLAG_STATE_WAIT_RESPAWN);
     UpdateWorldState(NETHERSTORM_FLAG_STATE_ALLIANCE, BG_EY_FLAG_STATE_WAIT_RESPAWN);
 
-    if (Source->GetBGTeam() == ALLIANCE)
+    if (Source->GetTeam() == ALLIANCE)
         SendMessageToAll(LANG_BG_EY_DROPPED_FLAG, CHAT_MSG_BG_SYSTEM_ALLIANCE, NULL);
     else
         SendMessageToAll(LANG_BG_EY_DROPPED_FLAG, CHAT_MSG_BG_SYSTEM_HORDE, NULL);
@@ -632,7 +632,7 @@ void BattlegroundEY::EventPlayerClickedOnFlag(Player* Source, GameObject* target
     if (GetStatus() != STATUS_IN_PROGRESS || IsFlagPickedup() || !Source->IsWithinDistInMap(target_obj, 10))
         return;
 
-    if (Source->GetBGTeam() == ALLIANCE)
+    if (Source->GetTeam() == ALLIANCE)
     {
         UpdateWorldState(NETHERSTORM_FLAG_STATE_ALLIANCE, BG_EY_FLAG_STATE_ON_PLAYER);
         PlaySoundToAll(BG_EY_SOUND_FLAG_PICKED_UP_ALLIANCE);
@@ -653,7 +653,7 @@ void BattlegroundEY::EventPlayerClickedOnFlag(Player* Source, GameObject* target
     Source->CastSpell(Source, BG_EY_NETHERSTORM_FLAG_SPELL, true);
     Source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
 
-    if (Source->GetBGTeam() == ALLIANCE)
+    if (Source->GetTeam() == ALLIANCE)
         PSendMessageToAll(LANG_BG_EY_HAS_TAKEN_FLAG, CHAT_MSG_BG_SYSTEM_ALLIANCE, NULL, Source->GetName().c_str());
     else
         PSendMessageToAll(LANG_BG_EY_HAS_TAKEN_FLAG, CHAT_MSG_BG_SYSTEM_HORDE, NULL, Source->GetName().c_str());
@@ -712,7 +712,7 @@ void BattlegroundEY::EventTeamCapturedPoint(Player* Source, uint32 Point)
     if (GetStatus() != STATUS_IN_PROGRESS)
         return;
 
-    uint32 Team = Source->GetBGTeam();
+    uint32 Team = Source->GetTeam();
 
     SpawnBGObject(m_CapturingPointTypes[Point].DespawnNeutralObjectType, RESPAWN_ONE_DAY);
     SpawnBGObject(m_CapturingPointTypes[Point].DespawnNeutralObjectType + 1, RESPAWN_ONE_DAY);
@@ -785,7 +785,7 @@ void BattlegroundEY::EventPlayerCapturedFlag(Player* Source, uint32 BgObjectType
 
     Source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
 
-    if (Source->GetBGTeam() == ALLIANCE)
+    if (Source->GetTeam() == ALLIANCE)
         PlaySoundToAll(BG_EY_SOUND_FLAG_CAPTURED_ALLIANCE);
     else
         PlaySoundToAll(BG_EY_SOUND_FLAG_CAPTURED_HORDE);
@@ -796,7 +796,7 @@ void BattlegroundEY::EventPlayerCapturedFlag(Player* Source, uint32 BgObjectType
     m_FlagCapturedBgObjectType = BgObjectType;
 
     uint8 team_id = 0;
-    if (Source->GetBGTeam() == ALLIANCE)
+    if (Source->GetTeam() == ALLIANCE)
     {
         team_id = TEAM_ALLIANCE;
         SendMessageToAll(LANG_BG_EY_CAPTURED_FLAG_A, CHAT_MSG_BG_SYSTEM_ALLIANCE, Source);
@@ -808,7 +808,7 @@ void BattlegroundEY::EventPlayerCapturedFlag(Player* Source, uint32 BgObjectType
     }
 
     if (m_TeamPointsCount[team_id] > 0)
-        AddPoints(Source->GetBGTeam(), BG_EY_FlagPoints[m_TeamPointsCount[team_id] - 1]);
+        AddPoints(Source->GetTeam(), BG_EY_FlagPoints[m_TeamPointsCount[team_id] - 1]);
 
     UpdatePlayerScore(Source, SCORE_FLAG_CAPTURES, 1);
 }
@@ -885,7 +885,7 @@ WorldSafeLocsEntry const* BattlegroundEY::GetClosestGraveYard(Player* player)
 {
     uint32 g_id = 0;
 
-    switch (player->GetBGTeam())
+    switch (player->GetTeam())
     {
         case ALLIANCE: g_id = EY_GRAVEYARD_MAIN_ALLIANCE; break;
         case HORDE:    g_id = EY_GRAVEYARD_MAIN_HORDE;    break;
@@ -914,7 +914,7 @@ WorldSafeLocsEntry const* BattlegroundEY::GetClosestGraveYard(Player* player)
 
     for (uint8 i = 0; i < EY_POINTS_MAX; ++i)
     {
-        if (m_PointOwnedByTeam[i] == player->GetBGTeam() && m_PointState[i] == EY_POINT_UNDER_CONTROL)
+        if (m_PointOwnedByTeam[i] == player->GetTeam() && m_PointState[i] == EY_POINT_UNDER_CONTROL)
         {
             entry = sWorldSafeLocsStore.LookupEntry(m_CapturingPointTypes[i].GraveYardId);
             if (!entry)

src/server/game/Battlegrounds/Zones/BattlegroundWS.cpp
@@ -282,7 +282,7 @@ void BattlegroundWS::EventPlayerCapturedFlag(Player* Source)
     uint32 winner = 0;
 
     Source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
-    if (Source->GetBGTeam() == ALLIANCE)
+    if (Source->GetTeam() == ALLIANCE)
     {
         if (!IsHordeFlagPickedup())
             return;
@@ -319,23 +319,23 @@ void BattlegroundWS::EventPlayerCapturedFlag(Player* Source)
         RewardReputationToTeam(889, m_ReputationCapture, HORDE);
     }
     //for flag capture is reward 2 honorable kills
-    RewardHonorToTeam(GetBonusHonorFromKill(2), Source->GetBGTeam());
+    RewardHonorToTeam(GetBonusHonorFromKill(2), Source->GetTeam());
 
     SpawnBGObject(BG_WS_OBJECT_H_FLAG, BG_WS_FLAG_RESPAWN_TIME);
     SpawnBGObject(BG_WS_OBJECT_A_FLAG, BG_WS_FLAG_RESPAWN_TIME);
 
-    if (Source->GetBGTeam() == ALLIANCE)
+    if (Source->GetTeam() == ALLIANCE)
         SendMessageToAll(LANG_BG_WS_CAPTURED_HF, CHAT_MSG_BG_SYSTEM_ALLIANCE, Source);
     else
         SendMessageToAll(LANG_BG_WS_CAPTURED_AF, CHAT_MSG_BG_SYSTEM_HORDE, Source);
 
-    UpdateFlagState(Source->GetBGTeam(), 1);                  // flag state none
+    UpdateFlagState(Source->GetTeam(), 1);                  // flag state none
     UpdateTeamScore(Source->GetTeamId());
     // only flag capture should be updated
     UpdatePlayerScore(Source, SCORE_FLAG_CAPTURES, 1);      // +1 flag captures
 
     // update last flag capture to be used if teamscore is equal
-    SetLastFlagCapture(Source->GetBGTeam());
+    SetLastFlagCapture(Source->GetTeam());
 
     if (GetTeamScore(TEAM_ALLIANCE) == BG_WS_MAX_TEAM_SCORE)
         winner = ALLIANCE;
@@ -356,7 +356,7 @@ void BattlegroundWS::EventPlayerCapturedFlag(Player* Source)
     }
     else
     {
-        _flagsTimer[GetTeamIndexByTeamId(Source->GetBGTeam()) ? 0 : 1] = BG_WS_FLAG_RESPAWN_TIME;
+        _flagsTimer[GetTeamIndexByTeamId(Source->GetTeam()) ? 0 : 1] = BG_WS_FLAG_RESPAWN_TIME;
     }
 }
 
@@ -366,7 +366,7 @@ void BattlegroundWS::EventPlayerDroppedFlag(Player* Source)
     {
         // if not running, do not cast things at the dropper player (prevent spawning the "dropped" flag), neither send unnecessary messages
         // just take off the aura
-        if (Source->GetBGTeam() == ALLIANCE)
+        if (Source->GetTeam() == ALLIANCE)
         {
             if (!IsHordeFlagPickedup())
                 return;
@@ -391,7 +391,7 @@ void BattlegroundWS::EventPlayerDroppedFlag(Player* Source)
 
     bool set = false;
 
-    if (Source->GetBGTeam() == ALLIANCE)
+    if (Source->GetTeam() == ALLIANCE)
     {
         if (!IsHordeFlagPickedup())
             return;
@@ -429,9 +429,9 @@ void BattlegroundWS::EventPlayerDroppedFlag(Player* Source)
     if (set)
     {
         Source->CastSpell(Source, SPELL_RECENTLY_DROPPED_FLAG, true);
-        UpdateFlagState(Source->GetBGTeam(), 1);
+        UpdateFlagState(Source->GetTeam(), 1);
 
-        if (Source->GetBGTeam() == ALLIANCE)
+        if (Source->GetTeam() == ALLIANCE)
         {
             SendMessageToAll(LANG_BG_WS_DROPPED_HF, CHAT_MSG_BG_SYSTEM_HORDE, Source);
             UpdateWorldState(BG_WS_FLAG_UNK_HORDE, uint32(-1));
@@ -442,7 +442,7 @@ void BattlegroundWS::EventPlayerDroppedFlag(Player* Source)
             UpdateWorldState(BG_WS_FLAG_UNK_ALLIANCE, uint32(-1));
         }
 
-        _flagsDropTimer[GetTeamIndexByTeamId(Source->GetBGTeam()) ? 0 : 1] = BG_WS_FLAG_DROP_TIME;
+        _flagsDropTimer[GetTeamIndexByTeamId(Source->GetTeam()) ? 0 : 1] = BG_WS_FLAG_DROP_TIME;
     }
 }
 
@@ -455,7 +455,7 @@ void BattlegroundWS::EventPlayerClickedOnFlag(Player* Source, GameObject* target
     ChatMsg type = CHAT_MSG_BG_SYSTEM_NEUTRAL;
 
     //alliance flag picked up from base
-    if (Source->GetBGTeam() == HORDE && GetFlagState(ALLIANCE) == BG_WS_FLAG_STATE_ON_BASE
+    if (Source->GetTeam() == HORDE && GetFlagState(ALLIANCE) == BG_WS_FLAG_STATE_ON_BASE
         && BgObjects[BG_WS_OBJECT_A_FLAG] == target_obj->GetGUID())
     {
         message_id = LANG_BG_WS_PICKEDUP_AF;
@@ -474,7 +474,7 @@ void BattlegroundWS::EventPlayerClickedOnFlag(Player* Source, GameObject* target
     }
 
     //horde flag picked up from base
-    if (Source->GetBGTeam() == ALLIANCE && GetFlagState(HORDE) == BG_WS_FLAG_STATE_ON_BASE
+    if (Source->GetTeam() == ALLIANCE && GetFlagState(HORDE) == BG_WS_FLAG_STATE_ON_BASE
         && BgObjects[BG_WS_OBJECT_H_FLAG] == target_obj->GetGUID())
     {
         message_id = LANG_BG_WS_PICKEDUP_HF;
@@ -496,7 +496,7 @@ void BattlegroundWS::EventPlayerClickedOnFlag(Player* Source, GameObject* target
     if (GetFlagState(ALLIANCE) == BG_WS_FLAG_STATE_ON_GROUND && Source->IsWithinDistInMap(target_obj, 10)
         && target_obj->GetGOInfo()->entry == BG_OBJECT_A_FLAG_GROUND_WS_ENTRY)
     {
-        if (Source->GetBGTeam() == ALLIANCE)
+        if (Source->GetTeam() == ALLIANCE)
         {
             message_id = LANG_BG_WS_RETURNED_AF;
             type = CHAT_MSG_BG_SYSTEM_ALLIANCE;
@@ -531,7 +531,7 @@ void BattlegroundWS::EventPlayerClickedOnFlag(Player* Source, GameObject* target
     if (GetFlagState(HORDE) == BG_WS_FLAG_STATE_ON_GROUND && Source->IsWithinDistInMap(target_obj, 10)
         && target_obj->GetGOInfo()->entry == BG_OBJECT_H_FLAG_GROUND_WS_ENTRY)
     {
-        if (Source->GetBGTeam() == HORDE)
+        if (Source->GetTeam() == HORDE)
         {
             message_id = LANG_BG_WS_RETURNED_HF;
             type = CHAT_MSG_BG_SYSTEM_HORDE;
@@ -802,7 +802,7 @@ WorldSafeLocsEntry const* BattlegroundWS::GetClosestGraveYard(Player* player)
     //if a player dies in preparation phase - then the player can't cheat
     //and teleport to the graveyard outside the flagroom
     //and start running around, while the doors are still closed
-    if (player->GetBGTeam() == ALLIANCE)
+    if (player->GetTeam() == ALLIANCE)
     {
         if (GetStatus() == STATUS_IN_PROGRESS)
             return sWorldSafeLocsStore.LookupEntry(WS_GRAVEYARD_MAIN_ALLIANCE);

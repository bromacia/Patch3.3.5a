//by SymbolixDEV
//Fixed Error SymbolixDEV
//release SymbolixDEV
#include "ScriptPCH.h"
 
class Boss_Announcer : public PlayerScript
{
    public:
    Boss_Announcer() : PlayerScript ("Boss_Announcer") {} 
 
    void OnCreatureKill(Player* player, Creature* boss) 
    {
        if (boss->isWorldBoss())
        {
            std::string plr = player->GetName();
            std::string boss_n = boss->GetName();
            bool ingroup = player->GetGroup();
            std::string tag_colour = "7bbef7";
            std::string plr_colour = "7bbef7";
            std::string boss_colour = "ff0000";
            std::ostringstream stream;
            stream << "|CFF" << tag_colour << 
                      "|r|cff" << plr_colour << " " << plr <<
                      "|r BOSS |CFF" << boss_colour << "[" << boss_n << "]|r " "group" << "!";
            sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
        }
    };
};
 
void AddSC_Boss_Announcer()
{
    new Boss_Announcer;
}

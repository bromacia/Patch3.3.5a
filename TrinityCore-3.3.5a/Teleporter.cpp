/*
Teleporter Script for Trinity
Coded By Lhorath
Coding Inspired From Rochet2 Teleporter
*/

//   Link Types//
/*
{#,  #,	2,	"|cff4169E1 Non-Icon Links|r",	0,	0,	0,	0,	0,	0,	0,	0},
{#,	#,	2,	"|cff4169E1|TInterface\\icons\\Achievement_Leader_King_Varian_Wrynn:30|t Icon & Color Links|r",	0,	0,	0,	0,	0,	0,	0,	0},
{#,	#,	2,	"|TInterface\\icons\\Achievement_Leader_King_Varian_Wrynn:30|t Icon Links (NoColor)|t",	0,	0,	0,	0,	0,	0,	0,	0},
{#,	#,	2,	"Non-Formated Links Links",	0,	0,	0,	0,	0,	0,	0,	0},
*/

// Color List //
/*
ORANGE			"|cffFFA500"
DARKORANGE		"|cffFF8C00"
RED				"|cffFF0000"
LIGHTRED		"|cffD63931"
ROYALBLUE		"|cff4169E1"
LIGHTBLUE		"|cffADD8E6"
YELLOW			"|cffFFFF00"
GREEN			"|cff008000"
PURPLE			"|cffDA70D6"
WHITE			"|cffffffff"
SUBWHITE		"|cffbbbbbb"
*/

#include "ScriptPCH.h"

// Teleport data

struct Eric
{
	uint32 menu_id, next_menu_id;
	uint8 icon;
	std::string name;
	uint32 cost;
	uint8 level, faction; // 0 Both, 1 Horde, 2 Ally
	uint32 map;
	float x, y, z, o;
};
typedef struct Eric Rochet2;

//	{menu_id, next_menu_id, icon, name, cost, level, faction, map, x, y, z, o}
Rochet2 TeLe [] = // key = i & Key
{
	{1,	2,	2,	"|cff4169E1|TInterface\\icons\\Achievement_Leader_King_Varian_Wrynn:30|t Alliance Capital Cities|r",	0,	0,	0,	0,	0,	0,	0,	0},
	{1,	3,	2,	"|cffFF0000|TInterface\\icons\\Achievement_Leader_Sylvanas:30|t Horde Capital Cities|r",	0,	0,	0,	0,	0,	0,	0,	0},
	{1,	4,	2,	"|cffFFFF00|TInterface\\icons\\Temp:30|t Neutral Capital Cities|r",	0,	0,	0,	0,	0,	0,	0,	0},
	{1,	5,	9,	"|cffFF8C00|TInterface\\icons\\Achievement_FeatsOfStrength_Gladiator_10:30|t PvP Areas|r",	0,	0,	0,	0,	0,	0,	0,	0},
	{1,	6,	3,	"|cffADD8E6|TInterface\\icons\\Achievement_Dungeon_Icecrown_Frostmourne:30|t Raids|r",	0,	0,	0,	0,	0,	0,	0,	0},

	{2,	0,	2,	"Stormwind",	0,	0,	2,	0,	-8842.09f,	626.358f,	94.0867f,	3.61363f},
	{2,	0,	2,	"Darnassus",	0,	0,	2,	1,	9869.91f,	2493.58f,	1315.88f,	2.78897f},
	{2,	0,	2,	"Ironforge",	0,	0,	2,	0,	-4900.47f,	-962.585f,	501.455f,	5.40538f},
	{2,	0,	2,	"Exodar",	0,	0,	2,	530,	-3864.92f,	-11643.7f,	-137.644f,	5.50862f},
	{2,	0,	9,	"|cffFF0000Attack Stormwind|r",	0,	80,	1,	0,	-9449.06f,	64.8392f,	56.3581f,	3.07047f},
	{2,	0,	9,	"|cffFF0000Attack Darnassus|r",	0,	80,	1,	1,	9889.03f,	915.869f,	1307.43f,	1.9336f},
	{2,	0,	9,	"|cffFF0000Attack Ironforge|r",	0,	80,	1,	0,	-5603.76f,	-482.704f,	396.98f,	5.23499f},
	{2,	0,	9,	"|cffFF0000Attack Exodar|r",	0,	80,	1,	530,	-4192.62f,	-12576.7f,	36.7598f,	1.62813f},
	{2,	1,	7,	"|cffffffffBack..|r",	0,	0,	0,	0,	0,	0,	0,	0},

	{3,	0,	2,	"Orgrimmar",	0,	0,	1,	1,	1601.08f,	-4378.69f,	9.9846f,	2.14362f},
	{3,	0,	2,	"Thunder bluff",	0,	0,	1,	1,	-1274.45f,	71.8601f,	128.159f,	2.80623f},
	{3,	0,	2,	"Undercity",	0,	0,	1,	0,	1633.75f,	240.167f,	-43.1034f,	6.26128f},
	{3,	0,	2,	"Silvermoon city",	0,	0,	1,	530,	9738.28f,	-7454.19f,	13.5605f,	0.043914f},
	{3,	0,	9,	"|cffFF0000Attack Orgrimmar|r",	0,	80,	2,	1,	228.978f,	-4741.87f,	10.1027f,	0.416883f},
	{3,	0,	9,	"|cffFF0000Attack Thunder bluff|r",	0,	80,	2,	1,	-2473.87f,	-501.225f,	-9.42465f,	0.6525f},
	{3,	0,	9,	"|cffFF0000Attack Undercity|r",	0,	80,	2,	0,	2274.95f,	323.918f,	34.1137f,	4.24367f},
	{3,	0,	9,	"|cffFF0000Attack Silvermoon|r",	0,	80,	2,	530,	9024.37f,	-6682.55f,	16.8973f,	3.14131f},
	{3,	1,	7,	"|cffffffffBack..|r",	0,	0,	0,	0,	0,	0,	0,	0},

	{4,	0,	2,	"Dalaran",	0,	67,	0,	571,	5809.55f,	503.975f,	657.526f,	2.38338f},
	{4,	0,	2,	"Shattrath",	0,	57,	0,	530,	-1887.62f,	5359.09f,	-12.4279f,	4.40435f},
	{4,	0,	2,	"Booty bay",	0,	30,	0,	0,	-14281.9f,	552.564f,	8.90422f,	0.860144f},
	{4,	1,	7,	"|cffffffffBack..|r",	0,	0,	0,	0,	0,	0,	0,	0},

	{5,	0,	2,	"Gurubashi arena",	0,	30,	0,	0,	-13181.8f,	339.356f,	42.9805f,	1.18013f},
	{5,	1,	7,	"|cffffffffBack..|r",	0,	0,	0,	0,	0,	0,	0,	0},

	{6,	0,	2,	"Trial of the Crusader",	0,	80,	0,	571,	8515.61f,	714.153f,	558.248f,	1.57753f},
	{6,	0,	2,	"Icecrown Citadel",	0,	80,	0,	571,	5855.22f,	2102.03f,	635.991f,	3.57899f},
	{6,	0,	2,	"Naxxramas",	0,	80,	0,	571,	3668.72f,	-1262.46f,	243.622f,	4.785f},
	{6,	0,	2,	"Onyxia's Lair",	0,	80,	0,	1,	-4708.27f,	-3727.64f,	54.5589f,	3.72786f},
	{6,	0,	2,	"The Eye of Eternity",	0,	80,	0,	571,	3784.17f,	7028.84f,	161.258f,	5.79993f},
	{6,	0,	2,	"The Obsidian Sanctum",	0,	80,	0,	571,	3472.43f,	264.923f,	-120.146f,	3.27923f},
	{6,	0,	2,	"Ulduar",	0,	80,	0,	571,	9222.88f,	-1113.59f,	1216.12f,	6.27549f},
	{6,	0,	2,	"Vault of Archavon",	0,	80,	0,	571,	5453.72f,	2840.79f,	421.28f,	0.0f},
	{6,	7,	7,	"|cffffffffBack..|r",	0,	0,	0,	0,	0,	0,	0,	0},

//	{10,	0,	2,	"Teleport",	0,	0,	0,	map,	xf,	yf,	zf,	of},
//	{10,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
};

// TextIDs from npc_text
enum eEnums
{
	TEXT_MAIN_H		=	400000,			//Horde main menu text
	TEXT_MAIN_A		=	400001,			//Alliance main menu text
	TEXT_DUNGEON	=	400002,			//Dungeon teleport menu texts
	TEXT_RAID		=	400003,			//Raid teleport menu text
	TEXT_AREA		=	400004,			//Area teleport location menu texts

	TELEPORT_COUNT	=	sizeof TeLe/sizeof(*TeLe),
};

#define ARE_YOU_SURE	"|cffffffffGetting a Move on to|r "
#define ERROR_COMBAT	"|cffff0000Kill Whatever that thing is first!|r"

bool Custom_FactCheck (uint32 Fact, unsigned char Key)
{
	bool Show = false;
	switch (TeLe[Key].faction)
	{
	case 0:
		Show = true;
		break;
	case 1:
		if (Fact == HORDE)
			Show = true;
		break;
	case 2:
		if (Fact == ALLIANCE)
			Show = true;
		break;
	}
	return (Show);
}

uint32 Custom_GetText (unsigned int menu, Player* pPlayer)
{
	uint32 TEXT = TEXT_AREA;
	switch (menu)
	{
	case 0:
		switch (pPlayer->GetTeam())
		{
		case ALLIANCE:
			TEXT = TEXT_MAIN_A;
			break;
		case HORDE:
			TEXT = TEXT_MAIN_H;
			break;
		}
	case 1:
	case 2:
	case 3:
		TEXT = TEXT_DUNGEON;
		break;
	case 4:
		TEXT = TEXT_RAID;
		break;
	}
	return (TEXT);
}

void Custom_GetMenu (Player* pPlayer, Creature* pCreature, uint32 Key)
{
	bool ENDMENU = false;
	for(uint32 i = 0; i < TELEPORT_COUNT; i++)
	{
		if (ENDMENU && TeLe[i].menu_id != Key)
			break;
		if (TeLe[i].menu_id == Key && pPlayer->getLevel() >= TeLe[i].level && Custom_FactCheck(pPlayer->GetTeam(), i))
		{
			if (TeLe[i].next_menu_id != 0)
				pPlayer->ADD_GOSSIP_ITEM_EXTENDED(TeLe[i].icon, TeLe[i].name, GOSSIP_SENDER_MAIN, i, "", TeLe[i].cost, false);
			else
				pPlayer->ADD_GOSSIP_ITEM_EXTENDED(TeLe[i].icon, TeLe[i].name, GOSSIP_SENDER_MAIN, i, ARE_YOU_SURE+TeLe[i].name, TeLe[i].cost, false);
			ENDMENU = true;
		}
	}
	pPlayer->PlayerTalkClass->SendGossipMenu(Custom_GetText(Key, pPlayer), pCreature->GetGUID());
}

class tele_script_mortal : public CreatureScript
{
	public:
	tele_script_mortal()
	: CreatureScript("tele_script_mortal")
	{
	}

	bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	{
		Custom_GetMenu(pPlayer, pCreature, 1);
		return true;
	}

	bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
	{
		pPlayer->PlayerTalkClass->ClearMenus(); // clear the menu

		pPlayer->ModifyMoney(-1 * TeLe[uiAction].cost); // take cash
		uint32 Key = TeLe[uiAction].next_menu_id;
		if (Key == 0) // if no next_menu_id, then teleport to coords
		{
			if (!pPlayer->isInCombat())
			{
				pPlayer->CLOSE_GOSSIP_MENU();
				pPlayer->TeleportTo(TeLe[uiAction].map, TeLe[uiAction].x, TeLe[uiAction].y, TeLe[uiAction].z, TeLe[uiAction].o);
				return true;
			}
			pPlayer->GetSession()->SendAreaTriggerMessage(ERROR_COMBAT);
			Key = TeLe[uiAction].menu_id;
		}

		Custom_GetMenu(pPlayer, pCreature, Key);
		return true;
	}
};

void AddSC_tele_script_mortal()
{
    new tele_script_mortal();
}

enum StatType
{
	SCORE_COMBAT,
	SCORE_OBJECTIVE,
	SCORE_LOGISTICS,
	SCORE_VETERAN,
	SCORE_INTEL,
	PLAYER_KILLS,
	PLAYER_DEATHS,
	NPC_KILLS,
	NPC_DEATHS,
	FRIENDLY_KILLS,
	FRIENDLY_DEATHS,
	SUICIDES,
	CAPTURES,
	DEFENDS,
	SUPPLIES,
	TRANSPORT,
	BUILDING,
	SPAWNPOINT,
	INTEL_SMALL,
	INTEL_MEDIUM,
	INTEL_LARGE,
	VOTEKICK_STARTED,
	VOTEKICK_ACCUSED
}

class PlayerStats : JsonApiStruct
{
	string ingameName;
	string lastPlayed;
	string discordId;
	int combatScore;
	int objectiveScore;
	int logisticsScore;
	int veteranScore;
	int intelScore;
	int playerKills;
	int playerDeaths;
	int npcKills;
	int npcDeaths;
	int friendlyKills;
	int friendlyDeaths;
	int suicides;
	int captures;
	int defends;
	int supplies;
	int transport;
	int building;
	int spawnpoint;
	int intelSmall;
	int intelMedium;
	int intelLarge;
	int voteStarted;
	int voteAccused;

	void PlayerStats()
	{
		RegAll();
	}
	
	void AddStat(StatType stat, int value)
	{
		switch (stat)
		{
			case StatType.SCORE_COMBAT:
				combatScore += value;
				break;
			
			case StatType.SCORE_OBJECTIVE:
				objectiveScore += value;
				break;
			
			case StatType.SCORE_LOGISTICS:
				logisticsScore += value;
				break;
			
			case StatType.SCORE_VETERAN:
				veteranScore += value;
				break;
			
			case StatType.SCORE_INTEL:
				intelScore += value;
				break;

			case StatType.PLAYER_KILLS:
				playerKills += value;
				break;
			
			case StatType.PLAYER_DEATHS:
				playerDeaths += value;
				break;

			case StatType.NPC_KILLS:
				npcKills += value;
				break;
			
			case StatType.NPC_DEATHS:
				npcDeaths += value;
				break;

			case StatType.FRIENDLY_KILLS:
				friendlyKills += value;
				break;
			
			case StatType.FRIENDLY_DEATHS:
				friendlyDeaths += value;
				break;

			case StatType.SUICIDES:
				suicides += value;
				break;

			case StatType.CAPTURES:
				captures += value;
				break;
			
			case StatType.DEFENDS:
				defends += value;
				break;

			case StatType.SUPPLIES:
				supplies += value;
				break;
			
			case StatType.TRANSPORT:
				transport += value;
				break;
			
			case StatType.BUILDING:
				building += value;
				break;
			
			case StatType.SPAWNPOINT:
				spawnpoint += value;
				break;

			case StatType.INTEL_SMALL:
				intelSmall += value;
				break;
			
			case StatType.INTEL_MEDIUM:
				intelMedium += value;
				break;
			
			case StatType.INTEL_LARGE:
				intelLarge += value;
				break;
			
			case StatType.VOTEKICK_STARTED:
				voteStarted += value;
				break;
			
			case StatType.VOTEKICK_ACCUSED:
				voteAccused += value;
				break;
		}
	}
}
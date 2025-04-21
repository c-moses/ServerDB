class StatQueue
{
	protected static ref StatQueue s_Instance;
	protected ref map<string, ref PlayerStats> m_QueuedStats;
	protected string m_playerDatabase = "$profile:/PlayerDatabase";

	void UpdateStat(string playerId, StatType stat, int value)
	{
		if (playerId == string.Empty)
			return;

		PlayerStats stats;
		if (m_QueuedStats.Contains(playerId))
			stats = m_QueuedStats.Get(playerId);
		else
		{
			stats = new PlayerStats();
			m_QueuedStats.Insert(playerId, stats);
		}

		stats.AddStat(stat, value);
	}

	void ScheduledStatUpdate()
	{
		if (m_QueuedStats.Count() <= 0)
			return;

		foreach (string playerId, PlayerStats queuedStats : m_QueuedStats)
		{
			string statsPath = m_playerDatabase + "/" + playerId + "/Stats.json";

			PlayerStats savedStats = new PlayerStats();
			if (!FileIO.FileExists(statsPath) || !savedStats.LoadFromFile(statsPath))
			{
				PrintFormat("[StatQueue] ERROR: Could not load stats file for %1", playerId);
				continue;
			}

			savedStats.combatScore     += queuedStats.combatScore;
			savedStats.objectiveScore  += queuedStats.objectiveScore;
			savedStats.logisticsScore  += queuedStats.logisticsScore;
			savedStats.veteranScore    += queuedStats.veteranScore;
			savedStats.intelScore      += queuedStats.intelScore;
			savedStats.playerKills     += queuedStats.playerKills;
			savedStats.playerDeaths    += queuedStats.playerDeaths;
			savedStats.npcKills        += queuedStats.npcKills;
			savedStats.npcDeaths       += queuedStats.npcDeaths;
			savedStats.friendlyKills   += queuedStats.friendlyKills;
			savedStats.friendlyDeaths  += queuedStats.friendlyDeaths;
			savedStats.suicides        += queuedStats.suicides;
			savedStats.captures        += queuedStats.captures;
			savedStats.defends         += queuedStats.defends;
			savedStats.supplies        += queuedStats.supplies;
			savedStats.transport       += queuedStats.transport;
			savedStats.building        += queuedStats.building;
			savedStats.spawnpoint      += queuedStats.spawnpoint;
			savedStats.intelSmall      += queuedStats.intelSmall;
			savedStats.intelMedium     += queuedStats.intelMedium;
			savedStats.intelLarge      += queuedStats.intelLarge;
			savedStats.voteStarted     += queuedStats.voteStarted;
			savedStats.voteAccused     += queuedStats.voteAccused;

			savedStats.Pack();
			savedStats.SaveToFile(statsPath);

			PrintFormat("[StatQueue] Stats for %1 | Kills: P:%2 AI:%3 F:%4 | Deaths: P:%5 AI:%6 F:%7 | Suicides: %8",
			savedStats.ingameName, savedStats.playerKills, savedStats.npcKills, savedStats.friendlyKills,
			savedStats.playerDeaths, savedStats.npcDeaths, savedStats.friendlyDeaths, savedStats.suicides);

			PrintFormat("[StatQueue] Objectives: Captures: %1, Defends: %2 | Logistics: Supplies: %3, Transport: %4, Build: %5, Spawn: %6",
				savedStats.captures, savedStats.defends, savedStats.supplies, savedStats.transport, savedStats.building, savedStats.spawnpoint);
	
			PrintFormat("[StatQueue] Intel: Small: %1, Medium: %2, Large: %3",
				savedStats.intelSmall, savedStats.intelMedium, savedStats.intelLarge);
			
			PrintFormat("[StatQueue] Votekicks: Started: %1, Accused: %2",
				savedStats.voteStarted, savedStats.voteAccused);
		}

		m_QueuedStats.Clear();
	}

	void StatQueue()
	{
		m_QueuedStats = new map<string, ref PlayerStats>();
	}

	static StatQueue GetInstance()
	{
		if (!s_Instance)
			s_Instance = new StatQueue();
		return s_Instance;
	}
}
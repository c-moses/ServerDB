//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode
{
	protected string m_serverDate = string.Empty;
	protected string m_playerDatabase = "$profile:/PlayerDatabase";
	protected string m_playerStatsFilename = "/Stats.json";
	
	protected bool m_isInit = false;
	protected float m_updateTimerInterval = 30.0;
	protected float m_accumulatedTime = 0.0;
	protected float m_offsetTime = 15.0;
	protected StatQueue m_statQueue;
	protected ServerRPCs m_serverRPCs;
	
	StatQueue GetStatQueue()
	{
		return m_statQueue;
	}
	
	ServerRPCs GetServerRPCs()
	{
		return m_serverRPCs;
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		int year, month, day;
		System.GetYearMonthDay(year, month, day);
		m_serverDate = string.Format("%1-%2-%3", year, month, day);
		
		FileIO.MakeDirectory(m_playerDatabase);
		
		m_statQueue = StatQueue.GetInstance();
		m_serverRPCs = ServerRPCs.GetInstance();
		m_isInit = true;
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
	
		if (!m_isInit)
			return;
		
		m_accumulatedTime += timeSlice;
		m_offsetTime += timeSlice;
	
		if (m_accumulatedTime >= m_updateTimerInterval)
		{
			m_accumulatedTime = 0;
			m_statQueue.ScheduledStatUpdate();
		}
	
		if (m_offsetTime >= m_updateTimerInterval)
		{
			m_offsetTime = 15.0;
			m_serverRPCs.ScheduledRPCsUpdate();
		}
	}
	
	void StartGameMode()
	{
		super.StartGameMode();
		if (!m_serverRPCs)
			return;
		
		m_serverRPCs.AddRPC("GameState", "GameStarted");
	}
	
	override void EndGameMode(SCR_GameModeEndData endData)
	{
		super.EndGameMode(endData);
		if (!m_serverRPCs)
			return;
		
		m_serverRPCs.AddRPC("GameState", "GameOver");
	}
	
//------------------------------------------------------------------------------------------------
	
	override void OnPlayerAuditSuccess(int iPlayerID)
	{
		super.OnPlayerAuditSuccess(iPlayerID);
		
		string playerIdentity = GetGame().GetBackendApi().GetPlayerIdentityId(iPlayerID);
		if (playerIdentity.IsEmpty())
			return;
		
		string playerProfile = m_playerDatabase + "/" + playerIdentity;
		FileIO.MakeDirectory(playerProfile);
		
		string playerLoadouts = playerProfile + "/Loadouts";
		FileIO.MakeDirectory(playerLoadouts);
		
		string playerName = GetGame().GetPlayerManager().GetPlayerName(iPlayerID);
		ValidatePlayerStats(playerName, playerProfile);
		
		RecountPlayers();
	}
	
	protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		super.OnPlayerDisconnected(playerId, cause, timeout);
		GetGame().GetCallqueue().CallLater(RecountPlayers, 100, false);
	}
	
	void RecountPlayers()
	{
		if (!m_serverRPCs)
			return;

		int playerCount = GetGame().GetPlayerManager().GetPlayerCount();
		m_serverRPCs.AddRPC("GameState", "PlayerCount", playerCount.ToString());
	}
	
//------------------------------------------------------------------------------------------------
	
	void ValidatePlayerStats(string playerName, string profile)
	{
		string statsPath = profile + m_playerStatsFilename;
		if (!FileIO.FileExists(statsPath))
		{
			CreateDefaultStats(playerName, statsPath);
		}
		else
		{
			ValidateExistingStats(playerName, statsPath);
		}
	}
	
	void ValidateExistingStats(string playerName, string statsPath)
	{
		PlayerStats stats = new PlayerStats();
		bool success = stats.LoadFromFile(statsPath);
		if (success)
		{
			bool needsUpdate = false;

			if (stats.ingameName != playerName)
			{
			    stats.ingameName = playerName;
			    needsUpdate = true;
			}
			
			if (stats.lastPlayed != m_serverDate)
			{
			    stats.lastPlayed = m_serverDate;
			    needsUpdate = true;
			}
			
			if (needsUpdate)
			{
			    stats.Pack();
			    stats.SaveToFile(statsPath);
			}
		}
		else
		{
			CreateDefaultStats(playerName, statsPath);
		}
	}
	
//------------------------------------------------------------------------------------------------
	
	void CreateDefaultStats(string playerName, string statsPath)
	{
		PlayerStats fresh = new PlayerStats();
		fresh.ingameName 		= playerName;
		fresh.lastPlayed 		= m_serverDate;
		fresh.discordId 		= "";
		fresh.combatScore     	= 0;
		fresh.objectiveScore  	= 0;
		fresh.logisticsScore  	= 0;
		fresh.veteranScore    	= 0;
		fresh.intelScore      	= 0;
		fresh.playerKills     	= 0;
		fresh.playerDeaths    	= 0;
		fresh.npcKills        	= 0;
		fresh.npcDeaths       	= 0;
		fresh.friendlyKills   	= 0;
		fresh.friendlyDeaths  	= 0;
		fresh.suicides        	= 0;
		fresh.captures        	= 0;
		fresh.defends         	= 0;
		fresh.supplies        	= 0;
		fresh.transport       	= 0;
		fresh.building        	= 0;
		fresh.spawnpoint      	= 0;
		fresh.intelSmall      	= 0;
		fresh.intelMedium     	= 0;
		fresh.intelLarge      	= 0;
		fresh.voteStarted     	= 0;
		fresh.voteAccused     	= 0;
	
		fresh.Pack();
		fresh.SaveToFile(statsPath);
	}
	
//------------------------------------------------------------------------------------------------
	
	override void OnPlayerKilledEx(notnull SCR_InstigatorContextData instigatorContextData)
	{
		SCR_ECharacterControlType victimType = instigatorContextData.GetVictimCharacterControlType();
		SCR_ECharacterDeathStatusRelations relation = instigatorContextData.GetVictimKillerRelation();
	
		int victimId = instigatorContextData.GetVictimPlayerID();
		string identity = GetGame().GetBackendApi().GetPlayerIdentityId(victimId);
	
		if (identity.IsEmpty())
			return;
	
		switch (relation)
		{
			case SCR_ECharacterDeathStatusRelations.SUICIDE:
				m_statQueue.UpdateStat(identity, StatType.SUICIDES, 1);
				break;
	
			case SCR_ECharacterDeathStatusRelations.KILLED_BY_FRIENDLY_PLAYER:
			case SCR_ECharacterDeathStatusRelations.KILLED_BY_FRIENDLY_AI:
				m_statQueue.UpdateStat(identity, StatType.FRIENDLY_DEATHS, 1);
				break;
	
			case SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER:
				m_statQueue.UpdateStat(identity, StatType.PLAYER_DEATHS, 1);
				break;
	
			case SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_AI:
				m_statQueue.UpdateStat(identity, StatType.NPC_DEATHS, 1);
				break;
		}
	}
	
	override void OnControllableDestroyedEx(notnull SCR_InstigatorContextData instigatorContextData)
	{
		SCR_ECharacterControlType killerType = instigatorContextData.GetKillerCharacterControlType();
		
		if (killerType == SCR_ECharacterControlType.AI)
			return;
	
		SCR_ECharacterControlType victimType = instigatorContextData.GetVictimCharacterControlType();
		SCR_ECharacterDeathStatusRelations relation = instigatorContextData.GetVictimKillerRelation();
	
		int killerId = instigatorContextData.GetKillerPlayerID();
		string identity = GetGame().GetBackendApi().GetPlayerIdentityId(killerId);
	
		if (identity.IsEmpty())
			return;
	
		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_FRIENDLY_PLAYER))
		{
			m_statQueue.UpdateStat(identity, StatType.FRIENDLY_KILLS, 1);
			m_statQueue.UpdateStat(identity, StatType.SCORE_COMBAT, -30);
			return;
		}
	
		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER))
		{
			if (victimType == SCR_ECharacterControlType.PLAYER)
			{
				m_statQueue.UpdateStat(identity, StatType.PLAYER_KILLS, 1);
				m_statQueue.UpdateStat(identity, StatType.SCORE_COMBAT, 50);
			}
			else if (victimType == SCR_ECharacterControlType.AI)
			{
				m_statQueue.UpdateStat(identity, StatType.NPC_KILLS, 1);
				m_statQueue.UpdateStat(identity, StatType.SCORE_COMBAT, 20);
			}
		}
	}
}
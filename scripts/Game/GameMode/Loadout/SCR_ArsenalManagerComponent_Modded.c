modded class SCR_ArsenalManagerComponent
{
	private string m_playerDatabase = "$profile:/PlayerDatabase/"; // {playerIdentity}/Loadouts/{factionKey}.json
	private SCR_FactionManager m_factionManager;
	
	ref map<string, string> m_playerPendingFactionChange = new map<string, string>();
	
//------------------------------------------------------------------------------------------------
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		if (!Replication.IsServer())
			return;
		
		SetEventMask(owner, EntityEvent.INIT);
	};
	
	override void EOnInit(IEntity owner) 
	{
		super.EOnInit(owner);
		m_factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!m_factionManager)
			return;

        m_factionManager.GetOnPlayerFactionChanged_S().Insert(OnFactionChanged);
	}
	
//------------------------------------------------------------------------------------------------
	
	void SaveLoadoutToDB(string identity, string faction, SCR_ArsenalPlayerLoadout playerLoadout)
	{
		string loadoutPath = m_playerDatabase + identity + "/Loadouts/" + faction + ".json";
	
		SCR_JsonSaveContext ctx = new SCR_JsonSaveContext();
		ctx.WriteValue("", playerLoadout);
		ctx.SaveToFile(loadoutPath);
	}
	
	SCR_ArsenalPlayerLoadout LoadLoadoutFromDB(string identity, string faction)
	{
		string loadoutPath = m_playerDatabase + identity + "/Loadouts/" + faction + ".json";
		PrintFormat("[ArsenalManager] Attempting to load loadout from: %1", loadoutPath);
		
		if (!FileIO.FileExists(loadoutPath))
			return null;
	
		SCR_JsonLoadContext ctx = new SCR_JsonLoadContext();
		if (!ctx.LoadFromFile(loadoutPath))
		{
			FileIO.DeleteFile(loadoutPath);
			return null;
		}
	
		SCR_ArsenalPlayerLoadout loadout = new SCR_ArsenalPlayerLoadout();
		if (!ctx.ReadValue("", loadout))
		{
			FileIO.DeleteFile(loadoutPath);
			return null;
		}
	
		m_aPlayerLoadouts.Set(identity, loadout);
		return loadout;
	}
	
//------------------------------------------------------------------------------------------------
	
	void OnFactionChanged(int playerId, SCR_PlayerFactionAffiliationComponent comp, Faction faction)
	{
		if (!m_factionManager)
			return;
	
		int factionIndex = m_factionManager.GetFactionIndex(faction);
		HandleFactionChange(playerId, factionIndex);
	}
	
	void HandleFactionChange(int playerId, int factionIndex)
	{
		PrintFormat("[ArsenalManager] HandleFactionChange: playerId=%1, factionIndex=%2", playerId, factionIndex);
		
		if (!Replication.IsServer() || !m_factionManager)
			return;
	
		Faction faction = m_factionManager.GetFactionByIndex(factionIndex);
		if (!faction)
			return;
	
		string identity;
		if (!GetPlayerIdentity(playerId, identity))
			return;
	
		m_aPlayerLoadouts.Remove(identity);
	
		string factionKey = faction.GetFactionKey();
		if (factionKey.IsEmpty())
		{
			Print("[ArsenalManager] Faction key is empty");
			return;
		}
	
		PrintFormat("[ArsenalManager] Faction key: %1", factionKey);
	
		// Store override
		m_playerPendingFactionChange.Set(identity, factionKey);
	
		SCR_ArsenalPlayerLoadout loadout = LoadLoadoutFromDB(identity, factionKey);
		if (!loadout)
		{
			DoPlayerClearHasLoadout(playerId);
			Rpc(DoPlayerClearHasLoadout, playerId);
			m_playerPendingFactionChange.Remove(identity);
			return;
		}
	
		m_aPlayerLoadouts.Set(identity, loadout);
	
		DoSendPlayerLoadout(playerId, loadout.loadoutData);
		Rpc(DoSendPlayerLoadout, playerId, loadout.loadoutData);
	
		DoSetPlayerHasLoadout(playerId, true, true, true);
		Rpc(DoSetPlayerHasLoadout, playerId, true, true, true);
	
		// ✅ Clear override after use
		m_playerPendingFactionChange.Remove(identity);
	}
	
//------------------------------------------------------------------------------------------------
	
	override protected void OnPlayerAuditSuccess(int playerId)
	{
		super.OnPlayerAuditSuccess(playerId);
		
		string identity;
		if (!GetPlayerIdentity(playerId, identity))
			return;

		SCR_ArsenalPlayerLoadout playerLoadout = m_aPlayerLoadouts.Get(identity);
		if (!playerLoadout || playerLoadout.loadout.IsEmpty())
			return;

		DoSendPlayerLoadout(playerId, playerLoadout.loadoutData);
		Rpc(DoSendPlayerLoadout, playerId, playerLoadout.loadoutData);
		
		DoSetPlayerHasLoadout(playerId, true, false, false);
		Rpc(DoSetPlayerHasLoadout, playerId, true, false, false);
	}
	
	override void DoSetPlayerHasLoadout(int playerId, bool loadoutValid, bool loadoutChanged, bool notification)
	{
		super.DoSetPlayerHasLoadout(playerId, loadoutValid, loadoutChanged, notification);
	
		if (!Replication.IsServer() || !loadoutValid || !loadoutChanged)
			return;
	
		string identity;
		if (!GetPlayerIdentity(playerId, identity))
			return;
	
		string faction;
		if (!m_playerPendingFactionChange.Find(identity, faction))
		{
			if (!GetPlayerFaction(playerId, faction))
				return;
		}
		else
		{
			PrintFormat("[ArsenalManager] Using overridden faction %1 for identity %2", faction, identity);
		}
	
		SCR_ArsenalPlayerLoadout playerLoadout = m_aPlayerLoadouts.Get(identity);
		if (!playerLoadout || playerLoadout.loadoutData == null)
			return;
	
		SaveLoadoutToDB(identity, faction, playerLoadout);
	}

//------------------------------------------------------------------------------------------------

    bool GetPlayerIdentity(int playerId, out string identity)
    {
        BackendApi backendApi = GetGame().GetBackendApi();
        if (!backendApi)
            return false;

        string identityId = backendApi.GetPlayerIdentityId(playerId);
        if (identityId.IsEmpty())
			return false;
		
		identity = identityId;
		return true;
    }
	
	bool GetPlayerFaction(int playerId, out string faction)
	{
		PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerId);
		if (!playerController)
			return false;
		
		IEntity controlledEntity = playerController.GetControlledEntity();
		if (!controlledEntity)
			return false;
		
		FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast(controlledEntity.FindComponent(FactionAffiliationComponent));
		if (!factionComponent)
			return false;
		
		Faction affiliatedFaction = factionComponent.GetAffiliatedFaction();
		if (!affiliatedFaction)
			return false;
		
		string factionKey = affiliatedFaction.GetFactionKey();
		if (factionKey.IsEmpty())
			return false;
		
		faction = factionKey;
		return true;
	}
};
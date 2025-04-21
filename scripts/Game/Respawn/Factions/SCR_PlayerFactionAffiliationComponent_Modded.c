modded class SCR_PlayerFactionAffiliationComponent
{
	private SCR_ArsenalManagerComponent m_arsenalManager;
	
//------------------------------------------------------------------------------------------------
	
	protected override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_arsenalManager = GetArsenalManager();
	}
	
	SCR_ArsenalManagerComponent GetArsenalManager()
	{
		return SCR_ArsenalManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_ArsenalManagerComponent));
	}
	
//------------------------------------------------------------------------------------------------
	
	override void SendRequestFactionResponse_S(int factionIndex, bool response)
	{
		if (response)
		{
			if (!m_arsenalManager)
				m_arsenalManager = GetArsenalManager();

			if (m_arsenalManager)
			{
				int playerId = GetPlayerId();
				m_arsenalManager.HandleFactionChange(playerId, factionIndex);
			}
		}
		
		super.SendRequestFactionResponse_S(factionIndex, response);
	}
};

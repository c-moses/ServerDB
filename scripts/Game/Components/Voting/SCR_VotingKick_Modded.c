modded class SCR_VotingKick : SCR_VotingReferendum
{	
	//------------------------------------------------------------------------------------------------
	override void InitFromTemplate(SCR_VotingBase template, int startingPlayerID, int value, float remainingDuration)
	{
		super.InitFromTemplate(template, startingPlayerID, value, remainingDuration);
		
		SCR_VotingKick votingKickTemplate = SCR_VotingKick.Cast(template);		
		m_bFactionSpecific = votingKickTemplate.isFactionSpecific();

		PlayerManager playerMgr = GetGame().GetPlayerManager();
		if (playerMgr)
		{
			SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			if (!gameMode)
			    return;
			
			ServerRPCs rpcs = gameMode.GetServerRPCs();
			if (!rpcs)
				return;
			
			string voteMsg = playerMgr.GetPlayerName(startingPlayerID) + " started a vote to kick " + playerMgr.GetPlayerName(value);
			rpcs.AddRPC("Events", "Votekick", voteMsg);
			Print(voteMsg);
			
			BackendApi api = GetGame().GetBackendApi();
			if (!api)
				return;
			
			StatQueue stats = gameMode.GetStatQueue();
			if (!stats)
				return;
			
			stats.UpdateStat(api.GetPlayerIdentityId(startingPlayerID), StatType.VOTEKICK_STARTED, 1);
			stats.UpdateStat(api.GetPlayerIdentityId(value), StatType.VOTEKICK_ACCUSED, 1);
		}
	}
}

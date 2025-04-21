modded class SCR_XPHandlerComponent
{
	override void AwardXP(notnull PlayerController controller, SCR_EXPRewards rewardID, float multiplier = 1.0, bool volunteer = false, int customXP = 0)
	{
		super.AwardXP(controller, rewardID, multiplier, volunteer, customXP);
		
		int playerId = controller.GetPlayerId();
		StatQueue queue = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetStatQueue();
		string identity = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		if (!queue || identity == string.Empty)
			return;
		
		int baseXP = GetXPRewardAmount(rewardID);
		int totalXP = Math.Round(baseXP * multiplier * GetXPMultiplier());

		switch (rewardID)
		{
			case SCR_EXPRewards.VETERANCY:
				queue.UpdateStat(identity, StatType.SCORE_VETERAN, totalXP);
				break;

			case SCR_EXPRewards.RELAY_RECONFIGURED:
			case SCR_EXPRewards.BASE_SEIZED:
				queue.UpdateStat(identity, StatType.CAPTURES, 1);
				queue.UpdateStat(identity, StatType.SCORE_OBJECTIVE, totalXP);
				break;

			case SCR_EXPRewards.BASE_DEFENDED:
				queue.UpdateStat(identity, StatType.DEFENDS, 1);
				queue.UpdateStat(identity, StatType.SCORE_OBJECTIVE, totalXP);
				break;

			case SCR_EXPRewards.SUPPLIES_DELIVERED:
				queue.UpdateStat(identity, StatType.SUPPLIES, 1);
				queue.UpdateStat(identity, StatType.SCORE_LOGISTICS, totalXP);
				break;

			case SCR_EXPRewards.TASK_TRANSPORT:
				queue.UpdateStat(identity, StatType.TRANSPORT, 1);
				queue.UpdateStat(identity, StatType.SCORE_LOGISTICS, totalXP);
				break;

			case SCR_EXPRewards.FREE_ROAM_BUILDING_BUILT:
				queue.UpdateStat(identity, StatType.BUILDING, 1);
				queue.UpdateStat(identity, StatType.SCORE_LOGISTICS, totalXP);
				break;

			case SCR_EXPRewards.SPAWN_PROVIDER:
				queue.UpdateStat(identity, StatType.SPAWNPOINT, 1);
				queue.UpdateStat(identity, StatType.SCORE_LOGISTICS, totalXP);
				break;

			case SCR_EXPRewards.VALUABLE_INTEL_HANDIN_SMALL:
				queue.UpdateStat(identity, StatType.INTEL_SMALL, 1);
				queue.UpdateStat(identity, StatType.SCORE_INTEL, totalXP);
				break;

			case SCR_EXPRewards.VALUABLE_INTEL_HANDIN_MEDIUM:
				queue.UpdateStat(identity, StatType.INTEL_MEDIUM, 1);
				queue.UpdateStat(identity, StatType.SCORE_INTEL, totalXP);
				break;

			case SCR_EXPRewards.VALUABLE_INTEL_HANDIN_LARGE:
				queue.UpdateStat(identity, StatType.INTEL_LARGE, 1);
				queue.UpdateStat(identity, StatType.SCORE_INTEL, totalXP);
				break;
		}
	}
}
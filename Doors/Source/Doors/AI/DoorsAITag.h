#pragma once

namespace DoorsAItags
{
	TCHAR const* const noise_tag = TEXT("Noise");
	TCHAR const* const noiseinteractable_tag = TEXT("NoiseInteractable");
}

namespace DoorsBlackboardKeys
{
	//Basic Enemy
	TCHAR const* const playerLocation = TEXT("PlayerLocation");
	TCHAR const* const currentState = TEXT("EnemyState");
	TCHAR const* const canSenseTrigger = TEXT("CanSenseTrigger");
	TCHAR const* const lastKnownLocation = TEXT("LastKnownLocation");
	TCHAR const* const interactibleIndex = TEXT("InteractibleIndex");
	TCHAR const* const interactibleLocation = TEXT("InteractibleLocation");
	TCHAR const* const interactibleObject = TEXT("InteractibleObject");
	TCHAR const* const isPlayerVisible = TEXT("IsPlayerVisible");
	TCHAR const* const waypointRotation = TEXT("WayPointRotation");
	TCHAR const* const waypointIndex = TEXT("WayPointIndex");
	TCHAR const* const waypointLocation = TEXT("WayPointLocation");
	TCHAR const* const waypointWaitTime = TEXT("WayPointWaitTime");
	TCHAR const* const waypointObject = TEXT("WaypointObject");
	TCHAR const* const notFacingWayPoint = TEXT("NotFacingWayPoint");
	TCHAR const* const canSearchInteractables = TEXT("CanSearchInteractables");
	TCHAR const* const playerObj = TEXT("PlayerObj");

}
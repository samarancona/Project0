// Copyright 2020 Dan Kestranek.



#include "Core/Abilities/P0_GameplayEffectTypes.h"

bool FP0_GameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	return Super::NetSerialize(Ar, Map, bOutSuccess) && TargetData.NetSerialize(Ar, Map, bOutSuccess);
}

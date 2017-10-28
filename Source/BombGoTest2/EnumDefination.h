#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class BlockTypeEnum : uint8
{
	BT_StrongHold 	UMETA(DisplayName = "Stronghold"),
	BT_Normal 	UMETA(DisplayName = "Normal")
};

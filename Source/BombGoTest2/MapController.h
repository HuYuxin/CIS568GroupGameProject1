// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapBlock.h"
#include "MapFloorTile.h"
#include "MapController.generated.h"

UCLASS()
class BOMBGOTEST2_API AMapController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TSubclassOf<class AMapBlock> mapBlockClass;
	TSubclassOf<class AMapFloorTile> mapFloorTileClass;
	void buildFloor();
	void placeBlocks();
	
};

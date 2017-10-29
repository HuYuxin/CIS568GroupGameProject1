// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapBlock.h"
#include "MapFloorTile.h"
#include "MapBoundary.h"
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

private:
	const int XSIZE = 16;
	const int YSIZE = 16;
	const int TILESIZE = 100;
	TArray<TArray<AMapBlock*>> blockRecord;
	void initializeMapRecord();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Initialize the game map
	TSubclassOf<class AMapBlock> mapBlockClass;
	TSubclassOf<class AMapFloorTile> mapFloorTileClass;
	TSubclassOf<class AMapBoundary> mapBoundaryClass;
	void buildFloor();
	void placeBlocks();
	void buildBoundary();

	FTimerHandle timerHandle;
	void destroyABlock();
	void destroyABlock(int x, int y);
	
};

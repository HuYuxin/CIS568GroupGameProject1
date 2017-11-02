// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumDefination.h"
#include "SoundDefinitions.h"
#include "Sound/SoundCue.h"
#include "MapBlock.generated.h"

UCLASS()
class BOMBGOTEST2_API AMapBlock : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	AMapBlock();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	int blockResource;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
		BlockTypeEnum blockType;
	//void initBlock(int Type, int Resource);
	void initBlock(BlockTypeEnum Type, int Resource);
	UFUNCTION(BlueprintCallable, Category = "MapBlock")
		void destroyBlock();
	
	TSubclassOf<class AActor> Explosion;
	USoundCue* explosionSoundCue;
};

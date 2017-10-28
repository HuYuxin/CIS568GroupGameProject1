// Fill out your copyright notice in the Description page of Project Settings.

#include "MapBlock.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMapBlock::AMapBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> FindExplosion(TEXT("Blueprint'/Game/ExplosionBP.ExplosionBP'"));
	if (FindExplosion.Object) {
		Explosion = (UClass*)FindExplosion.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> explosionSound(TEXT("SoundCue'/Game/ExplosionSound.ExplosionSound'"));
	if (explosionSound.Object != NULL)
	{
		explosionSoundCue = (USoundCue*)explosionSound.Object;
	}

}

// Called when the game starts or when spawned
void AMapBlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMapBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapBlock::initBlock(BlockTypeEnum Type, int Resource) {
	blockType = Type;
	blockResource = Resource;
}

void AMapBlock::destroyBlock() {
	Destroy();

	UWorld* const World = GetWorld();
	if (World) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		World->SpawnActor<AActor>(Explosion, GetActorLocation(), GetActorRotation(), SpawnParams);
		UGameplayStatics::PlaySoundAtLocation(World, explosionSoundCue, GetActorLocation());
	}
}


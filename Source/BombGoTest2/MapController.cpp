// Fill out your copyright notice in the Description page of Project Settings.

#include "MapController.h"
#include "ConstructorHelpers.h"
#include "PlatformFilemanager.h"
#include "CoreMisc.h"


// Sets default values
AMapController::AMapController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UBlueprint> MapBlock(TEXT("Blueprint'/Game/MapBlockBP.MapBlockBP'"));
	if (MapBlock.Object) {
		mapBlockClass = (UClass*)MapBlock.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> MapFloorTile(TEXT("Blueprint'/Game/MapFloorTileBP.MapFloorTileBP'"));
	if (MapBlock.Object) {
		mapFloorTileClass = (UClass*)MapFloorTile.Object->GeneratedClass;
	}
}

// Called when the game starts or when spawned
void AMapController::BeginPlay()
{
	Super::BeginPlay();
	buildFloor();
	placeBlocks();
	
}

// Called every frame
void AMapController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapController::buildFloor() {
	UWorld* const World = GetWorld();
	if (World) {
		int xSize = 16;
		int ySize = 16;
		int tileSize = 100;
		for (int i = 0; i < xSize * ySize; i++) {
			float x = (i / ySize) * tileSize;
			float y = (i % ySize) * tileSize;
			AMapFloorTile* floorTile = World->SpawnActor<AMapFloorTile>(mapFloorTileClass, FVector(x, y, 0), FRotator(0.f));
		}

	}
}

void AMapController::placeBlocks() {
	UWorld* const World = GetWorld();
	int tileSize = 100;
	if (World) {
		//FString projectDir = FPaths::GameDir();
		//projectDir += "Content/MapData.txt";
		FString projectDir = "C:/Users/Yuxin Hu/Documents/Unreal Projects/BombGoTest2/Content/MapData.txt";
		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*projectDir))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("** Could not Find File **"));
			UE_LOG(LogTemp, Warning, TEXT("** Could not Find File **"));
			return;
		}
		TArray<FString> StringArray;
		UE_LOG(LogTemp, Warning, TEXT("text info: %s"), *(projectDir));
		bool fileReadSuccess = FFileHelper::LoadANSITextFileToStrings(*(projectDir), NULL, StringArray);
		if (fileReadSuccess) {
			int ElementSize = StringArray.Num();
			for (int i = 0; i < ElementSize; i++) {
				FString data = StringArray[i];
				TArray<FString> xyPos;
				data.ParseIntoArray(xyPos, TEXT(","), false);
				int xShift = FCString::Atoi(*(xyPos[0]));
				int yShift = FCString::Atoi(*(xyPos[1]));
				AMapBlock* block = (AMapBlock*) World->SpawnActor<AMapBlock>(mapBlockClass, FVector(xShift * tileSize, yShift * tileSize, 0), FRotator(0.f));
				
				//Assign a color to static mesh component (which is a block)
				TArray<UStaticMeshComponent*> staticMeshComponents;
				block->GetComponents<UStaticMeshComponent>(staticMeshComponents);
				UStaticMeshComponent* component = staticMeshComponents[0];
				UMaterialInstanceDynamic * DynamicMaterial = UMaterialInstanceDynamic::Create(component->GetMaterial(0), nullptr);
				DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Blue);
				component->SetMaterial(0, DynamicMaterial);
				//Assign a color to static mesh component
			}
		}
	}
}




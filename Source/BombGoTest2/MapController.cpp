// Fill out your copyright notice in the Description page of Project Settings.

#include "MapController.h"
#include "ConstructorHelpers.h"
#include "PlatformFilemanager.h"
#include "Kismet/GameplayStatics.h"
#include "CoreMisc.h"
#include "UnrealNetwork.h"


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
	static ConstructorHelpers::FObjectFinder<UBlueprint> MapBoundary(TEXT("Blueprint'/Game/MapBoundaryBP.MapBoundaryBP'"));
	if (MapBoundary.Object) {
		mapBoundaryClass = (UClass*)MapBoundary.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> Stronghold(TEXT("Blueprint'/Game/StrongholdBP.StrongholdBP'"));
	if (Stronghold.Object) {
		strongholdClass = (UClass*)Stronghold.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> FindExplosion(TEXT("Blueprint'/Game/ExplosionBP.ExplosionBP'"));
	if (FindExplosion.Object) {
		Explosion = (UClass*)FindExplosion.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> explosionSound(TEXT("SoundCue'/Game/ExplosionSound.ExplosionSound'"));
	if (explosionSound.Object != NULL)
	{
		explosionSoundCue = (USoundCue*)explosionSound.Object;
	}
	//initializeMapRecord();
}
//void AMapController::initializeMapRecord_Implementation() {
//	if (Role != ROLE_Authority) return;
//	for (int row = 0; row < YSIZE; row++) {
//		TArray<AMapBlock*> StringArray;
//		for (int col = 0; col < XSIZE; col++) {
//			StringArray.Add(NULL);
//		}
//		blockRecord.Add(StringArray);
//	}
//}
//bool AMapController::initializeMapRecord_Validate() {
//	return true;
//}

// Called when the game starts or when spawned
void AMapController::BeginPlay()
{
	Super::BeginPlay();	
	buildFloor();
	placeBlocks();
	buildBoundary();
	//GetWorldTimerManager().SetTimer(timerHandle, this, &AMapController::destroyABlock, 5.0f, false);
}

// Called every frame
void AMapController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapController::buildFloor_Implementation() {
	if (Role != ROLE_Authority)
		return;

	UWorld* const World = GetWorld();
	if (World) {
		for (int i = 0; i < XSIZE * YSIZE; i++) {
			float x = (i / YSIZE) * TILESIZE;
			float y = (i % YSIZE) * TILESIZE;
			AMapFloorTile* floorTile = World->SpawnActor<AMapFloorTile>(mapFloorTileClass, FVector(x, y, 0), FRotator(0.f));
		}
	}
}
bool AMapController::buildFloor_Validate() {
	return true;
}
void AMapController::placeBlocks_Implementation() {
	if (Role != ROLE_Authority) return;
	UWorld* const World = GetWorld();
	if (World) {
		//FString projectDir = FPaths::GameDir();
		//projectDir += "Content/MapData.txt";
		FString dataMapFilePath = FPaths::ConvertRelativePathToFull(FPaths::GameDir());
		FString projectDir = dataMapFilePath+"Content/MapData.txt";
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
				
				
				if (xyPos.Num() > 2) {
					//this is a stronghold
					int idx= FCString::Atoi(*(xyPos[2]));
					AStronghold* stronghold = (AStronghold*)World->SpawnActor<AStronghold>(strongholdClass, FVector(xShift * TILESIZE, yShift * TILESIZE, 0), FRotator(0.f));
					FLinearColor blockColor = FLinearColor();
					stronghold->id = idx;
					if (xShift > 8) {
						blockColor = FLinearColor::Red;
					}
					else {
						blockColor = FLinearColor::Blue;
					}
					TArray<UStaticMeshComponent*> staticMeshComponents;
					stronghold->GetComponents<UStaticMeshComponent>(staticMeshComponents);
					UStaticMeshComponent* component = staticMeshComponents[0];
					UMaterialInstanceDynamic * DynamicMaterial = UMaterialInstanceDynamic::Create(component->GetMaterial(0), nullptr);
					DynamicMaterial->SetVectorParameterValue("Color", blockColor);
					component->SetMaterial(0, DynamicMaterial);
				}
				else {
					//this is a normal block
					AMapBlock* block = (AMapBlock*)World->SpawnActor<AMapBlock>(mapBlockClass, FVector(xShift * TILESIZE, yShift * TILESIZE, 0), FRotator(0.f));
					block->initBlock(BlockTypeEnum::BT_Normal, 0);
				}

				//Assign a color to static mesh component (which is a block)

				//Add this pointer to the 2D blockMap
				//blockRecord[yShift][xShift] = block;
			}
		}
	}
}
bool AMapController::placeBlocks_Validate() {
	return true;
}
void AMapController::buildBoundary_Implementation() {
	if (Role != ROLE_Authority) return;
	UWorld* const World = GetWorld();
	if (World) {
		for (int i = 0; i < XSIZE; i++) {
			float y = i * TILESIZE;
			AMapBoundary* floorTile = World->SpawnActor<AMapBoundary>(mapBoundaryClass, FVector(-TILESIZE/2, y, TILESIZE / 2), FRotator(90.0f, 0, 0));
		}
		for (int i = 0; i < XSIZE; i++) {
			float y = i * TILESIZE;
			AMapBoundary* floorTile = World->SpawnActor<AMapBoundary>(mapBoundaryClass, FVector((XSIZE-1+0.5)*TILESIZE, y, TILESIZE / 2), FRotator(-90.0f, 0, 0));
		}
		for (int i = 0; i < XSIZE; i++) {
			float x = i * TILESIZE;
			AMapBoundary* floorTile = World->SpawnActor<AMapBoundary>(mapBoundaryClass, FVector(x, -TILESIZE / 2, TILESIZE / 2), FRotator(0, 0, -90.0f));
		}
		for (int i = 0; i < XSIZE; i++) {
			float x = i * TILESIZE;
			AMapBoundary* floorTile = World->SpawnActor<AMapBoundary>(mapBoundaryClass, FVector(x, (YSIZE - 1 + 0.5)*TILESIZE, TILESIZE / 2), FRotator(0, 0, 90.0f));
		}
	}
}
bool AMapController::buildBoundary_Validate() {
	return true;
}

/*void AMapController::destroyABlock() {
	int x = 13;
	int y = 15;
	if (blockRecord[y][x] != NULL) {
		blockRecord[y][x]->destroyBlock();
	}
}*/

//bool AMapController::inBound(int x, int y) {
//	if ((x < XSIZE && x >= 0) && (y < YSIZE && y >= 0)) {
//		return true;
//	}else{
//		return false;
//	}
//}

//void AMapController::makeAnExplosion(int xGrid, int yGrid) {
//	UWorld* const World = GetWorld();
//	if (World) {
//		FActorSpawnParameters SpawnParams;
//		SpawnParams.Owner = this;
//		SpawnParams.Instigator = Instigator;
//		World->SpawnActor<AActor>(Explosion, FVector(xGrid*TILESIZE, yGrid *TILESIZE, 0), FRotator(0, 0, 0), SpawnParams);
//		UGameplayStatics::PlaySoundAtLocation(World, explosionSoundCue, FVector(xGrid*TILESIZE, yGrid *TILESIZE, 0));
//	}
//}

//void AMapController::destroyABlock(float xPos, float yPos) {
//	int xGrid = (xPos + TILESIZE/2) / TILESIZE;
//	int yGrid = (yPos + TILESIZE/2) / TILESIZE;
//	
//	//xGrid represents which column, yGrid represents which row
//	//grid below bomb
//	if (inBound(xGrid, yGrid + 1)) {
//		if (IsValid(blockRecord[yGrid + 1][xGrid])) {
//			blockRecord[yGrid + 1][xGrid]->destroyBlock();
//			blockRecord[yGrid + 1][xGrid] = NULL;
//		}
//		else {
//			//emit an explosion class
//			makeAnExplosion(xGrid, yGrid + 1);
//		}
//	}
//	
//	//grid above bomb
//	if (inBound(xGrid, yGrid - 1)) {
//		if (IsValid(blockRecord[yGrid - 1][xGrid])) {
//			blockRecord[yGrid - 1][xGrid]->destroyBlock();
//			blockRecord[yGrid - 1][xGrid] = NULL;
//		}
//		else {
//			//emit an explosion class
//			makeAnExplosion(xGrid, yGrid - 1);
//		}
//	}
//
//	//grid on left of bomb
//	if (inBound(xGrid - 1, yGrid)) {
//		if (IsValid(blockRecord[yGrid][xGrid - 1])) {
//			blockRecord[yGrid][xGrid - 1]->destroyBlock();
//			blockRecord[yGrid][xGrid - 1] = NULL;
//		}
//		else {
//			//emit an explosion class
//			makeAnExplosion(xGrid-1, yGrid);
//		}
//	}
//
//	//grid on right of bomb
//	if (inBound(xGrid + 1, yGrid)) {
//		if (IsValid(blockRecord[yGrid][xGrid + 1])) {
//			blockRecord[yGrid][xGrid + 1]->destroyBlock();
//			blockRecord[yGrid][xGrid + 1] = NULL;
//		}
//		else {
//			//emit an explosion class
//			makeAnExplosion(xGrid + 1, yGrid);
//		}
//	}
//}



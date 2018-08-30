// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "SpatialGDKEditorGenerateSnapshot.h"
#include "SpatialConstants.h"
#include "SpatialGDKEditorToolbarSettings.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "UObjectIterator.h"
#include <improbable/c_worker.h>
#include <improbable/c_schema.h>

DEFINE_LOG_CATEGORY(LogSpatialGDKSnapshot);

namespace
{

//using PathNameToEntityIdMap = worker::Map<std::string, worker::EntityId>;
//
//const WorkerAttributeSet UnrealWorkerAttributeSet{worker::List<std::string>{"UnrealWorker"}};
//const WorkerAttributeSet UnrealClientAttributeSet{worker::List<std::string>{"UnrealClient"}};
//
//const WorkerRequirementSet UnrealWorkerWritePermission{{UnrealWorkerAttributeSet}};
//const WorkerRequirementSet UnrealClientWritePermission{{UnrealClientAttributeSet}};
//const WorkerRequirementSet AnyWorkerReadPermission{{UnrealClientAttributeSet, UnrealWorkerAttributeSet}};
//
//// RTTB
//const WorkerAttributeSet CAPIClientAttributeSet{worker::List<std::string>{"CAPIClient"}};
//const WorkerAttributeSet CAPIWorkerAttributeSet{worker::List<std::string>{"CAPIWorker"}};
//const WorkerRequirementSet CAPIWorkerPermission{{CAPIWorkerAttributeSet}};
//const WorkerRequirementSet CAPIPermission{{CAPIClientAttributeSet, CAPIWorkerAttributeSet}};
//
//const Coordinates Origin{0, 0, 0};

//Worker_Entity CreateSpawnerEntity()
//{
	//Worker_Entity SpawnerEntity;
	//SpawnerEntity.entity_id = SpatialConstants::SPAWNER_ENTITY_ID;

	//return improbable::unreal::FEntityBuilder::Begin()
	//	.AddPositionComponent(Position::Data{Origin}, UnrealWorkerWritePermission)
	//	.AddMetadataComponent(Metadata::Data("SpatialSpawner"))
	//	.SetPersistence(true)
	//	.SetReadAcl(AnyWorkerReadPermission)
	//	.AddComponent<unreal::PlayerSpawner>(unreal::PlayerSpawner::Data{}, UnrealWorkerWritePermission)
	//	.AddComponent<improbable::unreal::UnrealMetadata>(UnrealMetadata, UnrealWorkerWritePermission)
	//	.Build();
//}

//worker::Entity CreateSpecialSpawner()
//{
//	improbable::unreal::UnrealMetadata::Data UnrealMetadata;
//
//	return improbable::unreal::FEntityBuilder::Begin()
//		.AddPositionComponent(Position::Data{Origin}, CAPIWorkerPermission)
//		.AddMetadataComponent(Metadata::Data("SpecialSpawner"))
//		.SetPersistence(true)
//		.SetReadAcl(CAPIPermission)
//		.AddComponent<unreal::SpecialSpawner>(unreal::SpecialSpawner::Data{}, CAPIWorkerPermission)
//		.AddComponent<improbable::unreal::UnrealMetadata>(UnrealMetadata, CAPIWorkerPermission)
//		.Build();
//}

//worker::Map<worker::EntityId, worker::Entity> CreateLevelEntities(UWorld* World)
//{
//	worker::Map<worker::EntityId, worker::Entity> LevelEntities;
//
//	// Set up grid of "placeholder" entities to allow workers to be authoritative over _something_.
//	int PlaceholderCount = SpatialConstants::PLACEHOLDER_ENTITY_ID_LAST - SpatialConstants::PLACEHOLDER_ENTITY_ID_FIRST + 1;
//	int PlaceholderCountAxis = sqrt(PlaceholderCount);
//	checkf(PlaceholderCountAxis * PlaceholderCountAxis == PlaceholderCount, TEXT("The number of placeholders must be a square number."));
//	checkf(PlaceholderCountAxis % 2 == 0, TEXT("The number of placeholders on each axis must be even."));
//	const float CHUNK_SIZE = 5.0f; // in SpatialOS coordinates.
//	int PlaceholderEntityIdCounter = SpatialConstants::PLACEHOLDER_ENTITY_ID_FIRST;
//	for (int x = -PlaceholderCountAxis / 2; x < PlaceholderCountAxis / 2; x++)
//	{
//		for (int y = -PlaceholderCountAxis / 2; y < PlaceholderCountAxis / 2; y++)
//		{
//			const Coordinates PlaceholderPosition{x * CHUNK_SIZE + CHUNK_SIZE * 0.5f, 0, y * CHUNK_SIZE + CHUNK_SIZE * 0.5f};
//			LevelEntities.emplace(PlaceholderEntityIdCounter, improbable::unreal::FEntityBuilder::Begin()
//				.AddPositionComponent(Position::Data{PlaceholderPosition}, UnrealWorkerWritePermission)
//				.AddMetadataComponent(Metadata::Data("Placeholder"))
//				.SetPersistence(true)
//				.SetReadAcl(AnyWorkerReadPermission)
//				.AddComponent<unreal::UnrealLevelPlaceholder>(unreal::UnrealLevelPlaceholder::Data{}, UnrealWorkerWritePermission)
//				.Build());
//			PlaceholderEntityIdCounter++;
//		}
//	}
//	// Sanity check.
//	check(PlaceholderEntityIdCounter == SpatialConstants::PLACEHOLDER_ENTITY_ID_LAST + 1);
//	return LevelEntities;
//}

//bool CreateSingletonToIdMap(PathNameToEntityIdMap& SingletonNameToEntityId)
//{
//	for (TObjectIterator<UClass> It; It; ++It)
//	{
//		// Find all singleton classes
//		if (It->HasAnySpatialClassFlags(SPATIALCLASS_Singleton) == false)
//		{
//			continue;
//		}
//
//		// Ensure we don't process skeleton or reinitialised classes
//		if (	It->GetName().StartsWith(TEXT("SKEL_"), ESearchCase::CaseSensitive)
//			||	It->GetName().StartsWith(TEXT("REINST_"), ESearchCase::CaseSensitive))
//		{
//			continue;
//		}
//
//		// Id is initially 0 to indicate that this Singleton entity has not been created yet.
//		// When the worker authoritative over the GSM sees 0, it knows it is safe to create it.
//		SingletonNameToEntityId.emplace(std::string(TCHAR_TO_UTF8(*It->GetPathName())), 0);
//	}
//
//	return true;
//}
//
//worker::Entity CreateGlobalStateManagerEntity(const PathNameToEntityIdMap& SingletonNameToEntityId)
//{
//	return improbable::unreal::FEntityBuilder::Begin()
//		.AddPositionComponent(Position::Data{Origin}, UnrealWorkerWritePermission)
//		.AddMetadataComponent(Metadata::Data("GlobalStateManager"))
//		.SetPersistence(true)
//		.SetReadAcl(AnyWorkerReadPermission)
//		.AddComponent<unreal::GlobalStateManager>(unreal::GlobalStateManager::Data{ SingletonNameToEntityId, {} }, UnrealWorkerWritePermission)
//		.AddComponent<improbable::unreal::UnrealMetadata>(improbable::unreal::UnrealMetadata::Data{}, UnrealWorkerWritePermission)
//		.Build();
//}

} // ::

//bool ValidateAndCreateSnapshotGenerationPath(FString& SavePath)
//{
//	FString DirectoryPath = FPaths::GetPath(SavePath);
//	if (!FPaths::CollapseRelativeDirectories(DirectoryPath))
//	{
//		UE_LOG(LogSpatialGDKSnapshot, Error, TEXT("Invalid path: %s - snapshot not generated"), *DirectoryPath);
//		return false;
//	}
//
//	if (!FPaths::DirectoryExists(DirectoryPath))
//	{
//		UE_LOG(LogSpatialGDKSnapshot, Display, TEXT("Snapshot directory does not exist - creating directory: %s"), *DirectoryPath);
//		if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*DirectoryPath))
//		{
//			UE_LOG(LogSpatialGDKSnapshot, Error, TEXT("Unable to create directory: %s - snapshot not generated"), *DirectoryPath);
//			return false;
//		}
//	}
//	return true;
//}

bool SpatialGDKGenerateSnapshot(UWorld* World)
{
	//const USpatialGDKEditorToolbarSettings* Settings = GetDefault<USpatialGDKEditorToolbarSettings>();
	//FString SavePath = FPaths::Combine(Settings->GetSpatialOSSnapshotPath(), Settings->GetSpatialOSSnapshotFile());
	//if (!ValidateAndCreateSnapshotGenerationPath(SavePath))
	//{
	//	return false;
	//}

	//UE_LOG(LogSpatialGDKSnapshot, Display, TEXT("Saving snapshot to: %s"), *SavePath);
	//worker::SnapshotOutputStream OutputStream = worker::SnapshotOutputStream(improbable::unreal::Components{}, TCHAR_TO_UTF8(*SavePath));

	//// Create spawner entity.
	//worker::Option<std::string> Result = OutputStream.WriteEntity(SpatialConstants::SPAWNER_ENTITY_ID, CreateSpawnerEntity());

	//if (!Result.empty())
	//{
	//	UE_LOG(LogSpatialGDKSnapshot, Error, TEXT("Error generating snapshot: %s"), UTF8_TO_TCHAR(Result.value_or("").c_str()));
	//	return false;
	//}

	//// Create Global State Manager
	//PathNameToEntityIdMap SingletonNameToEntityId;
	//if(!CreateSingletonToIdMap(SingletonNameToEntityId))
	//{
	//	UE_LOG(LogSpatialGDKSnapshot, Error, TEXT("Error generating snapshot: Couldn't create Singleton Name to EntityId map"));
	//	return false;
	//}

	//Result = OutputStream.WriteEntity(SpatialConstants::GLOBAL_STATE_MANAGER, CreateGlobalStateManagerEntity(SingletonNameToEntityId));
	//if (!Result.empty())
	//{
	//	UE_LOG(LogSpatialGDKSnapshot, Error, TEXT("Error generating snapshot: %s"), UTF8_TO_TCHAR(Result.value_or("").c_str()));
	//	return false;
	//}

	//Result = OutputStream.WriteEntity(SpatialConstants::SPECIAL_SPAWNER, CreateSpecialSpawner());

	//if (!Result.empty())
	//{
	//	UE_LOG(LogSpatialGDKSnapshot, Error, TEXT("Error generating snapshot: %s"), UTF8_TO_TCHAR(Result.value_or("").c_str()));
	//	return false;
	//}

	//// Create level entities.
	//for (const auto& EntityPair : CreateLevelEntities(World))
	//{
	//	Result = OutputStream.WriteEntity(EntityPair.first, EntityPair.second);
	//	if (!Result.empty())
	//	{
	//		UE_LOG(LogSpatialGDKSnapshot, Error, TEXT("Error generating snapshot: %s"), UTF8_TO_TCHAR(Result.value_or("").c_str()));
	//		return false;
	//	}
	//}

	//UE_LOG(LogSpatialGDKSnapshot, Display, TEXT("Snapshot exported to the path: %s"), *SavePath);
	return true;
}

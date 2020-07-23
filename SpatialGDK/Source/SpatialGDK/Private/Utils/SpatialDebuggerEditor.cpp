#include "Utils/SpatialDebuggerEditor.h"

#include "LoadBalancing/GridBasedLBStrategy.h"
#include "LoadBalancing/LayeredLBStrategy.h"
#include "Interop/Connection/SpatialWorkerConnection.h"
#include "SpatialCommonTypes.h"
#include "Utils/InspectionColors.h"
#include "Debug/DebugDrawService.h"
#include "Editor.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "EngineClasses/SpatialWorldSettings.h"

using namespace SpatialGDK;

ASpatialDebuggerEditor::ASpatialDebuggerEditor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = false;
}

void ASpatialDebuggerEditor::Destroyed()
{
	DestroyWorkerRegions();
}

void ASpatialDebuggerEditor::ShowWorkerRegions(bool bEnabled)
{
	bShowWorkerRegions = bEnabled;

	RefreshWorkerRegions();
}


void ASpatialDebuggerEditor::RefreshWorkerRegions()
{
	DestroyWorkerRegions();

	if (bShowWorkerRegions && AllowWorkerBoundaries())
	{
		InitialiseWorkerRegions();
		CreateWorkerRegions();
	}

	if (GEditor != nullptr && GEditor->GetActiveViewport() != nullptr)
	{
		// Redraw editor window to show changes
		GEditor->GetActiveViewport()->Invalidate();
	}
}

bool ASpatialDebuggerEditor::AllowWorkerBoundaries() const
{
	// Check if multi worker is enabled.
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return false;
	}
		
	const ASpatialWorldSettings* WorldSettings = Cast<ASpatialWorldSettings>(World->GetWorldSettings());

	const bool bIsMultiWorkerEnabled = WorldSettings != nullptr && WorldSettings->IsMultiWorkerEnabled();

	return bIsMultiWorkerEnabled;
}

void ASpatialDebuggerEditor::InitialiseWorkerRegions()
{
	ULayeredLBStrategy* LoadBalanceStrategy = NewObject<ULayeredLBStrategy>();
	LoadBalanceStrategy->Init(GetWorld());
	LoadBalanceStrategy->SetVirtualWorkerIds(1, LoadBalanceStrategy->GetMinimumRequiredWorkers());

	const ULayeredLBStrategy* LayeredLBStrategy = Cast<ULayeredLBStrategy>(LoadBalanceStrategy);

	if (const UGridBasedLBStrategy* GridBasedLBStrategy = Cast<UGridBasedLBStrategy>(LayeredLBStrategy->GetLBStrategyForVisualRendering()))
	{
		const UGridBasedLBStrategy::LBStrategyRegions LBStrategyRegions = GridBasedLBStrategy->GetLBStrategyRegions();

		// Only show worker regions if there is more than one
		if (LBStrategyRegions.Num() > 1)
		{
				WorkerRegions.SetNum(LBStrategyRegions.Num());
				for (int i = 0; i < LBStrategyRegions.Num(); i++)
				{
					const TPair<VirtualWorkerId, FBox2D>& LBStrategyRegion = LBStrategyRegions[i];
					FWorkerRegionInfo WorkerRegionInfo;
					// Generate our own unique worker name as we only need it to generate a unique colour
					const PhysicalWorkerName WorkerName = PhysicalWorkerName::Printf(TEXT("WorkerRegion%d"), i);
					WorkerRegionInfo.Color = GetColorForWorkerName(WorkerName);
					WorkerRegionInfo.Extents = LBStrategyRegion.Value;

					WorkerRegions[i] = WorkerRegionInfo;
				}
		}
		else
		{
			WorkerRegions.Empty();
		}
	}
}

#include "EventProcessor.h"

#include "GameFramework/Actor.h"
#include "Utils/SpatialStatics.h"

#include "Schema/RPCPayload.h"
#include "StructuredEventBuilder.h"

void GDKEventsToStructuredLogs::ReceiveAddEntity(AActor* Actor, const Worker_EntityId& EntityId)
{
	FAddEntityEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FAddEntityEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, EntityId);

	Logger->LogEvent(Event);
}

void GDKEventsToStructuredLogs::ReceiveRemoveEntity(AActor* Actor, const Worker_EntityId& EntityId)
{
	FRemoveEntityEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FRemoveEntityEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, EntityId);

	Logger->LogEvent(Event);
} //actor might be null

void GDKEventsToStructuredLogs::ReceiveAuthorityChange(AActor* Actor, ENetRole NewRole)
{
	FAuthorityChangeEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FAuthorityChangeEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, USpatialStatics::GetActorEntityId(Actor));
	Event.Authority = (NewRole == ENetRole::ROLE_Authority) ? "Authoritative" : "Non-Authoritative";

	Logger->LogEvent(Event);
}
void GDKEventsToStructuredLogs::ReceiveComponentUpdate(AActor* Actor, UObject* SubObject, Worker_ComponentId ComponentId)
{
	FUpdateComponentEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FUpdateComponentEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, USpatialStatics::GetActorEntityId(Actor));
	Event.Subobject = StructuredEventBuilder::ConstructSubobjectData(SubObject);
	Event.Component.Id = ComponentId;

	Logger->LogEvent(Event);
}

void GDKEventsToStructuredLogs::ReceiveCommandRequest(AActor* Actor, UObject* SubObject, UFunction* Function, TraceKey TraceId, Worker_RequestId LocalRequestId)
{
	FRPCRequestEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FRPCRequestEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, USpatialStatics::GetActorEntityId(Actor));
	Event.Subobject = StructuredEventBuilder::ConstructSubobjectData(SubObject);
	Event.Rpc = StructuredEventBuilder::ConstructUserRPCData(Function, TraceId, LocalRequestId, TEXT(""), 0, 0);

	Logger->LogEvent(Event);
}

void GDKEventsToStructuredLogs::ReceiveNamedCommandRequest(const FString& CommandName, Worker_RequestId LocalRequestId)
{
	FRPCRequestEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FRPCRequestEvent>();
	//todo: some meaningful actor data? make separate command type?
	Event.Rpc = StructuredEventBuilder::ConstructGdkRPCData(CommandName, LocalRequestId, FString(), 0, 0);

	Logger->LogEvent(Event);
}
void GDKEventsToStructuredLogs::ReceiveCommandResponse(AActor* Actor, UObject* SubObject, UFunction* Function, const Worker_CommandResponseOp& Op)
{
	FRPCResponseEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FRPCResponseEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, USpatialStatics::GetActorEntityId(Actor));
	Event.Subobject = StructuredEventBuilder::ConstructSubobjectData(SubObject);
	Event.Rpc = StructuredEventBuilder::ConstructUserRPCData(Function, InvalidTraceKey, Op.request_id, FString(Op.message), Op.status_code, 0);
	
	Logger->LogEvent(Event);
} // pointers can be null
void GDKEventsToStructuredLogs::ReceiveNamedCommandResponse(const FString& CommandName, const Worker_CommandResponseOp& Op)
{
	FRPCResponseEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FRPCResponseEvent>();
	//todo: some meaningful actor data? make separate command type?
	Event.Rpc = StructuredEventBuilder::ConstructGdkRPCData(CommandName, Op.request_id, FString(Op.message), Op.status_code, 0);

	Logger->LogEvent(Event);
}

void GDKEventsToStructuredLogs::ReceiveCreateEntityResponse(AActor* Actor, const FString& ApplicationError, const Worker_CreateEntityResponseOp& Op)
{
	FRPCResponseEvent Event = StructuredEventBuilder::ConstructNetReceiveEvent<FRPCResponseEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, Op.entity_id);
	Event.Rpc = StructuredEventBuilder::ConstructGdkRPCData(TEXT("CREATE_ENTITY"), Op.request_id, FString(Op.message), Op.status_code, 0);

	Logger->LogEvent(Event);
}

void GDKEventsToStructuredLogs::SendCreateEntity(AActor* Actor, const Worker_RequestId& CreateEntityRequestId)
{
	FRPCRequestEvent Event = StructuredEventBuilder::ConstructNetSendEvent<FRPCRequestEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, USpatialStatics::GetActorEntityId(Actor));
	Event.Rpc.LocalRequestId = CreateEntityRequestId;
	Event.Rpc.Name = TEXT("CREATE_ENTITY");
	Event.Rpc.Type = TEXT("SYSTEM");

	Logger->LogEvent(Event);
}
void GDKEventsToStructuredLogs::SendDeleteEntity(AActor* Actor, Worker_EntityId EntityId, const Worker_RequestId& DeleteEntityRequestId)
{
	FRPCRequestEvent Event = StructuredEventBuilder::ConstructNetSendEvent<FRPCRequestEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, EntityId); //EntityId manually fed as mapping somehow might already have been removed (even though the Id->Actor mapping was just fetched in calling code)
	Event.Rpc.LocalRequestId = DeleteEntityRequestId;
	Event.Rpc.Name = TEXT("DELETE_ENTITY");
	Event.Rpc.Type = TEXT("SYSTEM");

	Logger->LogEvent(Event);
}

void GDKEventsToStructuredLogs::SendAuthorityIntentUpdate(const AActor& Actor, VirtualWorkerId NewAuthoritativeVirtualWorkerId)
{
	FAuthorityIntentChangeEvent Event = StructuredEventBuilder::ConstructNetSendEvent<FAuthorityIntentChangeEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(&Actor, USpatialStatics::GetActorEntityId(&Actor));
	Event.NewIntendedAuthority = NewAuthoritativeVirtualWorkerId;

	Logger->LogEvent(Event);
}

void GDKEventsToStructuredLogs::SendRPC(AActor* Actor, UFunction* Function, TraceKey TraceId, Worker_RequestId LocalRequestId)
{
	FRPCRequestEvent Event = StructuredEventBuilder::ConstructNetSendEvent<FRPCRequestEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, USpatialStatics::GetActorEntityId(Actor));
	//todo: reintroduce - Event.Subobject = LogEvents::ConstructSubobjectData(SubObject);
	Event.Rpc = StructuredEventBuilder::ConstructUserRPCData(Function, TraceId, LocalRequestId, FString(), 0, 0);

	Logger->LogEvent(Event);
}
void GDKEventsToStructuredLogs::SendRPCRetry(AActor* Actor, UFunction* Function, int AttemptNumber, Worker_RequestId LocalRequestId)
{
	FRPCRequestEvent Event = StructuredEventBuilder::ConstructNetSendEvent<FRPCRequestEvent>();
	Event.Actor = StructuredEventBuilder::ConstructActorData(Actor, USpatialStatics::GetActorEntityId(Actor));
	//todo: reintroduce - Event.Subobject = LogEvents::ConstructSubobjectData(SubObject);
	Event.Rpc = StructuredEventBuilder::ConstructUserRPCData(Function, InvalidTraceKey, LocalRequestId, FString(), 0, AttemptNumber); //todo: can we get payload / tracekey?
	
	Logger->LogEvent(Event);
}

void GDKEventsToStructuredLogs::SendCommandResponse(Worker_RequestId LocalRequestId, bool success)
{
	FRPCResponseEvent Event = StructuredEventBuilder::ConstructNetSendEvent<FRPCResponseEvent>();
	// todo: can we get actor, subobject, RPCPayload?
	// Event.Actor = LogEvents::ConstructActorData(Actor, USpatialStatics::GetActorEntityId(Actor));
	// Event.Subobject = LogEvents::ConstructSubobjectData(SubObject);
	Event.Rpc = StructuredEventBuilder::ConstructUserRPCData(nullptr, InvalidTraceKey, LocalRequestId, FString(), success ? WORKER_STATUS_CODE_SUCCESS : WORKER_STATUS_CODE_APPLICATION_ERROR, 0);

	Logger->LogEvent(Event);
}

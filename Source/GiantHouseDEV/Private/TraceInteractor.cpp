// Fill out your copyright notice in the Description page of Project Settings.

#include "TraceInteractor.h"
#include "Engine/Engine.h"
#include "Public/ITraceInteractions.h"
#include "Public/LookAtTarget.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UTraceInteractor::UTraceInteractor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTraceInteractor::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UTraceInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Trace();
	// ...
}

void UTraceInteractor::Trace()
{
	FHitResult outHit;

	FVector start = GetComponentLocation();
	FVector end = ((GetForwardVector() * 1000.f) + start);
	FCollisionQueryParams CollisionParams;

	GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, CollisionParams);
	//DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 0.f,0, 2.f);

	if (outHit.GetActor() == nullptr) //We are not tracing an actor
	{
		if (pHitActor != nullptr)
		{
			IITraceInteractions::Execute_OnTraceEnd(pHitActor, this);
			pHitActor = nullptr;
		}
		return;
	}

	if (!outHit.GetActor()->GetClass()->ImplementsInterface(UITraceInteractions::StaticClass()))
	{
		if (pHitActor != nullptr)
		{
			IITraceInteractions::Execute_OnTraceEnd(pHitActor, this);
			pHitActor = nullptr;
		}
		return;
	}

	if (pHitActor != outHit.GetActor()) //We found a new actor so end trace on the old actor
	{
		if (pHitActor != nullptr)
			IITraceInteractions::Execute_OnTraceEnd(pHitActor, this);

		pHitActor = outHit.GetActor();

		if (pHitActor != nullptr) //STart trace on the new actor
			IITraceInteractions::Execute_OnTraceBegin(pHitActor, this);
	}

	if (pHitActor != nullptr)
		IITraceInteractions::Execute_OnTrace(pHitActor, this);
}


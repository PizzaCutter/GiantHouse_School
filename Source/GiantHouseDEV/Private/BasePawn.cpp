// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/BasePawn.h"
#include "Public/WorldInteractor.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Public/Teleporter.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Engine/Engine.h"
#include "Components/ArrowComponent.h"
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "DrawDebugHelpers.h"
#include "Public/TraceInteractor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"

// Sets default values
ABasePawn::ABasePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	pMotionControllerL = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_L"));
	pMotionControllerR = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionCOntroller_R"));

	pHandSphereL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandSphere_L"));
	pHandSphereR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandSphere_R"));

	pWorldInteractorL = CreateDefaultSubobject<UWorldInteractor>(TEXT("WorldInteractor_L"));
	pWorldInteractorR = CreateDefaultSubobject<UWorldInteractor>(TEXT("WorldInteractor_R"));

	pTraceInteractor = CreateDefaultSubobject<UTraceInteractor>(TEXT("TraceInteractor"));

	pTeleporterL = CreateDefaultSubobject<UTeleporter>(TEXT("Teleporter_L"));
	pTeleporterR = CreateDefaultSubobject<UTeleporter>(TEXT("Teleporter_R"));

	pControllerSocketL = CreateDefaultSubobject<UArrowComponent>(TEXT("ControllerSocket_L"));
	pControllerSocketR = CreateDefaultSubobject<UArrowComponent>(TEXT("ControllerSocket_R"));

	pTeleportArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleporArrow"));

	pCameraCollider = CreateDefaultSubobject<USphereComponent>(TEXT("CameraCollider"));

	//SETTING SPECIAL 
	pMotionControllerL->SetTrackingSource(EControllerHand::Left);
	pMotionControllerR->SetTrackingSource(EControllerHand::Right);

	pWorldInteractorL->InteractionRadius = 10.f;
	pWorldInteractorR->InteractionRadius = 10.f;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	pControllerSocketL->SetRelativeLocation(FVector(60.f, -30.f, -10.f));
	pControllerSocketR->SetRelativeLocation(FVector(60.f, 30.f, -10.f));

	//CREATING HIERARCHY
	pCamera->SetupAttachment(RootComponent);
	pCameraCollider->SetupAttachment(pCamera);

	pMotionControllerL->SetupAttachment(RootComponent);
	pHandSphereL->SetupAttachment(pMotionControllerL);
	pWorldInteractorL->SetupAttachment(pMotionControllerL);
	pTeleporterL->SetupAttachment(pMotionControllerL);

	pMotionControllerR->SetupAttachment(RootComponent);
	pHandSphereR->SetupAttachment(pMotionControllerR);
	pWorldInteractorR->SetupAttachment(pMotionControllerR);
	pTeleporterR->SetupAttachment(pMotionControllerR);

	pControllerSocketL->SetupAttachment(pCamera);
	pControllerSocketR->SetupAttachment(pCamera);

	pTraceInteractor->SetupAttachment(pCamera);
}

// Called when the game starts or when spawned
void ABasePawn::BeginPlay()
{
	Super::BeginPlay();

	pWorldInteractorL->Setup(pHandSphereL);
	pWorldInteractorR->Setup(pHandSphereR);

	pTeleporterL->Setup(this);
	pTeleporterR->Setup(this);

	FindHMD();

	if(!HasHMD)
	{
		SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, 180.f));
	}

	ArcPathMeshSetup();
}

void ABasePawn::OnConstruction(const FTransform & Transform)
{
#if UE_BUILD_SHIPPING
	pCamera->bLockToHmd = true;
	HasHMD = true;
#else
	if (ForceKeyboardMouse)
	{
		pCamera->bLockToHmd = false;
	}
	else
	{
		pCamera->bLockToHmd = true;
	}
#endif 

}

// Called every frame
void ABasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(TeleportationDelayTimer <= TeleportationDelay)
	{
		TeleportationDelayTimer += DeltaTime;
	}

	CheckTeleport();

#if !UE_BUILD_SHIPPING
	if (!HasHMD)
	{
		if (pMotionControllerL == nullptr || pMotionControllerR == nullptr)
			return;
		pMotionControllerL->SetWorldLocation(pControllerSocketL->GetComponentLocation());
		pMotionControllerR->SetWorldLocation(pControllerSocketR->GetComponentLocation());
	}
	if(ForceKeyboardMouse)
	{
		pMotionControllerL->SetWorldLocation(pControllerSocketL->GetComponentLocation());
		pMotionControllerR->SetWorldLocation(pControllerSocketR->GetComponentLocation());
	}
#endif
}

// Called to bind functionality to input
void ABasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("PickupLeft", IE_Pressed, pWorldInteractorL, &UWorldInteractor::PickupPressed);
	PlayerInputComponent->BindAction("PickupLeft", IE_Pressed, pWorldInteractorL, &UWorldInteractor::DragStart);
	PlayerInputComponent->BindAction("PickupLeft", IE_Released, pWorldInteractorL, &UWorldInteractor::PickupReleased);
	PlayerInputComponent->BindAction("PickupLeft", IE_Released, pWorldInteractorL, &UWorldInteractor::DragStop);

	PlayerInputComponent->BindAction("PickupRight", IE_Pressed, pWorldInteractorR, &UWorldInteractor::PickupPressed);
	PlayerInputComponent->BindAction("PickupRight", IE_Pressed, pWorldInteractorR, &UWorldInteractor::DragStart);
	PlayerInputComponent->BindAction("PickupRight", IE_Released, pWorldInteractorR, &UWorldInteractor::PickupReleased);
	PlayerInputComponent->BindAction("PickupRight", IE_Released, pWorldInteractorR, &UWorldInteractor::DragStop);

	PlayerInputComponent->BindAction("UseLeft", IE_Pressed, pWorldInteractorL, &UWorldInteractor::UsePressed);
	PlayerInputComponent->BindAction("UseLeft", IE_Released, pWorldInteractorL, &UWorldInteractor::UseReleased);

	PlayerInputComponent->BindAction("UseRight", IE_Pressed, pWorldInteractorR, &UWorldInteractor::UsePressed);
	PlayerInputComponent->BindAction("UseRight", IE_Released, pWorldInteractorR, &UWorldInteractor::UseReleased);

	if(bAllowTeleportation)
	{
		PlayerInputComponent->BindAxis("LeftThumbstickX", this, &ABasePawn::LeftThumbstickX);
		PlayerInputComponent->BindAxis("LeftThumbstickY", this, &ABasePawn::LeftThumbstickY);
		PlayerInputComponent->BindAxis("RightThumbstickX", this, &ABasePawn::RightThumbstickX);
		PlayerInputComponent->BindAxis("RightThumbstickY", this, &ABasePawn::RightThumbstickY);
	}

	//TO-DO TRACE INTERACTOR

	//DEV KEYBOARD/MOUSE IMPLEMENTATION
	PlayerInputComponent->BindAxis("DEVMoveForward", this, &ABasePawn::MoveForward);
	PlayerInputComponent->BindAxis("DEVMoveRight", this, &ABasePawn::MoveRight);
	PlayerInputComponent->BindAxis("DEVMouseHorizontal", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("DEVMouseVertical", this, &APawn::AddControllerPitchInput);
}

void ABasePawn::MoveForward(float axis)
{
	FVector cameraFowardVector = pCamera->GetForwardVector();
	FVector currentWorldLocation = RootComponent->GetComponentLocation();
	FVector newLocation = ((cameraFowardVector * MovementSpeed) * axis) * GetWorld()->GetDeltaSeconds();
	newLocation += currentWorldLocation;

	RootComponent->SetWorldLocation(newLocation);
}

void ABasePawn::MoveRight(float axis)
{
	FVector cameraFowardVector = pCamera->GetRightVector();
	FVector currentWorldLocation = RootComponent->GetComponentLocation();
	FVector newLocation = ((cameraFowardVector * MovementSpeed) * axis) * GetWorld()->GetDeltaSeconds();
	newLocation += currentWorldLocation;

	RootComponent->SetWorldLocation(newLocation);
}

void ABasePawn::LeftThumbstickX(float axis)
{
	leftThumbstick.X = axis;
}
void ABasePawn::LeftThumbstickY(float axis)
{
	leftThumbstick.Y = axis;
}
void ABasePawn::RightThumbstickX(float axis)
{
	rightThumbstick.X = axis;
}
void ABasePawn::RightThumbstickY(float axis)
{
	rightThumbstick.Y = axis;
}

void ABasePawn::FindHMD()
{
	HasHMD = GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected();
}

void ABasePawn::CheckTeleport()
{
	if (bDying)
		return;

	if (TeleportationDelayTimer < TeleportationDelay)
		return;

	if (bTeleportDisabled)
	{
		if (pTeleporterL == nullptr || pTeleporterR == nullptr)
			return;

		pTeleporterL->SetActive(false);
		pTeleporterR->SetActive(false);
		return;
	}

	pTeleporterL->SetAxisValues(leftThumbstick);
	pTeleporterR->SetAxisValues(rightThumbstick);

	if (leftThumbstick.Size() >= 0.5f && prevLeftThumbstick.Size() <= 0.5f)
	{
		if (!pTeleporterL->IsActive())
		{
			pTeleporterL->SetActive(true);
		}
	}
	//The thumbstick is not being moved, if it was active the frame before then we teleport
	if (pTeleporterL->IsActive() && leftThumbstick.Size() <= 0.1f && prevLeftThumbstick.Size() >= 0.1f)
	{
		pTeleporterL->Teleport();
		pTeleporterL->SetActive(false);
		pTeleporterR->SetActive(false);
		TeleportationDelayTimer = 0.f;
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("TELEPORTED LEFT"));
	}

	if (rightThumbstick.Size() >= 0.1f && prevRightThumbstick.Size() <= 0.1f)
	{
		if (!pTeleporterR->IsActive())
		{
			pTeleporterR->SetActive(true);
		}
	}
	//The thumbstick is not being moved, if it was active the frame before then we teleport
	if (pTeleporterR->IsActive() && rightThumbstick.Size() <= 0.1f && prevRightThumbstick.Size() >= 0.1f)
	{
		pTeleporterR->Teleport();
		pTeleporterR->SetActive(false);
		pTeleporterL->SetActive(false);
		TeleportationDelayTimer = 0.f;

		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("TELEPORTED RIGHT"));
	}

	prevLeftThumbstick = leftThumbstick;
	prevRightThumbstick = rightThumbstick;
}

void ABasePawn::ResetTeleporter()
{
	if (pTeleporterL == nullptr || pTeleporterR == nullptr)
		return;
	pTeleporterL->ResetTeleportAreas();
	pTeleporterR->ResetTeleportAreas();
}

void ABasePawn::ArcPathMeshSetup()
{
	pTeleportTracePointsMeshes.Reset();
	AddMeshesToArcPathObjectPool();

	pTeleportArrowDynamicMaterial = UMaterialInstanceDynamic::Create(pTeleportArrow->GetMaterial(0), this);

	if (pTeleportArrow != nullptr)
		pTeleportArrow->SetMaterial(0, pTeleportArrowDynamicMaterial);
}

void ABasePawn::AddMeshesToArcPathObjectPool(int count)
{
	for (int i = 0; i < count; i++)
	{
		int newIndex = pTeleportTracePointsMeshes.Add(NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("ArcPathPointMesh_" + (pTeleportTracePointsMeshes.Num() + 1))));
		pTeleportTracePointsMeshes[newIndex]->SetStaticMesh(ArcTracePointMesh);
		pTeleportTracePointsMeshes[newIndex]->RegisterComponent();
		pTeleportTracePointsMeshes[newIndex]->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		pTeleportTracePointsMeshes[newIndex]->SetWorldScale3D(FVector(TracePointSize));
		pTeleportTracePointsMeshes[newIndex]->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);

		int newMaterialIndex = pTeleportTracePointsMaterials.Add(UMaterialInstanceDynamic::Create(pTracePointMaterial, this));
		pTeleportTracePointsMeshes[newMaterialIndex]->SetMaterial(0, pTeleportTracePointsMaterials[newMaterialIndex]);
		pTeleportTracePointsMaterials[newMaterialIndex]->SetScalarParameterValue("EmissiveIntensity", ArcPointMinEmissive);
	}
}
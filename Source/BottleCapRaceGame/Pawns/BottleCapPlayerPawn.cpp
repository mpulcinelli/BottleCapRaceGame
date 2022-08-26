// Fill out your copyright notice in the Description page of Project Settings.

#include "BottleCapPlayerPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BottleCapRaceGame/Components/BottleCapPawnMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "BottleCapRaceGame/UI/AccumulatorProgressWidget.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Components/TextRenderComponent.h"
#include "BottleCapPlayerState.h"
#include "BottleCapRaceGame/BottleCapRaceGameGameModeBase.h"

ABottleCapPlayerPawn::ABottleCapPlayerPawn() : InternalId(1)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// AutoPossessPlayer = EAutoReceiveInput::Player0;
	TurnRateGamepad = 10.f;

	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));

	RootComponent = SphereVisual;
	SphereVisual->SetSimulatePhysics(true);
	SphereVisual->SetCollisionProfileName(TEXT("Pawn"));
	SphereVisual->SetAngularDamping(1.1f);
	SphereVisual->SetLinearDamping(1.0f);
	SphereVisual->SetMassOverrideInKg(NAME_None, WeightKG, true);

	SphereVisual->BodyInstance.MassScale = 8.0f;
	SphereVisual->BodyInstance.MaxAngularVelocity = 5.0f;
	SphereVisual->SetNotifyRigidBodyCollision(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/Meshes/Chapinha"));

	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SphereVisual->SetWorldScale3D(FVector(0.8f));
		SphereVisual->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}

	AccumulatorVisual = CreateDefaultSubobject<UWidgetComponent>(TEXT("AccumulatorVisual"));
	AccumulatorVisual->SetupAttachment(RootComponent);
	AccumulatorVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	AccumulatorVisual->SetRelativeRotation(FRotator(0.000000, 180.000000, 0.000000));
	AccumulatorVisual->SetRelativeScale3D(FVector(0.300000, 0.300000, 0.030000));
	AccumulatorVisual->SetVisibility(false, true);
	AccumulatorVisual->SetIsReplicated(true);

	static ConstructorHelpers::FClassFinder<UUserWidget> AccumulatorProgressAsset(TEXT("/Game/BPs/WBP_AccumulatorProgress"));
	if (AccumulatorProgressAsset.Succeeded())
	{
		AccumulatorVisual->SetWidgetClass(AccumulatorProgressAsset.Class);
	}

	PointDirectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("PointDirectionDecal"));
	PointDirectionDecal->SetupAttachment(RootComponent);
	PointDirectionDecal->SetRelativeLocation(FVector(487.000000, 0.000000, 57.000000));
	PointDirectionDecal->SetRelativeRotation(FRotator(-90.0, 0.000000, 0.000000));
	PointDirectionDecal->SetVisibility(false, false);

	static ConstructorHelpers::FObjectFinder<UMaterial> MatDecalDirection(TEXT("/Game/Materials/MAT_ARROW_INDC_DIRECTION"));
	if (MatDecalDirection.Succeeded())
	{
		PointDirectionDecal->SetDecalMaterial(MatDecalDirection.Object);
	}

	PlayerName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerName"));
	PlayerName->SetupAttachment(RootComponent);
	PlayerName->SetRelativeLocation(FVector(0.0f, 0.0f, 186.0f));
	PlayerName->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	PlayerName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);

	// PlayerNameV2 = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerNameV2"));
	// PlayerNameV2->SetupAttachment(RootComponent);
	// PlayerNameV2->SetRelativeLocation(FVector(30.0f, 0.0f, 286.0f));
	// PlayerNameV2->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	// PlayerNameV2->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);

	USpringArmComponent *SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 150.0f), FRotator(-60.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;
	SpringArm->bUsePawnControlRotation = true;
	// SpringArm->SetIsReplicated(true);

	// Create a camera and attach to our spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Create an instance of our movement component, and tell it to update our root component.
	SelfMovementComponent = CreateDefaultSubobject<UBottleCapPawnMovementComponent>(TEXT("SelfMovementComponent"));
	SelfMovementComponent->UpdatedComponent = RootComponent;
	bAlwaysRelevant = true;
}

UPawnMovementComponent *ABottleCapPlayerPawn::GetMovementComponent() const
{
	return SelfMovementComponent;
}

void ABottleCapPlayerPawn::MoveForward(float AxisValue)
{
	if (!Controller || AxisValue == 0.0f)
		return;

	if (IsLocallyControlled())
	{
		Server_MoveForward(AxisValue, Controller->GetControlRotation());
	}
}

void ABottleCapPlayerPawn::Server_MoveForward_Implementation(float AxisValue, FRotator Rotation)
{
	if (SelfMovementComponent && (SelfMovementComponent->UpdatedComponent == RootComponent))
	{
		SetActorRotation(Rotation);
		CurrentRotation = Rotation;
		//SelfMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
		//CurrentPosition = GetActorLocation();
	}
}

void ABottleCapPlayerPawn::MoveRight(float AxisValue)
{
	if (!Controller || AxisValue == 0.0f)
		return;

	if (IsLocallyControlled())
	{
		Server_MoveRight(AxisValue, Controller->GetControlRotation());
	}
}

void ABottleCapPlayerPawn::Server_MoveRight_Implementation(float AxisValue, FRotator Rotation)
{
	if (SelfMovementComponent && (SelfMovementComponent->UpdatedComponent == RootComponent))
	{
		SetActorRotation(Rotation);
		CurrentRotation = Rotation;
		//SelfMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
		//CurrentPosition = GetActorLocation();
	}
}

void ABottleCapPlayerPawn::Server_ProvokeImpulse_Implementation(FVector Impulse)
{

	ABottleCapRaceGameGameModeBase *GM = Cast<ABottleCapRaceGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->MoveCap();
	}

	SphereVisual->AddImpulse(Impulse);
}

void ABottleCapPlayerPawn::Turn(float AxisValue)
{
	AddControllerYawInput(AxisValue * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ABottleCapPlayerPawn::AccumulateStart()
{
	if (IsLocallyControlled() && CanIMoveMe)
	{
		//CanIMove();
		PointDirectionDecal->SetVisibility(true, true);
		GetWorld()->GetTimerManager().SetTimer(AccumulatePowerHandle, this, &ABottleCapPlayerPawn::IncrementAccumulation, 0.1f, true, 0.0f);
		AccumulatorVisual->SetVisibility(true, true);
	}
}

void ABottleCapPlayerPawn::AccumulateStop()
{

	if (IsLocallyControlled() && CanIMoveMe)
	{
		GetWorld()->GetTimerManager().ClearTimer(AccumulatePowerHandle);

		FVector ForwardDirect = Camera->GetForwardVector();

		const FVector Impulse = FVector(ForwardDirect.X * PowerAccumulated * 15000, ForwardDirect.Y * PowerAccumulated * 15000, PowerAccumulated * 2000);

		Server_ProvokeImpulse(Impulse);

		PowerAccumulated = 0.0f;

		AccumulatorProgressWidget->PBarAccumulator->SetPercent(PowerAccumulated);

		AccumulatorVisual->SetVisibility(false, true);
		PointDirectionDecal->SetVisibility(false, true);
	}
}

void ABottleCapPlayerPawn::IncrementAccumulation()
{
	if (PowerAccumulated <= 12.0f)
	{
		PowerAccumulated += PowerRateAccumulator;
		AccumulatorProgressWidget->PBarAccumulator->SetPercent(FMath::Lerp(0.0f, 1.0f, PowerAccumulated / 12.0f));
	}
}

// void ABottleCapPlayerPawn::Server_TurnPlayerUp_Implementation()
// {
// 	SphereVisual->SetSimulatePhysics(false);
// 	FRotator MyRotation = SphereVisual->GetComponentRotation();
// 	auto NewRot = FMath::RInterpTo(MyRotation, FRotator(0.0f, 0.0f, 0.0f), GetWorld()->GetDeltaSeconds(), 100.0f);
// 	SphereVisual->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
// }

void ABottleCapPlayerPawn::Server_UpdateMyName_Implementation(const FText &_MyName)
{
	PlayerName->SetText(_MyName);
	UE_LOG(LogTemp, Warning, TEXT("Server_UpdateMyName_Implementation: %s"), *_MyName.ToString());
}

void ABottleCapPlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	UE_LOG(LogTemp, Warning, TEXT("GetLifetimeReplicatedProps"));
	DOREPLIFETIME(ABottleCapPlayerPawn, PowerAccumulated);
	DOREPLIFETIME(ABottleCapPlayerPawn, IsFallen);
	DOREPLIFETIME(ABottleCapPlayerPawn, CurrentPosition);
	DOREPLIFETIME(ABottleCapPlayerPawn, CurrentRotation);
	DOREPLIFETIME(ABottleCapPlayerPawn, SelfMovementComponent);
	DOREPLIFETIME(ABottleCapPlayerPawn, SphereVisual);
	DOREPLIFETIME(ABottleCapPlayerPawn, MyName);
	DOREPLIFETIME(ABottleCapPlayerPawn, InternalId);
	DOREPLIFETIME(ABottleCapPlayerPawn, CanIMoveMe);
}

void ABottleCapPlayerPawn::LookUp(float AxisValue)
{
	AddControllerPitchInput((AxisValue * -1) * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

class UAccumulatorProgressWidget *ABottleCapPlayerPawn::GetAccumulatorProgressWidget() const
{
	return Cast<UAccumulatorProgressWidget>(AccumulatorVisual->GetWidget());
}

// Called when the game starts or when spawned
void ABottleCapPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TArray<AActor *> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), OutActors);

		InternalId = OutActors.Num();

		MyName = FText::AsNumber(InternalId);

		Server_UpdateMyName(MyName);

		ABottleCapRaceGameGameModeBase *GM = Cast<ABottleCapRaceGameGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnChangePlayerToPlay.AddDynamic(this, &ABottleCapPlayerPawn::OnServerChangePlayerToPlay);
		}
	}

	SetReplicateMovement(true);
	SphereVisual->SetIsReplicated(true);
	SphereVisual->bReplicatePhysicsToAutonomousProxy = true;
	SelfMovementComponent->SetIsReplicated(true);

	SphereVisual->OnComponentHit.AddDynamic(this, &ABottleCapPlayerPawn::OnHit);

	AccumulatorProgressWidget = GetAccumulatorProgressWidget();

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
}

void ABottleCapPlayerPawn::OnServerChangePlayerToPlay(int32 id)
{
	CanIMoveMe = InternalId == id;
}

void ABottleCapPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SphereVisual->GetUpVector().Z <= 0.0f && !IsFallen)
	{
		IsFallen = true;
		// Server_TurnPlayerUp();
	}
	else if (SphereVisual->GetUpVector().Z >= 1.0f && IsFallen)
	{
		IsFallen = false;
		SphereVisual->SetAllPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f));
		SphereVisual->SetAllPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, 0.0f));
		SphereVisual->SetSimulatePhysics(true);
	}
}

void ABottleCapPlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABottleCapPlayerPawn::AccumulateStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ABottleCapPlayerPawn::AccumulateStop);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABottleCapPlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABottleCapPlayerPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABottleCapPlayerPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABottleCapPlayerPawn::LookUp);
}

void ABottleCapPlayerPawn::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComponent, FVector NormalImpulse, const FHitResult &Hit)
{
}

void ABottleCapPlayerPawn::OnRep_PosChange()
{
	SetActorLocation(CurrentPosition);
}

void ABottleCapPlayerPawn::OnRep_RotChange()
{
	SetActorRotation(CurrentRotation);
}

void ABottleCapPlayerPawn::OnRep_MyName()
{
	PlayerName->SetText(MyName);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_MyName: %s"), *MyName.ToString());
}

void ABottleCapPlayerPawn::OnRep_InternalId()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_InternalId: %d"), InternalId);
}


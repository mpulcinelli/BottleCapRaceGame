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
#include "BottleCapRaceGame/GameMode/BottleCapRaceGameGameModeBase.h"
#include "BottleCapRaceGame/Helpers/FormatMessage.h"

ABottleCapPlayerPawn::ABottleCapPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	TurnRateGamepad = 10.f;

	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));

	RootComponent = SphereVisual;
	SphereVisual->SetCollisionProfileName(TEXT("Pawn"));
	SphereVisual->SetNotifyRigidBodyCollision(true);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/Meshes/Chapinha"));

	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SphereVisual->SetWorldScale3D(FVector(0.5f));
		SphereVisual->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
	}

	AccumulatorVisual = CreateDefaultSubobject<UWidgetComponent>(TEXT("AccumulatorVisual"));
	AccumulatorVisual->SetupAttachment(RootComponent);
	AccumulatorVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	AccumulatorVisual->SetRelativeRotation(FRotator(0.000000, 180.000000, 0.000000));
	AccumulatorVisual->SetRelativeScale3D(FVector(0.300000, 0.300000, 0.030000));
	AccumulatorVisual->SetVisibility(false, true);
	AccumulatorVisual->SetIsReplicated(true);
	AccumulatorVisual->SetOnlyOwnerSee(true);

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

	static ConstructorHelpers::FObjectFinder<UMaterial> MatPlayerName(TEXT("/Game/Materials/MAT_PlayerName"));
	if (MatPlayerName.Succeeded())
	{
		PlayerName->SetMaterial(0, MatPlayerName.Object);
	}

	PlayerRemainingMoves = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerRemainingMoves"));
	PlayerRemainingMoves->SetupAttachment(RootComponent);
	PlayerRemainingMoves->SetRelativeLocation(FVector(0.0f, -80.0f, 186.0f));
	PlayerRemainingMoves->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	PlayerRemainingMoves->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);

	static ConstructorHelpers::FObjectFinder<UMaterial> MatRemainingMoves(TEXT("/Game/Materials/MAT_RemainingMoves"));
	if (MatRemainingMoves.Succeeded())
	{
		PlayerRemainingMoves->SetMaterial(0, MatRemainingMoves.Object);
	}

	USpringArmComponent *SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	SpringArm->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 1.0f;
	SpringArm->bUsePawnControlRotation = true;

	// Create a camera and attach to our spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ABottleCapPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	PRINT_LOG();

	SphereVisual->SetAngularDamping(1.1f);
	SphereVisual->SetLinearDamping(1.0f);
	SphereVisual->SetMassOverrideInKg(NAME_None, WeightKG, true);

	if (!IsLocallyControlled())
	{
		PlayerRemainingMoves->SetVisibility(false);
	}

	auto PS = GetPlayerState();
	if (PS)
	{
		InternalId = PS->GetPlayerId();
	}

	if (HasAuthority())
	{
		TArray<AActor *> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), OutActors);

		ABottleCapRaceGameGameModeBase *GM = Cast<ABottleCapRaceGameGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnChangePlayerToPlay.AddDynamic(this, &ABottleCapPlayerPawn::OnServerChangePlayerToPlay);
			GM->OnChangeRemainingMoves.AddDynamic(this, &ABottleCapPlayerPawn::OnServerChangeRemainingMoves);
			RemainingMoves = GM->GetPlayerRemainingMoves();
			Server_UpdatePlayerRemainingMoves(RemainingMoves);
		}
	}

	SphereVisual->SetSimulatePhysics(true);

	AccumulatorProgressWidget = GetAccumulatorProgressWidget();

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
}

void ABottleCapPlayerPawn::UpdateMyName(int32 _MyName)
{
	PRINT_LOG();

	if (HasAuthority())
	{
		PRINT_LOG_2("MyName", FString::FromInt(_MyName));
		Server_UpdateMyName(FText::AsNumber(_MyName));
	}
}

void ABottleCapPlayerPawn::Server_UpdatePlayerRemainingMoves_Implementation(int32 qtd)
{
	RemainingMoves = qtd;

	if (IsLocallyControlled() && CanIMoveMe)
	{
		PlayerRemainingMoves->SetVisibility(true);
		PlayerRemainingMoves->SetText(FText::AsNumber(qtd));
	}
	else
	{
		PlayerRemainingMoves->SetVisibility(false);
		PlayerRemainingMoves->SetText(FText::AsNumber(0));
	}

	PRINT_LOG_2("int32 qtd", FString::FromInt(qtd));
}

void ABottleCapPlayerPawn::Server_ProvokeImpulse_Implementation(FVector Impulse)
{
	PRINT_LOG();

	ABottleCapRaceGameGameModeBase *GM = Cast<ABottleCapRaceGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->MoveCap();
	}

	SphereVisual->AddImpulse(Impulse);
}

void ABottleCapPlayerPawn::Turn(float AxisValue)
{
	if (!Controller || AxisValue == 0.0f)
		return;

	if (IsLocallyControlled())
	{
		AddControllerYawInput(AxisValue * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
		CurrentRotation = GetActorRotation();
		Server_Turn(CurrentRotation);
	}
}

void ABottleCapPlayerPawn::Server_Turn_Implementation(FRotator AxisValue)
{
	SetActorRotation(AxisValue);
}

void ABottleCapPlayerPawn::AccumulateStart()
{
	if (IsLocallyControlled() && CanIMoveMe)
	{
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

void ABottleCapPlayerPawn::Server_UpdateMyName_Implementation(const FText &MyName)
{
	PRINT_LOG_2("MyName", *MyName.ToString());
	
	InternalId = FCString::Atoi(*MyName.ToString());
	const FText _MyName = FText::AsNumber(InternalId);
	PlayerName->SetText(_MyName);
}

void ABottleCapPlayerPawn::LookUp(float AxisValue)
{
	if (!Controller || AxisValue == 0.0f)
		return;

	if (IsLocallyControlled())
	{
		AddControllerPitchInput((AxisValue * -1) * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
		CurrentRotation = GetActorRotation();
		Server_LookUp(CurrentRotation);
	}
}

void ABottleCapPlayerPawn::Server_LookUp_Implementation(FRotator AxisValue)
{
	SetActorRotation(AxisValue);
}

class UAccumulatorProgressWidget *ABottleCapPlayerPawn::GetAccumulatorProgressWidget() const
{
	return Cast<UAccumulatorProgressWidget>(AccumulatorVisual->GetWidget());
}

void ABottleCapPlayerPawn::OnServerChangePlayerToPlay(int32 id)
{
	CanIMoveMe = InternalId == id;
}

void ABottleCapPlayerPawn::OnServerChangeRemainingMoves(int32 qtd)
{

	if (IsLocallyControlled() && CanIMoveMe)
	{
		PlayerRemainingMoves->SetVisibility(true);
		PlayerRemainingMoves->SetText(FText::AsNumber(qtd));
	}
	else
	{
		PlayerRemainingMoves->SetVisibility(false);
		PlayerRemainingMoves->SetText(FText::AsNumber(0));
	}

	Server_UpdatePlayerRemainingMoves(qtd);

	PRINT_LOG_1(FString::FromInt(qtd));
}

void ABottleCapPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABottleCapPlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABottleCapPlayerPawn::AccumulateStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ABottleCapPlayerPawn::AccumulateStop);
	PlayerInputComponent->BindAxis("Turn", this, &ABottleCapPlayerPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABottleCapPlayerPawn::LookUp);
}

void ABottleCapPlayerPawn::OnRep_RemainingMoves()
{
	if (IsLocallyControlled() && CanIMoveMe)
	{
		PlayerRemainingMoves->SetVisibility(true);
		PlayerRemainingMoves->SetText(FText::AsNumber(RemainingMoves));
	}
	else
	{
		PlayerRemainingMoves->SetVisibility(false);
		PlayerRemainingMoves->SetText(FText::AsNumber(0));
	}

	PRINT_LOG_1(FString::FromInt(RemainingMoves));
}

void ABottleCapPlayerPawn::OnRep_ChangeInternalId()
{
	//if (!IsLocallyControlled())
	//{
		const FText _MyName = FText::AsNumber(InternalId);
		PlayerName->SetText(_MyName);

		PRINT_LOG_1(FString::FromInt(InternalId));
	//}
}

void ABottleCapPlayerPawn::OnRep_RotChange()
{
	if (!IsLocallyControlled())
	{
		SetActorRotation(CurrentRotation);
	}
}

void ABottleCapPlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABottleCapPlayerPawn, PowerAccumulated);
	DOREPLIFETIME(ABottleCapPlayerPawn, SphereVisual);
	DOREPLIFETIME(ABottleCapPlayerPawn, CanIMoveMe);
	DOREPLIFETIME(ABottleCapPlayerPawn, CurrentRotation);
	DOREPLIFETIME(ABottleCapPlayerPawn, RemainingMoves);
	DOREPLIFETIME(ABottleCapPlayerPawn, InternalId);

	PRINT_LOG();
}

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
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/TriggerBox.h"
#include "BottleCapRaceGame/BottleCapGameInstance.h"

/**
 * @brief Construct a new ABottleCapPlayerPawn::ABottleCapPlayerPawn object
 * 
 * @param ObjectInitializer 
 */
ABottleCapPlayerPawn::ABottleCapPlayerPawn(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	TurnRateGamepad = 10.f;

	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	RootComponent = SphereVisual;
	SphereVisual->SetCollisionProfileName(TEXT("Pawn"));
	SphereVisual->AlwaysLoadOnClient = true;
	SphereVisual->AlwaysLoadOnServer = true;
	SphereVisual->bOwnerNoSee = false;
	SphereVisual->bCastDynamicShadow = true;
	SphereVisual->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	// SphereVisual->SetNotifyRigidBodyCollision(true);
	SphereVisual->SetGenerateOverlapEvents(true);
	// SphereVisual->SetCanEverAffectNavigation(false);

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

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	SpringArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;

	// Create a camera and attach to our spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	MyTurnIndicatorEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MyTurnIndicatorEffect"));
	MyTurnIndicatorEffect->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> CircleTurnNiagara(TEXT("/Game/ParticleEffects/FX_CircleTurn"));
	if (CircleTurnNiagara.Succeeded())
	{
		MyTurnIndicatorEffect->SetAsset(CircleTurnNiagara.Object);
	}

	MyTurnIndicatorEffect->SetAutoActivate(false);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	bAlwaysRelevant = true;
}

/**
 * @brief Método executado quando o jogo inicia.
 * 
 */
void ABottleCapPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

#if UE_BUILD_DEVELOPMENT
	if (HasAuthority())
	{
		PRINT_LOG_1(GetWorld()->GetAuthGameMode()->GetFullName());
	}
#endif

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

	InitialLocation = GetActorLocation();

	SphereVisual->OnComponentBeginOverlap.AddDynamic(this, &ABottleCapPlayerPawn::OnOverlapOtherComponent);
	SphereVisual->OnComponentHit.AddDynamic(this, &ABottleCapPlayerPawn::OnHitOtherComponent);
}

/**
 * @brief Método executado quando o player passa por algum outro objeto.
 * 
 * @param OverlappedComponent 
 * @param OtherActor 
 * @param OtherComp 
 * @param OtherBodyIndex 
 * @param bFromSweep 
 * @param SweepResult 
 */
void ABottleCapPlayerPawn::OnOverlapOtherComponent(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor->GetClass() == ATriggerBox::StaticClass())
	{
#if UE_BUILD_DEVELOPMENT
		PRINT_LOG_1(OtherActor->GetFullName());
#endif
		if (IsLocallyControlled())
		{
#if UE_BUILD_DEVELOPMENT
			PRINT_LOG_2("IsLocallyControlled()", InitialLocation.ToString());
#endif

			Server_ResetLocation(InitialLocation);
		}
	}
}

/**
 * @brief Método executado quando o player bate em algum outro objeto.
 * 
 * @param HitComponent 
 * @param OtherActor 
 * @param OtherComp 
 * @param NormalImpulse 
 * @param Hit 
 */
void ABottleCapPlayerPawn::OnHitOtherComponent(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (OtherActor->GetClass() == ATriggerBox::StaticClass())
	{
#if UE_BUILD_DEVELOPMENT
		PRINT_LOG_1(OtherActor->GetFullName());
#endif
	}
}

/**
 * @brief Atualiza o nome do player que será apresentado.
 * 
 * @param _MyName 
 */
void ABottleCapPlayerPawn::UpdateMyName(int32 _MyName)
{
#if UE_BUILD_DEVELOPMENT
	PRINT_LOG();
#endif

	if (HasAuthority())
	{
		PRINT_LOG_2("MyName", FString::FromInt(_MyName));
		Server_UpdateMyName(FText::AsNumber(_MyName));
	}
}

/**
 * @brief Atualiza a quantidade de movimentos que ainda estão faltando.
 * 
 * @param qtd 
 */
void ABottleCapPlayerPawn::Server_UpdatePlayerRemainingMoves_Implementation(int32 qtd)
{
	RemainingMoves = qtd;

	if (IsLocallyControlled() && CanIMoveMe)
	{
		PlayerRemainingMoves->SetVisibility(true);
		PlayerRemainingMoves->SetText(FText::AsNumber(qtd));
		MyTurnIndicatorEffect->Activate();
	}
	else
	{
		PlayerRemainingMoves->SetVisibility(false);
		PlayerRemainingMoves->SetText(FText::AsNumber(0));
		MyTurnIndicatorEffect->Deactivate();
	}

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG_2("RemainingMoves", FString::FromInt(qtd));
#endif
}

/**
 * @brief Quando o player morre ele volta para o local de origem.
 * 
 * @param Location 
 */
void ABottleCapPlayerPawn::Server_ResetLocation_Implementation(FVector Location)
{

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG_2("LOCATION:", Location.ToString());
#endif

	ABottleCapRaceGameGameModeBase *GM = Cast<ABottleCapRaceGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->GoNextPlayer();
	}

	SetActorLocation(Location);
	SetActorRotation(FRotator(0.0f));
}

/**
 * @brief Faz com que o servidor provoque o impulso no cliente.
 * 
 * @param Impulse 
 */
void ABottleCapPlayerPawn::Server_ProvokeImpulse_Implementation(FVector Impulse)
{
	ABottleCapRaceGameGameModeBase *GM = Cast<ABottleCapRaceGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->MoveCap();
	}

	SphereVisual->AddImpulse(Impulse);
}

/**
 * @brief Método para olhar para cima e para baixo
 *
 * @param AxisValue
 */
void ABottleCapPlayerPawn::LookUp(float AxisValue)
{
	if (!Controller || AxisValue == 0.0f)
		return;

	SpringArm->AddLocalRotation(FRotator(AxisValue, 0.000000, 0.000000));
}

/**
 * @brief Método para olhar para os lados.
 *
 * @param AxisValue
 */
void ABottleCapPlayerPawn::Turn(float AxisValue)
{
	if (!Controller || AxisValue == 0.0f)
		return;

	AddControllerYawInput(AxisValue * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
	CurrentActorRotation = GetActorRotation();

	Server_Turn(CurrentActorRotation);
}

/**
 * @brief Método para fazer o servidor olhar para os lados.
 *
 * @param AxisValue
 */
void ABottleCapPlayerPawn::Server_Turn_Implementation(FRotator AxisValue)
{
	if (!IsLocallyControlled())
	{
		SetActorRotation(AxisValue);
	}
}

/**
 * @brief Método para iniciar o acumulo de força para fazer a movimentação do player.
 */
void ABottleCapPlayerPawn::AccumulateStart()
{
	if (IsLocallyControlled() && CanIMoveMe)
	{
		PointDirectionDecal->SetVisibility(true, true);
		GetWorld()->GetTimerManager().SetTimer(AccumulatePowerHandle, this, &ABottleCapPlayerPawn::IncrementAccumulation, 0.1f, true, 0.0f);
		AccumulatorVisual->SetVisibility(true, true);
	}
}

/**
 * @brief Método para finalizar e disparar o acumulo de força para fazer a movimentação do player.
 */
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

/**
 * @brief Método para incrementar o acumulo de força para a movimentação do player.
 *
 */
void ABottleCapPlayerPawn::IncrementAccumulation()
{
	if (PowerAccumulated <= 12.0f)
	{
		PowerAccumulated += PowerRateAccumulator;
		AccumulatorProgressWidget->PBarAccumulator->SetPercent(FMath::Lerp(0.0f, 1.0f, PowerAccumulated / 12.0f));
	}
}

/**
 * @brief Faz com que o servidor atualize o nome do cliente.
 * 
 * @param MyName 
 */
void ABottleCapPlayerPawn::Server_UpdateMyName_Implementation(const FText &MyName)
{

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG_2("MyName", *MyName.ToString());
#endif

	InternalId = FCString::Atoi(*MyName.ToString());
	const FText _MyName = FText::AsNumber(InternalId);
	PlayerName->SetText(_MyName);
}

/**
 * @brief Recupera o widget com o acumulador visual.
 * 
 * @return UAccumulatorProgressWidget* 
 */
class UAccumulatorProgressWidget *ABottleCapPlayerPawn::GetAccumulatorProgressWidget() const
{
	return Cast<UAccumulatorProgressWidget>(AccumulatorVisual->GetWidget());
}

/**
 * @brief Evento disparado pelo servidor para os clientes informando qual o ID deve poder se movimentar.
 * 
 * @param id 
 */
void ABottleCapPlayerPawn::OnServerChangePlayerToPlay(int32 id)
{
	CanIMoveMe = InternalId == id;
}

/**
 * @brief Evento disparado pelo servidor para os clientes informando a quantidade de movimentos ainda estão disponíveis.
 * 
 * @param qtd 
 */
void ABottleCapPlayerPawn::OnServerChangeRemainingMoves(int32 qtd)
{

	if (IsLocallyControlled() && CanIMoveMe)
	{
		PlayerRemainingMoves->SetVisibility(true);
		PlayerRemainingMoves->SetText(FText::AsNumber(qtd));
		MyTurnIndicatorEffect->Activate();
	}
	else
	{
		PlayerRemainingMoves->SetVisibility(false);
		PlayerRemainingMoves->SetText(FText::AsNumber(0));
		MyTurnIndicatorEffect->Deactivate();
	}

	Server_UpdatePlayerRemainingMoves(qtd);

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG_1(FString::FromInt(qtd));
#endif
}

/**
 * @brief Evento executado em cada frame do jogo.
 * 
 * @param DeltaTime 
 */
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

/**
 * @brief Método executado no cliente para replicação dos dados de quantidade de movimentos restantes.
 * 
 */
void ABottleCapPlayerPawn::OnRep_RemainingMoves()
{
	if (IsLocallyControlled() && CanIMoveMe)
	{
		PlayerRemainingMoves->SetVisibility(true);
		PlayerRemainingMoves->SetText(FText::AsNumber(RemainingMoves));
		MyTurnIndicatorEffect->Activate();
	}
	else
	{
		PlayerRemainingMoves->SetVisibility(false);
		PlayerRemainingMoves->SetText(FText::AsNumber(0));
		MyTurnIndicatorEffect->Deactivate();
	}

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG_1(FString::FromInt(RemainingMoves));
#endif
}

/**
 * @brief Método executado no cliente para replicação dos dados de internal ID.
 * 
 */
void ABottleCapPlayerPawn::OnRep_ChangeInternalId()
{
	const FText _MyName = FText::AsNumber(InternalId);
	PlayerName->SetText(_MyName);

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG_1(FString::FromInt(InternalId));
#endif
}

/**
 * @brief Método executado no cliente para ajustar a localização do ator no mapa.
 * 
 */
void ABottleCapPlayerPawn::OnRep_ActorRotChange()
{
	if (!IsLocallyControlled())
	{
		SetActorRotation(CurrentActorRotation);
	}

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG_1(CurrentActorRotation.ToString());
#endif
}

void ABottleCapPlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABottleCapPlayerPawn, PowerAccumulated);
	DOREPLIFETIME(ABottleCapPlayerPawn, SphereVisual);
	DOREPLIFETIME(ABottleCapPlayerPawn, Camera);
	DOREPLIFETIME(ABottleCapPlayerPawn, CanIMoveMe);
	DOREPLIFETIME(ABottleCapPlayerPawn, CurrentActorRotation);
	DOREPLIFETIME(ABottleCapPlayerPawn, RemainingMoves);
	DOREPLIFETIME(ABottleCapPlayerPawn, InternalId);

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG();
#endif
}

void ABottleCapPlayerPawn::Destroyed()
{
	Super::Destroyed();

	if (HasAuthority())
	{
		if (auto GM = GetWorld()->GetAuthGameMode())
		{
			if (auto PC = GetWorld()->GetFirstPlayerController())
			{
				GM->RestartPlayer(PC);
			}
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BottleCapPlayerPawn.generated.h"

UCLASS()
class BOTTLECAPRACEGAME_API ABottleCapPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties

	ABottleCapPlayerPawn(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Replicated)
	class UStaticMeshComponent *SphereVisual;

	UPROPERTY(EditAnywhere)
	class UTextRenderComponent *PlayerName;

	UPROPERTY(EditAnywhere)
	class UTextRenderComponent *PlayerRemainingMoves;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent *AccumulatorVisual;

	UPROPERTY(EditAnywhere, Replicated)
	class UCameraComponent *Camera;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent *SpringArm;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent *MyTurnIndicatorEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Input)
	float PowerRateAccumulator = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Input)
	float WeightKG = 150.0f;

	UPROPERTY(Replicated)
	bool CanIMoveMe = false;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_RemainingMoves)
	int32 RemainingMoves = 0;

	UFUNCTION(Server, Reliable)
	void Server_ProvokeImpulse(FVector Impulse);
	void Server_ProvokeImpulse_Implementation(FVector Impulse);

	UFUNCTION(Server, Reliable)
	void Server_ResetLocation(FVector Location);
	void Server_ResetLocation_Implementation(FVector Location);

	UFUNCTION(Server, Reliable)
	void Server_Turn(FRotator AxisValue);
	void Server_Turn_Implementation(FRotator AxisValue);
	void Turn(float AxisValue);

	void LookUp(float AxisValue);

	UFUNCTION(Server, Reliable)
	void Server_UpdatePlayerRemainingMoves(int32 qtd);
	void Server_UpdatePlayerRemainingMoves_Implementation(int32 qtd);

	UFUNCTION()
	void AccumulateStart();

	UFUNCTION()
	void AccumulateStop();

	void IncrementAccumulation();

	UFUNCTION(NetMulticast, Reliable)
	void Server_UpdateMyName(const FText &_MyName);

	void UpdateMyName(int32 _MyName);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnServerChangePlayerToPlay(int32 id);

	UFUNCTION()
	void OnServerChangeRemainingMoves(int32 qtd);

	UPROPERTY(Replicated)
	float PowerAccumulated = 0.0f;

	class UAccumulatorProgressWidget *GetAccumulatorProgressWidget() const;

	class UAccumulatorProgressWidget *AccumulatorProgressWidget;

	UPROPERTY(EditAnywhere)
	class UDecalComponent *PointDirectionDecal;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_ActorRotChange)
	FRotator CurrentActorRotation;

	UPROPERTY()
	FVector InitialLocation;

	FTimerHandle AccumulatePowerHandle;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_ChangeInternalId)
	int32 InternalId;

	UFUNCTION()
	void OnRep_ActorRotChange();

	UFUNCTION()
	void OnRep_RemainingMoves();

	UFUNCTION()
	void OnRep_ChangeInternalId();

	UFUNCTION()
	void OnOverlapOtherComponent(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnHitOtherComponent(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	float TurnRateGamepad;

	float JumpImpulse = 550000.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
};

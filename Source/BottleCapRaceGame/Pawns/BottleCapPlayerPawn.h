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
	ABottleCapPlayerPawn();

	UPROPERTY(Replicated)
	class UBottleCapPawnMovementComponent *SelfMovementComponent;

	virtual UPawnMovementComponent *GetMovementComponent() const override;

	UPROPERTY(EditAnywhere, Replicated)
	class UStaticMeshComponent *SphereVisual;

	UPROPERTY(EditAnywhere)
	class UTextRenderComponent *PlayerName;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyName)
	FText MyName;



	UPROPERTY(EditAnywhere)
	class UWidgetComponent *AccumulatorVisual;

	UPROPERTY(EditAnywhere)
	class UCameraComponent *Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Input)
	float PowerRateAccumulator = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Input)
	float WeightKG = 150.0f;

	UPROPERTY(Replicated)
	bool CanIMoveMe=false;

	UFUNCTION(Server, Reliable)
	void Server_MoveForward(float AxisValue, FRotator Rotation);
	void Server_MoveForward_Implementation(float AxisValue, FRotator Rotation);
	void MoveForward(float AxisValue);

	UFUNCTION(Server, Reliable)
	void Server_MoveRight(float AxisValue, FRotator Rotation);
	void Server_MoveRight_Implementation(float AxisValue, FRotator Rotation);
	void MoveRight(float AxisValue);

	UFUNCTION(Server, Reliable)
	void Server_ProvokeImpulse(FVector Impulse);
	void Server_ProvokeImpulse_Implementation(FVector Impulse);


	void Turn(float AxisValue);

	void LookUp(float AxisValue);

	UFUNCTION()
	void AccumulateStart();

	UFUNCTION()
	void AccumulateStop();

	void IncrementAccumulation();

	// UFUNCTION(Server, Reliable)
	// void Server_TurnPlayerUp();
	// void Server_TurnPlayerUp_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Server_UpdateMyName(const FText & _MyName);
	void Server_UpdateMyName_Implementation(const FText & _MyName);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;



private:

	UFUNCTION()
	void OnServerChangePlayerToPlay(int32 id);

	UPROPERTY(Replicated)
	float PowerAccumulated = 0.0f;

	class UAccumulatorProgressWidget *GetAccumulatorProgressWidget() const;

	class UAccumulatorProgressWidget *AccumulatorProgressWidget;

	UPROPERTY(EditAnywhere)
	class UDecalComponent* PointDirectionDecal;

	FTimerHandle AccumulatePowerHandle;
	FTimerHandle TurnPlayerUpHandle;
	FTimerHandle CheckIfICanMoveUpHandle;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_PosChange)
	FVector CurrentPosition;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_RotChange)
	FRotator CurrentRotation;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_InternalId)
	int32 InternalId;

	UPROPERTY(Replicated)
	bool IsFallen = false;

	UFUNCTION()
	void OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComponent, FVector NormalImpulse, const FHitResult &Hit);

	UFUNCTION()
	void OnRep_PosChange();

	UFUNCTION()
	void OnRep_RotChange();

	UFUNCTION()
	void OnRep_MyName();

	UFUNCTION()
	void OnRep_InternalId();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float TurnRateGamepad;

	float JumpImpulse = 550000.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TurretComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TURRETLABA_API UTurretComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTurretComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Settings")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Settings", meta = (ClampMin = "100.0"))
	float DetectionRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Settings", meta = (ClampMin = "0.05"))
	float FireInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Settings", meta = (ClampMin = "0.1"))
	float RotationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Settings")
	FName MuzzleSocketName = "MuzzleSocket";

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret State")
	AActor* TargetActor;

	FTimerHandle FireTimerHandle;
	bool bIsShooting;

	void FindTarget();
	bool HasLineOfSight(AActor* Target) const;
	void RotateTowardsTarget(float DeltaTime);
	void StartFiring();
	void StopFiring();
	void FireProjectile();
	FVector GetMuzzleLocation() const;
		
};

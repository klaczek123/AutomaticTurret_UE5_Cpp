#include "TurretComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"

UTurretComponent::UTurretComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTurretComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTurretComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!TargetActor || TargetActor->GetDistanceTo(GetOwner()) > DetectionRange)
	{
		TargetActor = nullptr;
		FindTarget();
	}

	if (TargetActor)
	{
		if (HasLineOfSight(TargetActor))
		{
			RotateTowardsTarget(DeltaTime);
			StartFiring();
		}
		else
		{
			StopFiring();
		}
	}
	else
	{
		StopFiring();
	}
}

void UTurretComponent::FindTarget()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	float Distance = PlayerPawn->GetDistanceTo(GetOwner());
	if (Distance <= DetectionRange)
	{
		TargetActor = PlayerPawn;
	}
}

bool UTurretComponent::HasLineOfSight(AActor* Target) const
{
	if (!Target || !GetWorld()) return false;

	FHitResult HitResult;
	FVector Start = GetMuzzleLocation();
	FVector End = Target->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	if (GetOwner())
	{
		TArray<UPrimitiveComponent*> OwnerComponents;
		GetOwner()->GetComponents<UPrimitiveComponent>(OwnerComponents);
		QueryParams.AddIgnoredComponents(OwnerComponents);
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		return (HitActor == Target || (HitActor && HitActor->IsA(Target->GetClass())));
	}

	return true;
}

void UTurretComponent::RotateTowardsTarget(float DeltaTime)
{
	if (!TargetActor || !GetOwner()) return;

	FVector TargetLocation = TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, 30.0f);
	FVector TurretLocation = GetMuzzleLocation();

	FRotator TargetRotation = (TargetLocation - TurretLocation).Rotation();

	
	TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, -30.0f, 15.0f);
	TargetRotation.Roll = 0.0f;

	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

	GetOwner()->SetActorRotation(NewRotation);
}

void UTurretComponent::StartFiring()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&UTurretComponent::FireProjectile,
			FireInterval,
			true
		);
	}
}

void UTurretComponent::StopFiring()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UTurretComponent::FireProjectile()
{
	if (!ProjectileClass || !GetWorld()) return;

	FVector SpawnLocation = GetMuzzleLocation();
	FRotator SpawnRotation = GetOwner()->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}

FVector UTurretComponent::GetMuzzleLocation() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return FVector::ZeroVector;

	USkeletalMeshComponent* SkelMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (SkelMesh && SkelMesh->DoesSocketExist(MuzzleSocketName))
	{
		return SkelMesh->GetSocketLocation(MuzzleSocketName);
	}

	return Owner->GetActorLocation() + (Owner->GetActorForwardVector() * 100.0f) + FVector(0.0f, 0.0f, 50.0f);
}
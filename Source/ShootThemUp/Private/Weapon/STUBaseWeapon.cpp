// Polah Studios' Game, All Rights Reserved.


#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMeshComponent");
	SetRootComponent(WeaponMeshComponent);
}

void ASTUBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	check(WeaponMeshComponent);

	CurrentAmmo = DefaultAmmo;
}

void ASTUBaseWeapon::StartFire()
{
}

void ASTUBaseWeapon::StopFire()
{
}

void ASTUBaseWeapon::MakeShot()
{
}

APlayerController* ASTUBaseWeapon::GetPlayerController() const
{
	const auto Player = Cast<ACharacter>(GetOwner());
	if (!Player)
	{
		return nullptr;
	}

	return Player->GetController<APlayerController>();
}

bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation)
{
	const auto Controller = GetPlayerController();
	if (!Controller)
	{
		return false;
	}

	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	return true;
}

FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const
{
	return WeaponMeshComponent->GetSocketLocation(MuzzleSocketName);
}

bool ASTUBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd)
{
	FVector ViewLocation;
	FRotator ViewRotation;
	if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
	{
		return false;
	}

	TraceStart = ViewLocation;
	const FVector ShootDirection = ViewRotation.Vector();
	TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
	return true;
}

void ASTUBaseWeapon::MakeHit(FHitResult& HitResult, FVector& TraceStart, FVector& TraceEnd)
{
	// method is protected, so we check world here for the inheritors
	if (!GetWorld())
	{
		return;
	}

	// For not shooting yourself
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(
		HitResult, 
		TraceStart, 
		TraceEnd, 
		ECollisionChannel::ECC_Visibility,
		CollisionParams
	);
}

void ASTUBaseWeapon::DecreaseAmmo()
{
	if (CurrentAmmo.BulletsInClip == 0)
	{
		UE_LOG(LogBaseWeapon, Warning, TEXT("Clip is empty"));
		return;
	}

	CurrentAmmo.BulletsInClip--;
	LogAmmo();


	if (IsClipEmpty() && !IsAmmoEmpty())
	{
		StopFire();
		OnClipEmpty.Broadcast();
	}
}

bool ASTUBaseWeapon::IsAmmoEmpty() const
{
	return !CurrentAmmo.IsInfinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}

bool ASTUBaseWeapon::IsClipEmpty() const
{
	return CurrentAmmo.BulletsInClip == 0;
}

void ASTUBaseWeapon::ChangeClip()
{
	if (!CurrentAmmo.IsInfinite)
	{
		if (CurrentAmmo.Clips == 0)
		{
			UE_LOG(LogBaseWeapon, Warning, TEXT("No more clips"));
			return;
		}
		CurrentAmmo.Clips--;
	}
	CurrentAmmo.BulletsInClip = DefaultAmmo.BulletsInClip;
	UE_LOG(LogBaseWeapon, Display, TEXT("Reload done."));
}

bool ASTUBaseWeapon::CanReload() const
{
	return CurrentAmmo.BulletsInClip < DefaultAmmo.BulletsInClip && CurrentAmmo.Clips > 0;
}

void ASTUBaseWeapon::LogAmmo()
{
	FString AmmoInfo = "Ammo: " + FString::FromInt(CurrentAmmo.BulletsInClip) + " / ";
	AmmoInfo += CurrentAmmo.IsInfinite ? "Infinite" : FString::FromInt(CurrentAmmo.Clips);
	UE_LOG(LogBaseWeapon, Display, TEXT("%s"), *AmmoInfo);

}

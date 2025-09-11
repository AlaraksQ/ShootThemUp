// Polah Studios' Game, All Rights Reserved.


#include "Weapon/STURocketLauncherWeapon.h"
#include "Weapon/STUProjectile.h"
#include "Kismet/GameplayStatics.h"

void ASTURocketLauncherWeapon::StartFire()
{
	MakeShot();
}

void ASTURocketLauncherWeapon::MakeShot()
{
	const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
	auto Projectile = UGameplayStatics::BeginDeferredActorSpawnFromClass(
		GetWorld(),
		ProjectileClass,
		SpawnTransform);

	// TODO: Set projectile params

	UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
}

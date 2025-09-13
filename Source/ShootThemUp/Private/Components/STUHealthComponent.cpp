// Polah Studios' Game, All Rights Reserved.


#include "Components/STUHealthComponent.h"
#include "Dev/STUFireDamageType.h"
#include "Dev/STUIceDamageType.h"
#include "Engine/World.h"
#include "TimerManager.h"


DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All);

USTUHealthComponent::USTUHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void USTUHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHealth(MaxHealth);

	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage);
	}
}


void USTUHealthComponent::OnTakeAnyDamage(
	AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DagageCauser
)
{
	if (Damage <= 0 || IsDead() || !GetWorld())
	{
		return;
	}

	SetHealth(Health - Damage);

	GetWorld()->GetTimerManager().ClearTimer(RegenerationTimerHandle);

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
	else if (autoRegeneration && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RegenerationTimerHandle, this, &USTUHealthComponent::RegenerationUpdate, RegenerationTickTime, true, RegenerationDelay);
	}
}

void USTUHealthComponent::RegenerationUpdate()
{
	SetHealth(Health + RegenerationModifier);
	OnHealthChanged.Broadcast(Health);

	if (FMath::IsNearlyEqual(Health, MaxHealth) && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenerationTimerHandle);
	}
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);
}

#include "TitanPCWeaponComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"

UTitanPCWeaponComponent::UTitanPCWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTitanPCWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Load weapon data table
    if (WeaponDataTable.IsValid())
    {
        LoadedWeaponDataTable = WeaponDataTable.LoadSynchronous();
    }
    
    // Add starting weapons
    for (const FName& WeaponID : StartingWeapons)
    {
        AddWeapon(WeaponID);
    }
    
    // Switch to first weapon if available
    if (OwnedWeapons.Num() > 0)
    {
        SwitchToWeapon(OwnedWeapons[0].WeaponID);
    }
}

void UTitanPCWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    UpdateFiring(DeltaTime);
    UpdateReloading(DeltaTime);
}

bool UTitanPCWeaponComponent::AddWeapon(FName WeaponID, int32 InitialAmmo, int32 InitialReserveAmmo)
{
    if (WeaponID == NAME_None || HasWeapon(WeaponID))
    {
        return false;
    }
    
    FTitanPCWeaponData WeaponData = GetWeaponData(WeaponID);
    if (WeaponData.WeaponName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon data not found for ID: %s"), *WeaponID.ToString());
        return false;
    }
    
    // Set default ammo values if not specified
    if (InitialAmmo < 0)
    {
        InitialAmmo = WeaponData.MagazineSize;
    }
    
    if (InitialReserveAmmo < 0)
    {
        InitialReserveAmmo = WeaponData.MagazineSize * 3; // 3 magazines worth
    }
    
    FTitanPCWeapon NewWeapon(WeaponID, InitialAmmo, InitialReserveAmmo);
    OwnedWeapons.Add(NewWeapon);
    
    UE_LOG(LogTemp, Warning, TEXT("Added weapon: %s"), *WeaponData.WeaponName.ToString());
    return true;
}

bool UTitanPCWeaponComponent::RemoveWeapon(FName WeaponID)
{
    for (int32 i = 0; i < OwnedWeapons.Num(); i++)
    {
        if (OwnedWeapons[i].WeaponID == WeaponID)
        {
            // If removing current weapon, switch to another
            if (CurrentWeaponID == WeaponID)
            {
                if (OwnedWeapons.Num() > 1)
                {
                    // Switch to next weapon or first if removing last
                    int32 NextIndex = (i + 1) % OwnedWeapons.Num();
                    if (NextIndex == i) NextIndex = (i + OwnedWeapons.Num() - 1) % OwnedWeapons.Num();
                    SwitchToWeapon(OwnedWeapons[NextIndex].WeaponID);
                }
                else
                {
                    CurrentWeaponID = NAME_None;
                    CurrentWeaponIndex = -1;
                }
            }
            
            OwnedWeapons.RemoveAt(i);
            
            // Update current weapon index if necessary
            if (CurrentWeaponIndex > i)
            {
                CurrentWeaponIndex--;
            }
            
            return true;
        }
    }
    
    return false;
}

bool UTitanPCWeaponComponent::SwitchToWeapon(FName WeaponID)
{
    if (WeaponID == NAME_None || WeaponID == CurrentWeaponID)
    {
        return false;
    }
    
    for (int32 i = 0; i < OwnedWeapons.Num(); i++)
    {
        if (OwnedWeapons[i].WeaponID == WeaponID)
        {
            FName OldWeaponID = CurrentWeaponID;
            CurrentWeaponID = WeaponID;
            CurrentWeaponIndex = i;
            
            // Stop current actions
            StopFiring();
            bIsReloading = false;
            
            OnWeaponSwitched.Broadcast(OldWeaponID, CurrentWeaponID);
            BroadcastAmmoChanged();
            
            FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
            UE_LOG(LogTemp, Warning, TEXT("Switched to weapon: %s"), *WeaponData.WeaponName.ToString());
            
            return true;
        }
    }
    
    return false;
}

void UTitanPCWeaponComponent::SwitchToNextWeapon()
{
    if (OwnedWeapons.Num() <= 1) return;
    
    int32 NextIndex = (CurrentWeaponIndex + 1) % OwnedWeapons.Num();
    SwitchToWeapon(OwnedWeapons[NextIndex].WeaponID);
}

void UTitanPCWeaponComponent::SwitchToPreviousWeapon()
{
    if (OwnedWeapons.Num() <= 1) return;
    
    int32 PrevIndex = (CurrentWeaponIndex - 1 + OwnedWeapons.Num()) % OwnedWeapons.Num();
    SwitchToWeapon(OwnedWeapons[PrevIndex].WeaponID);
}

FTitanPCWeapon UTitanPCWeaponComponent::GetCurrentWeapon() const
{
    if (CurrentWeaponIndex >= 0 && CurrentWeaponIndex < OwnedWeapons.Num())
    {
        return OwnedWeapons[CurrentWeaponIndex];
    }
    
    return FTitanPCWeapon();
}

FTitanPCWeaponData UTitanPCWeaponComponent::GetCurrentWeaponData() const
{
    return GetWeaponData(CurrentWeaponID);
}

bool UTitanPCWeaponComponent::HasWeapon(FName WeaponID) const
{
    for (const FTitanPCWeapon& Weapon : OwnedWeapons)
    {
        if (Weapon.WeaponID == WeaponID)
        {
            return true;
        }
    }
    
    return false;
}

TArray<FTitanPCWeapon> UTitanPCWeaponComponent::GetAllWeapons() const
{
    return OwnedWeapons;
}

void UTitanPCWeaponComponent::StartFiring()
{
    if (CanFire())
    {
        bIsFiring = true;
    }
}

void UTitanPCWeaponComponent::StopFiring()
{
    bIsFiring = false;
    BurstShotsFired = 0;
}

bool UTitanPCWeaponComponent::TryFire()
{
    if (!CanFire())
    {
        return false;
    }
    
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    // Check fire rate
    float TimeBetweenShots = 60.0f / WeaponData.FireRate;
    if (CurrentTime - LastFireTime < TimeBetweenShots)
    {
        return false;
    }
    
    // Consume ammo
    if (CurrentWeaponIndex >= 0 && CurrentWeaponIndex < OwnedWeapons.Num())
    {
        FTitanPCWeapon& CurrentWeapon = OwnedWeapons[CurrentWeaponIndex];
        if (CurrentWeapon.CurrentAmmo <= 0)
        {
            return false; // No ammo
        }
        
        CurrentWeapon.CurrentAmmo--;
        BroadcastAmmoChanged();
    }
    
    LastFireTime = CurrentTime;
    BurstShotsFired++;
    
    // Perform the actual firing
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        FVector StartLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 100.0f;
        FVector AimDirection = Character->GetControlRotation().Vector();
        
        if (WeaponData.bHitscan)
        {
            FVector EndLocation = StartLocation + CalculateFireDirection(AimDirection) * WeaponData.Range;
            PerformHitscanTrace(StartLocation, EndLocation);
        }
        // TODO: Add projectile firing logic here
    }
    
    PlayFireEffects();
    ApplyRecoil();
    OnWeaponFired.Broadcast(CurrentWeaponID);
    
    return true;
}

bool UTitanPCWeaponComponent::TryReload()
{
    if (!CanReload())
    {
        return false;
    }
    
    bIsReloading = true;
    ReloadStartTime = GetWorld()->GetTimeSeconds();
    
    PlayReloadEffects();
    
    return true;
}

void UTitanPCWeaponComponent::PerformMeleeAttack()
{
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        FVector StartLocation = Character->GetActorLocation();
        FVector ForwardVector = Character->GetActorForwardVector();
        FVector EndLocation = StartLocation + ForwardVector * MeleeRange;
        
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(GetOwner());
        
        if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, WeaponTraceChannel, QueryParams))
        {
            if (AActor* HitActor = HitResult.GetActor())
            {
                // Apply melee damage
                UGameplayStatics::ApplyPointDamage(
                    HitActor,
                    MeleeDamage,
                    StartLocation,
                    HitResult,
                    Character->GetController(),
                    Character,
                    UDamageType::StaticClass()
                );
                
                UE_LOG(LogTemp, Warning, TEXT("Melee attack hit: %s for %f damage"), *HitActor->GetName(), MeleeDamage);
            }
        }
    }
}

void UTitanPCWeaponComponent::StartAiming()
{
    bIsAiming = true;
}

void UTitanPCWeaponComponent::StopAiming()
{
    bIsAiming = false;
}

bool UTitanPCWeaponComponent::AddAmmo(FName WeaponID, int32 AmmoAmount)
{
    for (FTitanPCWeapon& Weapon : OwnedWeapons)
    {
        if (Weapon.WeaponID == WeaponID)
        {
            FTitanPCWeaponData WeaponData = GetWeaponData(WeaponID);
            Weapon.CurrentAmmo = FMath::Min(Weapon.CurrentAmmo + AmmoAmount, WeaponData.MagazineSize);
            
            if (WeaponID == CurrentWeaponID)
            {
                BroadcastAmmoChanged();
            }
            
            return true;
        }
    }
    
    return false;
}

bool UTitanPCWeaponComponent::AddReserveAmmo(FName WeaponID, int32 AmmoAmount)
{
    for (FTitanPCWeapon& Weapon : OwnedWeapons)
    {
        if (Weapon.WeaponID == WeaponID)
        {
            Weapon.ReserveAmmo += AmmoAmount;
            
            if (WeaponID == CurrentWeaponID)
            {
                BroadcastAmmoChanged();
            }
            
            return true;
        }
    }
    
    return false;
}

int32 UTitanPCWeaponComponent::GetCurrentAmmo() const
{
    FTitanPCWeapon CurrentWeapon = GetCurrentWeapon();
    return CurrentWeapon.CurrentAmmo;
}

int32 UTitanPCWeaponComponent::GetReserveAmmo() const
{
    FTitanPCWeapon CurrentWeapon = GetCurrentWeapon();
    return CurrentWeapon.ReserveAmmo;
}

int32 UTitanPCWeaponComponent::GetMaxAmmo() const
{
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    return WeaponData.MagazineSize;
}

bool UTitanPCWeaponComponent::CanFire() const
{
    if (CurrentWeaponID == NAME_None || bIsReloading)
    {
        return false;
    }
    
    FTitanPCWeapon CurrentWeapon = GetCurrentWeapon();
    if (CurrentWeapon.CurrentAmmo <= 0)
    {
        return false;
    }
    
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    
    // Check burst fire limits
    if (WeaponData.FireMode == ETitanPCFireMode::Burst)
    {
        return BurstShotsFired < WeaponData.BurstCount;
    }
    
    return true;
}

bool UTitanPCWeaponComponent::CanReload() const
{
    if (CurrentWeaponID == NAME_None || bIsReloading)
    {
        return false;
    }
    
    FTitanPCWeapon CurrentWeapon = GetCurrentWeapon();
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    
    return CurrentWeapon.CurrentAmmo < WeaponData.MagazineSize && CurrentWeapon.ReserveAmmo > 0;
}

float UTitanPCWeaponComponent::GetReloadProgress() const
{
    if (!bIsReloading)
    {
        return 0.0f;
    }
    
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    float ElapsedTime = GetWorld()->GetTimeSeconds() - ReloadStartTime;
    return FMath::Clamp(ElapsedTime / WeaponData.ReloadTime, 0.0f, 1.0f);
}

void UTitanPCWeaponComponent::UpdateFiring(float DeltaTime)
{
    if (!bIsFiring || CurrentWeaponID == NAME_None)
    {
        return;
    }
    
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    
    // Handle automatic firing
    if (WeaponData.FireMode == ETitanPCFireMode::FullAuto)
    {
        TryFire();
    }
    else if (WeaponData.FireMode == ETitanPCFireMode::Burst)
    {
        if (BurstShotsFired < WeaponData.BurstCount)
        {
            TryFire();
        }
        else
        {
            // Reset burst after a delay
            float CurrentTime = GetWorld()->GetTimeSeconds();
            if (CurrentTime - LastFireTime > 0.5f) // Half second between bursts
            {
                BurstShotsFired = 0;
            }
        }
    }
}

void UTitanPCWeaponComponent::UpdateReloading(float DeltaTime)
{
    if (!bIsReloading)
    {
        return;
    }
    
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    float ElapsedTime = GetWorld()->GetTimeSeconds() - ReloadStartTime;
    
    if (ElapsedTime >= WeaponData.ReloadTime)
    {
        // Complete reload
        if (CurrentWeaponIndex >= 0 && CurrentWeaponIndex < OwnedWeapons.Num())
        {
            FTitanPCWeapon& CurrentWeapon = OwnedWeapons[CurrentWeaponIndex];
            
            int32 AmmoNeeded = WeaponData.MagazineSize - CurrentWeapon.CurrentAmmo;
            int32 AmmoToReload = FMath::Min(AmmoNeeded, CurrentWeapon.ReserveAmmo);
            
            CurrentWeapon.CurrentAmmo += AmmoToReload;
            CurrentWeapon.ReserveAmmo -= AmmoToReload;
            
            BroadcastAmmoChanged();
        }
        
        bIsReloading = false;
        OnWeaponReloaded.Broadcast(CurrentWeaponID);
        
        UE_LOG(LogTemp, Warning, TEXT("Reload completed for weapon: %s"), *WeaponData.WeaponName.ToString());
    }
}

void UTitanPCWeaponComponent::PerformHitscanTrace(const FVector& StartLocation, const FVector& EndLocation)
{
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, WeaponTraceChannel, QueryParams);
    
    if (bHit)
    {
        if (AActor* HitActor = HitResult.GetActor())
        {
            FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
            
            // Apply damage
            UGameplayStatics::ApplyPointDamage(
                HitActor,
                WeaponData.Damage,
                StartLocation,
                HitResult,
                Cast<ACharacter>(GetOwner())->GetController(),
                GetOwner(),
                UDamageType::StaticClass()
            );
            
            UE_LOG(LogTemp, Warning, TEXT("Hit %s for %f damage"), *HitActor->GetName(), WeaponData.Damage);
        }
    }
    
    // Debug line
    //DrawDebugLine(GetWorld(), StartLocation, bHit ? HitResult.Location : EndLocation, FColor::Red, false, 1.0f);
}

void UTitanPCWeaponComponent::ApplyRecoil()
{
    // TODO: Apply camera/aim recoil based on weapon data
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    UE_LOG(LogTemp, Log, TEXT("Applying recoil: %f"), WeaponData.Recoil);
}

void UTitanPCWeaponComponent::PlayFireEffects()
{
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    
    // Play fire sound
    if (WeaponData.FireSound.IsValid())
    {
        USoundBase* FireSound = WeaponData.FireSound.LoadSynchronous();
        if (FireSound)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetOwner()->GetActorLocation());
        }
    }
    
    // Play muzzle flash effect
    if (WeaponData.MuzzleFlash.IsValid())
    {
        UParticleSystem* MuzzleFlash = WeaponData.MuzzleFlash.LoadSynchronous();
        if (MuzzleFlash)
        {
            FVector MuzzleLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.0f;
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleLocation);
        }
    }
}

void UTitanPCWeaponComponent::PlayReloadEffects()
{
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    
    // Play reload sound
    if (WeaponData.ReloadSound.IsValid())
    {
        USoundBase* ReloadSound = WeaponData.ReloadSound.LoadSynchronous();
        if (ReloadSound)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound, GetOwner()->GetActorLocation());
        }
    }
}

FVector UTitanPCWeaponComponent::CalculateFireDirection(const FVector& AimDirection)
{
    FTitanPCWeaponData WeaponData = GetCurrentWeaponData();
    
    // Calculate spread
    float SpreadAmount = WeaponData.Spread;
    if (bIsAiming)
    {
        SpreadAmount *= AimSpreadReduction;
    }
    
    // Add random spread
    FVector SpreadDirection = AimDirection;
    if (SpreadAmount > 0.0f)
    {
        SpreadDirection = FMath::VRandCone(AimDirection, FMath::DegreesToRadians(SpreadAmount));
    }
    
    return SpreadDirection;
}

void UTitanPCWeaponComponent::BroadcastAmmoChanged()
{
    FTitanPCWeapon CurrentWeapon = GetCurrentWeapon();
    OnAmmoChanged.Broadcast(CurrentWeapon.CurrentAmmo, CurrentWeapon.ReserveAmmo);
}

FTitanPCWeaponData UTitanPCWeaponComponent::GetWeaponData(FName WeaponID) const
{
    if (LoadedWeaponDataTable && WeaponID != NAME_None)
    {
        if (FTitanPCWeaponData* Data = LoadedWeaponDataTable->FindRow<FTitanPCWeaponData>(WeaponID, TEXT("GetWeaponData")))
        {
            return *Data;
        }
    }
    
    // Return default data if not found
    FTitanPCWeaponData DefaultData;
    DefaultData.WeaponName = FText::FromName(WeaponID);
    return DefaultData;
}
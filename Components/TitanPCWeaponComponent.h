#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "TitanPCWeaponComponent.generated.h"

UENUM(BlueprintType)
enum class ETitanPCWeaponType : uint8
{
    None            UMETA(DisplayName = "None"),
    Rifle           UMETA(DisplayName = "Assault Rifle"),
    Pistol          UMETA(DisplayName = "Pistol"),
    Shotgun         UMETA(DisplayName = "Shotgun"),
    SMG             UMETA(DisplayName = "SMG"),
    Sniper          UMETA(DisplayName = "Sniper Rifle"),
    Melee           UMETA(DisplayName = "Melee"),
    Grenade         UMETA(DisplayName = "Grenade")
};

UENUM(BlueprintType)
enum class ETitanPCFireMode : uint8
{
    Single          UMETA(DisplayName = "Single Fire"),
    Burst           UMETA(DisplayName = "Burst Fire"),
    FullAuto        UMETA(DisplayName = "Full Auto")
};

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCWeaponData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    FText WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    ETitanPCWeaponType WeaponType = ETitanPCWeaponType::Rifle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    ETitanPCFireMode FireMode = ETitanPCFireMode::Single;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float Damage = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float Range = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float FireRate = 600.0f; // Rounds per minute

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 MagazineSize = 30;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float ReloadTime = 2.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float Spread = 1.0f; // Degrees

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float Recoil = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 BurstCount = 3; // For burst fire

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    bool bHitscan = true; // vs projectile

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
    TSoftObjectPtr<UStaticMesh> WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
    TSoftObjectPtr<USoundBase> FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
    TSoftObjectPtr<USoundBase> ReloadSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
    TSoftObjectPtr<UParticleSystem> MuzzleFlash;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
    FGameplayTagContainer WeaponTags;

    FTitanPCWeaponData()
    {
        WeaponName = FText::FromString("Default Weapon");
        WeaponType = ETitanPCWeaponType::Rifle;
        FireMode = ETitanPCFireMode::Single;
        Damage = 30.0f;
        Range = 1000.0f;
        FireRate = 600.0f;
        MagazineSize = 30;
        ReloadTime = 2.5f;
        Spread = 1.0f;
        Recoil = 2.0f;
        BurstCount = 3;
        bHitscan = true;
    }
};

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCWeapon
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    FName WeaponID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 CurrentAmmo = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 ReserveAmmo = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float Condition = 100.0f; // Weapon durability

    FTitanPCWeapon()
    {
        WeaponID = NAME_None;
        CurrentAmmo = 0;
        ReserveAmmo = 0;
        Condition = 100.0f;
    }

    FTitanPCWeapon(FName InWeaponID, int32 InCurrentAmmo, int32 InReserveAmmo)
    {
        WeaponID = InWeaponID;
        CurrentAmmo = InCurrentAmmo;
        ReserveAmmo = InReserveAmmo;
        Condition = 100.0f;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFired, FName, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloaded, FName, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponSwitched, FName, OldWeaponID, FName, NewWeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, ReserveAmmo);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TITANPC_API UTitanPCWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTitanPCWeaponComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Weapon Management
    UFUNCTION(BlueprintCallable, Category = "Weapons")
    bool AddWeapon(FName WeaponID, int32 InitialAmmo = -1, int32 InitialReserveAmmo = -1);

    UFUNCTION(BlueprintCallable, Category = "Weapons")
    bool RemoveWeapon(FName WeaponID);

    UFUNCTION(BlueprintCallable, Category = "Weapons")
    bool SwitchToWeapon(FName WeaponID);

    UFUNCTION(BlueprintCallable, Category = "Weapons")
    void SwitchToNextWeapon();

    UFUNCTION(BlueprintCallable, Category = "Weapons")
    void SwitchToPreviousWeapon();

    UFUNCTION(BlueprintPure, Category = "Weapons")
    FTitanPCWeapon GetCurrentWeapon() const;

    UFUNCTION(BlueprintPure, Category = "Weapons")
    FTitanPCWeaponData GetCurrentWeaponData() const;

    UFUNCTION(BlueprintPure, Category = "Weapons")
    bool HasWeapon(FName WeaponID) const;

    UFUNCTION(BlueprintPure, Category = "Weapons")
    TArray<FTitanPCWeapon> GetAllWeapons() const;

    // Combat Actions
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartFiring();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopFiring();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool TryFire();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool TryReload();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformMeleeAttack();

    // Aiming
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartAiming();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopAiming();

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsAiming() const { return bIsAiming; }

    // Ammo Management
    UFUNCTION(BlueprintCallable, Category = "Ammo")
    bool AddAmmo(FName WeaponID, int32 AmmoAmount);

    UFUNCTION(BlueprintCallable, Category = "Ammo")
    bool AddReserveAmmo(FName WeaponID, int32 AmmoAmount);

    UFUNCTION(BlueprintPure, Category = "Ammo")
    int32 GetCurrentAmmo() const;

    UFUNCTION(BlueprintPure, Category = "Ammo")
    int32 GetReserveAmmo() const;

    UFUNCTION(BlueprintPure, Category = "Ammo")
    int32 GetMaxAmmo() const;

    // State Queries
    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsFiring() const { return bIsFiring; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsReloading() const { return bIsReloading; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool CanFire() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool CanReload() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetReloadProgress() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnWeaponFired OnWeaponFired;

    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnWeaponReloaded OnWeaponReloaded;

    UPROPERTY(BlueprintAssignable, Category = "Weapons")
    FOnWeaponSwitched OnWeaponSwitched;

    UPROPERTY(BlueprintAssignable, Category = "Ammo")
    FOnAmmoChanged OnAmmoChanged;

protected:
    void UpdateFiring(float DeltaTime);
    void UpdateReloading(float DeltaTime);
    void PerformHitscanTrace(const FVector& StartLocation, const FVector& EndLocation);
    void ApplyRecoil();
    void PlayFireEffects();
    void PlayReloadEffects();
    FVector CalculateFireDirection(const FVector& AimDirection);
    void BroadcastAmmoChanged();

    FTitanPCWeaponData GetWeaponData(FName WeaponID) const;

private:
    // Current weapon state
    UPROPERTY()
    TArray<FTitanPCWeapon> OwnedWeapons;

    UPROPERTY()
    FName CurrentWeaponID = NAME_None;

    int32 CurrentWeaponIndex = -1;

    // Combat state
    bool bIsFiring = false;
    bool bIsReloading = false;
    bool bIsAiming = false;

    // Timers
    float LastFireTime = 0.0f;
    float ReloadStartTime = 0.0f;
    int32 BurstShotsFired = 0;

    // Weapon database
    UPROPERTY(EditDefaultsOnly, Category = "Database")
    TSoftObjectPtr<UDataTable> WeaponDataTable;

    UPROPERTY()
    UDataTable* LoadedWeaponDataTable;

    // Default weapons to start with
    UPROPERTY(EditDefaultsOnly, Category = "Setup")
    TArray<FName> StartingWeapons;

    // Trace settings
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TEnumAsByte<ECollisionChannel> WeaponTraceChannel = ECC_Pawn;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float MeleeRange = 150.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float MeleeDamage = 50.0f;

    // Aiming settings
    UPROPERTY(EditDefaultsOnly, Category = "Aiming")
    float AimSpeedMultiplier = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Aiming")
    float AimSpreadReduction = 0.5f;
};
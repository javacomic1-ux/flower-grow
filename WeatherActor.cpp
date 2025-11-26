#include "WeatherActor.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
AWeatherActor::AWeatherActor()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RainSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RainSystem"));
    SnowSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnowSystem"));

    RainSystem->SetupAttachment(RootComponent);
    SnowSystem->SetupAttachment(RootComponent);

    CurrentWeather = EWeatherType::Sunny;
    UE_LOG(LogTemp, Warning, TEXT("🌞 Weather starts sunny!"));
}

void AWeatherActor::BeginPlay()
{
    Super::BeginPlay();

    // Start sunny (set in constructor already)
    CurrentWeather = EWeatherType::Sunny;
    UE_LOG(LogTemp, Warning, TEXT("🌞 Weather starts sunny!"));

    // Turn off particles
    if (RainSystem) RainSystem->Deactivate();
    if (SnowSystem) SnowSystem->Deactivate();

    // Change weather every 15 seconds
    GetWorldTimerManager().SetTimer(
        WeatherTimerHandle,
        this,
        &AWeatherActor::ChangeWeather,
        15.0f,
        true
    );
}

void AWeatherActor::ChangeWeather()
{
    int32 RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, 2);
    CurrentWeather = static_cast<EWeatherType>(RandomIndex);

    if (RainSystem) RainSystem->Deactivate();
    if (SnowSystem) SnowSystem->Deactivate();

    if (SkyLight)
    {
        USkyLightComponent* SkyComp = SkyLight->GetLightComponent();
        if (SkyComp)
        {
            switch (CurrentWeather)
            {
            case EWeatherType::Sunny:
                SkyComp->SetIntensity(4.0f);
                SkyComp->SetLightColor(FLinearColor(1.0f, 0.95f, 0.85f)); // warm tone
                break;

            case EWeatherType::Rainy:
                SkyComp->SetIntensity(1.5f);
                SkyComp->SetLightColor(FLinearColor(0.75f, 0.8f, 1.0f)); // cooler tone
                if (RainSystem) RainSystem->Activate();
                break;

            case EWeatherType::Snowy:
                SkyComp->SetIntensity(2.0f);
                SkyComp->SetLightColor(FLinearColor(0.9f, 0.9f, 1.0f)); // bluish white
                if (SnowSystem) SnowSystem->Activate();
                break;
            }

            // Refresh sky lighting after changes
            SkyComp->RecaptureSky();
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Weather changed to: %d"), static_cast<uint8>(CurrentWeather));
}

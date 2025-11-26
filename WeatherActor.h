#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "Engine/SkyLight.h"
#include "WeatherActor.generated.h"

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Sunny,
    Rainy,
    Snowy
};

UCLASS()
class MYPROJECT_API AWeatherActor : public AActor
{
    GENERATED_BODY()

public:
    AWeatherActor();

protected:
    virtual void BeginPlay() override;

    // Timer to change weather every 15 seconds
    FTimerHandle WeatherTimerHandle;

    // Function to change weather randomly
    void ChangeWeather();

public:

    bool IsRaining() const { return CurrentWeather == EWeatherType::Rainy; }
    bool IsSnowing() const { return CurrentWeather == EWeatherType::Snowy; }
    bool IsSuning() const { return CurrentWeather == EWeatherType::Sunny; }

    // Current weather state
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    EWeatherType CurrentWeather;

    // Niagara systems for rain and snow
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather")
    UNiagaraComponent* RainSystem;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather")
    UNiagaraComponent* SnowSystem;

    // SkyLight to adjust for different weathers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    ASkyLight* SkyLight;
};
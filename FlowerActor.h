#pragma once
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlowerActor.generated.h"

class AWeatherActor;

class UWBP_MainMenu;

UCLASS()
class MYPROJECT_API AFlowerActor : public AActor
{
    GENERATED_BODY()

public:
    AFlowerActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 🌸 Components
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* FlowerMesh;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* CoverMesh;

    // 🌺 Animations
    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* Sprout0Anim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* Sprout1Anim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* Idle0Anim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* Idle1Anim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* Energy0Anim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* Energy1Anim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* Death0Anim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* Death1Anim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* CoverAnim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* UncoverAnim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* IdleAnim;

    UPROPERTY(EditAnywhere, Category = "Animations")
    UAnimationAsset* EndingAnim;

    // 💗 State
    UPROPERTY(EditAnywhere, Category = "Stats")
    float Health;

    bool bIsCovering;
    bool bIgnoreCoverInput;
    bool bStage0;
    bool bStage1;
    bool bStage2;
    bool bTransition;
    bool bMenu;

    // 🕒 Timer
    FTimerHandle CoverTimerHandle;
    FTimerHandle GrowthTimerHandle;
    FTimerHandle SproutTimerHandle;
    FTimerHandle SwitchHandle;

    // 🔄 Animation tracking
    UAnimationAsset* CurrentAnim;

public:
    void ToggleCovering();
    void HandleCovering();
    void Growth();
    void PlayIdleAnimation();
    void Sprout0play();
    void SproutFinished();
    void PlayIdleCover();
    void Death();
    void Ending();

    UFUNCTION(BlueprintCallable)
    void ShowGameOverScreen();

    UFUNCTION(BlueprintCallable, Category="Gameplay")
    void Start();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void Play();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void RestartGame();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

    // Holds the actual spawned widget instance
    UPROPERTY()
    UUserWidget* GameOverWidgetInstance;


private:
    AWeatherActor* WeatherActor;
};
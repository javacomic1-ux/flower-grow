#include "FlowerActor.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "WeatherActor.h"
#include "UWBP_MainMenu.h"
#include "UWBP_GameOver.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

AFlowerActor::AFlowerActor()
{
    PrimaryActorTick.bCanEverTick = true;

    FlowerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlowerMesh"));
    CoverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CoverMesh"));
    RootComponent = FlowerMesh;

    CoverMesh->SetupAttachment(FlowerMesh);

    Health = 1.0f;
    bIsCovering = true;
    bStage0 = true;
    bStage1 = false;
    bStage2 = false;
    CurrentAnim = nullptr;
    bTransition = false;
}

void AFlowerActor::BeginPlay()
{
    Super::BeginPlay();

    Play();
    CoverMesh->PlayAnimation(UncoverAnim, false);

}

void AFlowerActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!WeatherActor) return;
    if (bMenu) return;

    if (bIsCovering)
    {
        if (WeatherActor->IsSnowing())
        {
            Health = FMath::Clamp(Health - 3.0f * DeltaTime, 0.0f, 10.0f);
        }
        else if (WeatherActor->IsSuning())
        {
            Health = FMath::Clamp(Health + 1.0f * DeltaTime, 0.0f, 10.0f);
        }
        else if (WeatherActor->IsRaining())
        {
            Health = FMath::Clamp(Health + 2.0f * DeltaTime, 0.0f, 10.0f);
        }

        UE_LOG(LogTemp, Warning, TEXT("🌸 Flower Health: %.1f"), Health);
    }

    UAnimationAsset* DesiredAnim = nullptr;

    // Stage-based animation logic
    if (!bStage2 && !bTransition)
    {

        if (bStage0 && !bStage1)
        {
            if (Health <= 0 || Health >= 10) Death();
            else if (Health < 7 && bIsCovering) DesiredAnim = Energy0Anim;
            else DesiredAnim = Idle0Anim;
        }
        else if (bStage1 && !bStage0)
        {
            if (Health <= 0 || Health >= 10) Death();
            else if (Health < 7 && bIsCovering) DesiredAnim = Energy1Anim;
            else DesiredAnim = Idle1Anim;
        }
        else if (bStage2 && !bStage1)
        {
            Ending();
        }

        if (DesiredAnim && CurrentAnim != DesiredAnim)
        {
            FlowerMesh->PlayAnimation(DesiredAnim, true);
            CurrentAnim = DesiredAnim;
        }
    }
}

void AFlowerActor::ToggleCovering()
{
    if (bIgnoreCoverInput) return;

    if (bIsCovering) // start covering
    {
        bIsCovering = false;

        float AnimDuration = CoverAnim->GetPlayLength();
        CoverMesh->PlayAnimation(CoverAnim, false);
        UE_LOG(LogTemp, Warning, TEXT("🌸 Flower is covering!"));

        // Schedule idle only if still covering
        GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFlowerActor::PlayIdleCover, AnimDuration, false);

        GetWorldTimerManager().SetTimer(CoverTimerHandle, this, &AFlowerActor::HandleCovering, 1.0f, true);
    }
    else // start uncovering
    {
        CoverMesh->PlayAnimation(UncoverAnim, false);
        UE_LOG(LogTemp, Warning, TEXT("🌞 Flower is uncovering!"));

        bIsCovering = true;
        GetWorldTimerManager().ClearTimer(CoverTimerHandle);
    }
}

void AFlowerActor::HandleCovering()
{
    if (WeatherActor->IsSuning() || WeatherActor->IsRaining())
    {
        Health = FMath::Clamp(Health - 1.0f, 0.0f, 10.0f);
        UE_LOG(LogTemp, Warning, TEXT("🌸 Flower Health: %.1f"), Health);
    }

}

// Growth stages
void AFlowerActor::Growth()
{
    bStage0 = false;
    bStage1 = true;
    bTransition = true;

    float AnimDuration = Sprout1Anim->GetPlayLength();
    FlowerMesh->PlayAnimation(Sprout1Anim, false);
    UE_LOG(LogTemp, Warning, TEXT("sprout1"));

    GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFlowerActor::SproutFinished, AnimDuration, false);
}

void AFlowerActor::PlayIdleCover()
{
    if (bIsCovering) {
        return;
    }
    else
    {
        CoverMesh->PlayAnimation(IdleAnim, true);
        UE_LOG(LogTemp, Warning, TEXT("switched to idle animation"));
    }
}

void AFlowerActor::SproutFinished()
{
    CurrentAnim = nullptr;
    bTransition = false;

}

void AFlowerActor::Sprout0play()
{
    bTransition = true;
    float AnimDuration = Sprout0Anim->GetPlayLength();
    FlowerMesh->PlayAnimation(Sprout0Anim, false);
    UE_LOG(LogTemp, Warning, TEXT("sprout0"));

    GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFlowerActor::SproutFinished, AnimDuration, false);
}

void AFlowerActor::Death()
{
    bStage2 = true;

    bIgnoreCoverInput = true;

    if (!bIsCovering)
    {
        CoverMesh->PlayAnimation(UncoverAnim, false);
    }

    // Decide which death animation we want
    UAnimationAsset* DeathAnim = (bStage0) ? Death0Anim : Death1Anim;
    float AnimDuration = 0.0f;
    FlowerMesh->PlayAnimation(DeathAnim, false);
    AnimDuration = DeathAnim->GetPlayLength();

    // Spawn Game Over widget
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (GameOverWidgetClass)
        {
            GameOverWidgetInstance = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
            if (GameOverWidgetInstance)
            {
                GameOverWidgetInstance->AddToViewport();

                // Show cursor and focus UI
                PC->bShowMouseCursor = true;
                PC->bEnableClickEvents = true;
                PC->bEnableMouseOverEvents = true;

                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(GameOverWidgetInstance->TakeWidget());
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PC->SetInputMode(InputMode);
            }
        }
    }

    // Schedule showing game over after death animation
    GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFlowerActor::ShowGameOverScreen, AnimDuration, false);

    UE_LOG(LogTemp, Error, TEXT("💀 Flower died!"))
}

void AFlowerActor::Ending()
{

    bStage1 = true;
    bStage0 = true;

    bIgnoreCoverInput = true;
    CoverMesh->PlayAnimation(UncoverAnim, false);

    FlowerMesh->PlayAnimation(EndingAnim, false);

    CoverMesh->PlayAnimation(UncoverAnim, false);

    UE_LOG(LogTemp, Error, TEXT("ending"));

}

void AFlowerActor::ShowGameOverScreen()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->SetPause(true);
    
        if (GameOverWidgetClass)
        {
            UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
            if (GameOverWidget)
            {
                GameOverWidget->AddToViewport(9999);
            }
        }
    }
}

void AFlowerActor::Start()
{
    bMenu = false;

    if (bMenu) return;

    Sprout0play();

    GetWorldTimerManager().SetTimer(GrowthTimerHandle, this, &AFlowerActor::Growth, 30.0f, false);
    GetWorldTimerManager().SetTimer(SproutTimerHandle, this, &AFlowerActor::Ending, 60.0f, false);


    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        EnableInput(PC);
        InputComponent->BindAction("Cover", IE_Pressed, this, &AFlowerActor::ToggleCovering);
    }

    // Finds WeatherActor
    for (TActorIterator<AWeatherActor> It(GetWorld()); It; ++It)
    {
        WeatherActor = *It;
        break;
    }

    UE_LOG(LogTemp, Error, TEXT("game started!"));
}

void AFlowerActor::Play()
{
    if (!MainMenuWidgetClass) return;

    bMenu = true;

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        // Create the widget
        UWBP_MainMenu* MainMenuWidget = CreateWidget<UWBP_MainMenu>(PC, MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            // Assign this FlowerActor to the widget
            MainMenuWidget->FLowerReference = this;

            // Add widget to viewport
            MainMenuWidget->AddToViewport();
        }
    }
}

void AFlowerActor::RestartGame()
{
    bMenu = true;

    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->RemoveFromParent();
        GameOverWidgetInstance = nullptr;
    }

    Play();
}


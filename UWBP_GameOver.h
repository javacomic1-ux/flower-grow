#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWBP_GameOver.generated.h"

UCLASS()
class MYPROJECT_API UWBP_GameOver : public UUserWidget
{
    GENERATED_BODY()

public:
    // Reference to your FlowerActor
    UPROPERTY(BlueprintReadWrite, Category = "References")
    class AFlowerActor* FLowerReference;


    // Function called by the button (e.g., Restart or Retry)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void OnRetryPressed();
};

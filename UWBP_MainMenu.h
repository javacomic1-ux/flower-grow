#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWBP_MainMenu.generated.h"

UCLASS()
class MYPROJECT_API UWBP_MainMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    // Reference to your FlowerActor
    UPROPERTY(BlueprintReadWrite, Category = "References")
    class AFlowerActor* FLowerReference;

    // Function called by the button
    UFUNCTION(BlueprintCallable, Category = "UI")
    void OnPlayPressed(); // <-- make sure name matches your .cpp
};

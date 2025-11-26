#include "UWBP_GameOver.h"
#include "FlowerActor.h"

void UWBP_GameOver::OnRetryPressed()
{
    if (FLowerReference)
    {
        FLowerReference->RestartGame();
    }
}

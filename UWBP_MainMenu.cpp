#include "UWBP_MainMenu.h"
#include "FlowerActor.h"

void UWBP_MainMenu::OnPlayPressed()
{
    if (FLowerReference)
    {
        FLowerReference->Play();
    }
}

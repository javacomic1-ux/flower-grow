#include "WBP_End.h"
#include "FlowerActor.h"

void WBP_End::OnPlayPressed()
{
    if (FLowerReference)
    {
        FLowerReference->Play();
    }
}

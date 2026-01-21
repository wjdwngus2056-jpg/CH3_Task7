#include "TaskPlayerController.h"
#include "EnhancedInputSubsystems.h"

ATaskPlayerController::ATaskPlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    LookAction(nullptr)
{
}

void ATaskPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (TObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer())
    {
        if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }
}

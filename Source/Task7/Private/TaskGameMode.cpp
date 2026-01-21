#include "TaskGameMode.h"
#include "TaskCharacter.h"
#include "TaskPlayerController.h"

ATaskGameMode::ATaskGameMode()
{
    DefaultPawnClass = ATaskCharacter::StaticClass();
    PlayerControllerClass = ATaskPlayerController::StaticClass();
}

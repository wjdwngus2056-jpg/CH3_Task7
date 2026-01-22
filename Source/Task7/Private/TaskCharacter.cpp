#include "TaskCharacter.h"
#include "TaskPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ATaskCharacter::ATaskCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	RootComponent = BoxComp;
	BoxComp->SetSimulatePhysics(false);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComp");
    SkeletalMeshComp->SetupAttachment(BoxComp);
	SkeletalMeshComp->SetSimulatePhysics(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 150.0f;
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	StartLocation = FVector::ZeroVector;

	Velocity = 500.0f;
	SkyVelocityParm = 0.5f;
	RotationSpeed = 140.0f;
	Gravity = 980.0f;
}

void ATaskCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 35.0f));
	StartLocation = GetActorLocation();
}

void ATaskCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector GravityOffset = { 0.f, 0.f, Gravity * DeltaTime * -1.0f };

	if (!isLanding())
	{
		AddActorWorldOffset(GravityOffset, true);
	}
	else
	{
		SetActorRotation(FRotator(0.0f, GetActorRotation().Yaw, 0.0f));
	}

}

void ATaskCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (TObjectPtr<UEnhancedInputComponent> EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (TObjectPtr<ATaskPlayerController> PlayerController = Cast<ATaskPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ATaskCharacter::Move
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ATaskCharacter::Look
				);
			}

			if (PlayerController->UpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->UpAction,
					ETriggerEvent::Triggered,
					this,
					&ATaskCharacter::Up
				);
			}

			if (PlayerController->LandingAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LandingAction,
					ETriggerEvent::Triggered,
					this,
					&ATaskCharacter::Landing
				);
			}
		}
	}
}

void ATaskCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller)
	{
		return;
	}

	const FVector2D MoveInput = Value.Get<FVector2D>();
	float Deltatime = UGameplayStatics::GetWorldDeltaSeconds(this);

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		if (!isLanding())
		{
			AddActorLocalOffset(FVector(MoveInput.X, 0, 0) * SkyVelocityParm * Velocity * Deltatime, true);
		}
		else
		{
			AddActorLocalOffset(FVector(MoveInput.X, 0, 0) * Velocity * Deltatime, true);
		}
		
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddActorLocalOffset(FVector(0, MoveInput.Y, 0) * Velocity * Deltatime, true);
	}
}

void ATaskCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller)
	{
		return;
	}

	const FVector LookInput = Value.Get<FVector>();
	float Deltatime = UGameplayStatics::GetWorldDeltaSeconds(this);
	float YawRotation = LookInput.Y * RotationSpeed * Deltatime;
	float PitchRotation = LookInput.X * RotationSpeed * Deltatime;
	float RollRotation = LookInput.Z * RotationSpeed * Deltatime;
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();

	if (LookInput.IsNearlyZero())
	{
		return;
	}

	if (!isLanding())
	{
		AddActorLocalRotation(FRotator(PitchRotation, YawRotation, RollRotation));
	}
	else
	{
		AddActorLocalRotation(FRotator(0.0f, YawRotation, 0.0f));
		SpringArmRotation.Pitch += PitchRotation;
		SpringArmComp->SetRelativeRotation(SpringArmRotation);
	}
}

void ATaskCharacter::Up(const FInputActionValue& Value)
{
	float Deltatime = UGameplayStatics::GetWorldDeltaSeconds(this);
	float UpVelocity = (SkyVelocityParm * Velocity + Gravity) * Deltatime;
	if (Value.Get<bool>())
	{
		AddActorLocalOffset(FVector(0.0f, 0.0f, UpVelocity));
	}
}

void ATaskCharacter::Landing(const FInputActionValue& Value)
{
	float Deltatime = UGameplayStatics::GetWorldDeltaSeconds(this);
	float DownVelocity = -1.0f * (SkyVelocityParm * Velocity + Gravity) * Deltatime;
	if (Value.Get<bool>())
	{
		if (!isLanding())
		{
			AddActorLocalOffset(FVector(0.0f, 0.0f, DownVelocity));
		}
	}
}

bool ATaskCharacter::isLanding()
{
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 32.f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool Hitland = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	return Hitland;
}


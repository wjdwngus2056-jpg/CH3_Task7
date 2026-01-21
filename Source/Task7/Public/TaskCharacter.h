#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TaskCharacter.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class TASK7_API ATaskCharacter : public APawn
{
	GENERATED_BODY()

public:
	ATaskCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void Up(const FInputActionValue& Value);
	UFUNCTION()
	void Landing(const FInputActionValue& Value);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> BoxComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

protected:	

	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor|Properties")
	float Velocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor|Properties")
	float RotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor|Properties")
	float Gravity;

};

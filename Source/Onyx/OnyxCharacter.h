// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OnyxCharacter.generated.h"

UCLASS(config=Game)
class AOnyxCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* CollectionSphere;
public:
	AOnyxCharacter();

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetInitialStamina();

	UFUNCTION(BlueprintPure, Category = "Stats")
		float  GetCurrentStamina();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		void UpdateStamina(float StaminaUpdate);

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetInitialHealth();

	UFUNCTION(BlueprintPure, Category = "Stats")
		float  GetCurrentHealth();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		void UpdateHealth(float HealthUpdate);


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		bool IsSprinting;
	UFUNCTION(BlueprintCallable, Category = "Stats") void StopCharacterSprint();
	UFUNCTION(BlueprintCallable, Category = "Stats") void CharacterSprint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float SpeedFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float BaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float InitialStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float InitialHealth;
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);


	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Stats")
		float CharacterStamina;
	UPROPERTY(VisibleAnywhere, Category = "Stats")
		float CharacterHealth;
};
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OnyxCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Classes/Components/SphereComponent.h"

//////////////////////////////////////////////////////////////////////////
// AOnyxCharacter

AOnyxCharacter::AOnyxCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);


	SpeedFactor = 1.5f;

	SpeedFactor = 1;

	BaseSpeed = GetCharacterMovement()->MaxWalkSpeed;
	IsSprinting = false;
	PrimaryActorTick.bCanEverTick = true;

	InitialStamina, CharacterStamina = 100.0f;
	InitialHealth, CharacterHealth = 100.0f;
	InitialShards, CharacterShards = 0.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AOnyxCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AOnyxCharacter::CharacterSprint);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AOnyxCharacter::StopCharacterSprint);
	PlayerInputComponent->BindAxis("MoveForward", this, &AOnyxCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AOnyxCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AOnyxCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AOnyxCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AOnyxCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AOnyxCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AOnyxCharacter::OnResetVR);
}


void AOnyxCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AOnyxCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AOnyxCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AOnyxCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AOnyxCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AOnyxCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AOnyxCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AOnyxCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (!IsSprinting)
		UpdateHealth(1.0f);
	if (IsSprinting)
	{
		if (GetCurrentStamina() <= 0)
			StopCharacterSprint();
		else UpdateStamina(-.5f);
	}
	if (GetCurrentStamina() < 100 && !IsSprinting)
		UpdateStamina(.2f);  //CHANGE STAMINA RATE
}

void AOnyxCharacter::CharacterSprint() { GetCharacterMovement()->MaxWalkSpeed *= SpeedFactor; IsSprinting = true; }

void AOnyxCharacter::StopCharacterSprint() { GetCharacterMovement()->MaxWalkSpeed = BaseSpeed; IsSprinting = false; }

float AOnyxCharacter::GetInitialStamina()
{
	return InitialStamina;
}

float AOnyxCharacter::GetCurrentStamina()
{
	return CharacterStamina;
}

void AOnyxCharacter::UpdateStamina(float StaminaUpdate)
{
	CharacterStamina = GetCurrentStamina() + StaminaUpdate;
	if (GetCurrentStamina() > 100)
		CharacterStamina = 100;
	if (GetCurrentStamina() < 0)
		CharacterStamina = 0;
}

float AOnyxCharacter::GetInitialHealth()
{
	return InitialHealth;
}

float AOnyxCharacter::GetCurrentHealth()
{
	return CharacterHealth;
}

void AOnyxCharacter::UpdateHealth(float HealthUpdate)
{
	CharacterHealth = GetCurrentHealth() + HealthUpdate;
	if (GetCurrentHealth() > 100)
		CharacterHealth = 100;
	if (GetCurrentHealth() < 0)
		CharacterHealth = 0;
}

float AOnyxCharacter::GetCurrentShards()
{
	return CharacterShards;
}
void AOnyxCharacter::UpdateShards(float ShardUpdate)
{
	if (ShardUpdate == 4)
		//EndGame
		CharacterShards += ShardUpdate;
}
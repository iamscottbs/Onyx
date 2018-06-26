// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpBase.h"
#include "ItemPickup.generated.h"

/**
 * 
 */
UCLASS()
class ONYX_API AItemPickup : public APickUpBase
{
	GENERATED_BODY()
public:
	AItemPickup();

	UFUNCTION(BlueprintPure, Category = "Pickup")
		float GetPower();

	UFUNCTION(BlueprintPure, Category = "Pickup")
		FString GetType();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (BlueprintProtected = "true"))
		float PickupPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (BlueprintProtected = "true"))
		FString PickupType;

};

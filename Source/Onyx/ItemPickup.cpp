// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemPickup.h"

//Set default values
AItemPickup::AItemPickup() {

	//The base power of the pickup
	PickupPower = 1.f;

	//Initialize the pickup type name
	PickupType = TEXT("");


}

float AItemPickup::GetPower() {
	return PickupPower;
}



//Return the pickup type
FString AItemPickup::GetType() {
	return PickupType;
}



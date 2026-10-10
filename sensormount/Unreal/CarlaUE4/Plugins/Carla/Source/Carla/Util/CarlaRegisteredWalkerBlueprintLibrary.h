// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "CarlaRegisteredWalkerBlueprintLibrary.generated.h"

class AActor;
class ACharacter;

UCLASS()
class CARLA_API UCarlaRegisteredWalkerBlueprintLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:

  UFUNCTION(BlueprintCallable, Category="CARLA|Walker", meta=(WorldContext="WorldContextObject"))
  static ACharacter *SpawnRegisteredWalkerFemale1V1(
      const UObject *WorldContextObject,
      const FTransform &SpawnTransform,
      const FString &RoleName);

  UFUNCTION(BlueprintCallable, Category="CARLA|Prop", meta=(WorldContext="WorldContextObject"))
  static AActor *SpawnRegisteredSensorMount(
      const UObject *WorldContextObject,
      const FTransform &SpawnTransform,
      const FString &RoleName);

  UFUNCTION(BlueprintPure, Category="CARLA|Walker", meta=(WorldContext="WorldContextObject"))
  static int32 GetCarlaActorId(
      const UObject *WorldContextObject,
      const AActor *Actor);
};

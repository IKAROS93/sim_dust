// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Util/CarlaRegisteredWalkerBlueprintLibrary.h"

#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Actor/ActorDescription.h"
#include "Carla/Actor/CarlaActor.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Game/CarlaGameModeBase.h"
#include "Carla/Game/CarlaStatics.h"

#include "GameFramework/Character.h"

namespace
{
  static constexpr TCHAR WalkerFemale1V1ActorId[] = TEXT("walker.pedestrian.0001");
  static constexpr TCHAR WalkerFemale1V1ClassPath[] =
      TEXT("/Game/Carla/Blueprints/Walkers/BP_Walker_Female1_v1.BP_Walker_Female1_v1_C");
  static constexpr TCHAR SensorMountActorId[] = TEXT("static.prop.sensormount");

  TSubclassOf<ACharacter> LoadWalkerFemale1V1Class()
  {
    return LoadClass<ACharacter>(nullptr, WalkerFemale1V1ClassPath);
  }

  const FActorDefinition *FindDefinitionByIdAndClass(
      const TArray<FActorDefinition> &Definitions,
      const TSubclassOf<ACharacter> &TargetClass)
  {
    if (TargetClass == nullptr)
    {
      return nullptr;
    }

    const FString TargetPath = TargetClass->GetPathName();
    for (const FActorDefinition &Definition : Definitions)
    {
      if (Definition.Class == nullptr)
      {
        continue;
      }

      if ((Definition.Id == WalkerFemale1V1ActorId) &&
          (Definition.Class->GetPathName() == TargetPath))
      {
        return &Definition;
      }
    }
    return nullptr;
  }

  const FActorDefinition *FindDefinitionById(
      const TArray<FActorDefinition> &Definitions,
      const FString &TargetId)
  {
    for (const FActorDefinition &Definition : Definitions)
    {
      if (Definition.Id == TargetId)
      {
        return &Definition;
      }
    }
    return nullptr;
  }

  FActorDescription MakeDescriptionFromDefinition(
      const FActorDefinition &Definition,
      const FString &RoleNameOverride)
  {
    FActorDescription Description;
    Description.UId = Definition.UId;
    Description.Id = Definition.Id;
    Description.Class = Definition.Class;

    for (const FActorVariation &Variation : Definition.Variations)
    {
      FActorAttribute Attribute;
      Attribute.Id = Variation.Id;
      Attribute.Type = Variation.Type;
      if (Variation.RecommendedValues.Num() > 0)
      {
        Attribute.Value = Variation.RecommendedValues[0];
      }

      if (Attribute.Id == TEXT("role_name") && !RoleNameOverride.IsEmpty())
      {
        Attribute.Value = RoleNameOverride;
      }

      Description.Variations.Emplace(Attribute.Id, Attribute);
    }

    return Description;
  }

  UCarlaEpisode *ResolveEpisode(const UObject *WorldContextObject)
  {
    if (UCarlaEpisode *Episode = UCarlaStatics::GetCurrentEpisode(WorldContextObject))
    {
      return Episode;
    }

    if (ACarlaGameModeBase *GameMode = UCarlaStatics::GetGameMode(WorldContextObject))
    {
      return const_cast<UCarlaEpisode *>(&GameMode->GetCarlaEpisode());
    }

    return nullptr;
  }
} // namespace

ACharacter *UCarlaRegisteredWalkerBlueprintLibrary::SpawnRegisteredWalkerFemale1V1(
    const UObject *WorldContextObject,
    const FTransform &SpawnTransform,
    const FString &RoleName)
{
  UCarlaEpisode *Episode = ResolveEpisode(WorldContextObject);
  if (Episode == nullptr)
  {
    UE_LOG(LogCarla, Error, TEXT("SpawnRegisteredWalkerFemale1V1 failed: no active CARLA episode"));
    return nullptr;
  }

  TSubclassOf<ACharacter> WalkerClass = LoadWalkerFemale1V1Class();
  if (WalkerClass == nullptr)
  {
    UE_LOG(LogCarla, Error, TEXT("SpawnRegisteredWalkerFemale1V1 failed: could not load '%s'"), WalkerFemale1V1ClassPath);
    return nullptr;
  }

  const FActorDefinition *Definition = FindDefinitionByIdAndClass(Episode->GetActorDefinitions(), WalkerClass);
  if (Definition == nullptr)
  {
    UE_LOG(
        LogCarla,
        Error,
        TEXT("SpawnRegisteredWalkerFemale1V1 failed: no registered actor definition found for id '%s' and class '%s'"),
        WalkerFemale1V1ActorId,
        *WalkerClass->GetPathName());
    return nullptr;
  }

  FActorDescription Description = MakeDescriptionFromDefinition(*Definition, RoleName);
  const auto Result = Episode->SpawnActorWithInfo(SpawnTransform, Description);
  if (Result.Key != EActorSpawnResultStatus::Success || Result.Value == nullptr)
  {
    UE_LOG(
        LogCarla,
        Error,
        TEXT("SpawnRegisteredWalkerFemale1V1 failed to spawn '%s' (status=%s)"),
        *Definition->Id,
        *FActorSpawnResult::StatusToString(Result.Key));
    return nullptr;
  }

  return Cast<ACharacter>(Result.Value->GetActor());
}

AActor *UCarlaRegisteredWalkerBlueprintLibrary::SpawnRegisteredSensorMount(
    const UObject *WorldContextObject,
    const FTransform &SpawnTransform,
    const FString &RoleName)
{
  UCarlaEpisode *Episode = ResolveEpisode(WorldContextObject);
  if (Episode == nullptr)
  {
    UE_LOG(LogCarla, Error, TEXT("SpawnRegisteredSensorMount failed: no active CARLA episode"));
    return nullptr;
  }

  const FActorDefinition *Definition = FindDefinitionById(Episode->GetActorDefinitions(), SensorMountActorId);
  if (Definition == nullptr)
  {
    UE_LOG(
        LogCarla,
        Error,
        TEXT("SpawnRegisteredSensorMount failed: no registered actor definition found for id '%s'"),
        SensorMountActorId);
    return nullptr;
  }

  FActorDescription Description = MakeDescriptionFromDefinition(*Definition, RoleName);
  const auto Result = Episode->SpawnActorWithInfo(SpawnTransform, Description);
  if (Result.Key != EActorSpawnResultStatus::Success || Result.Value == nullptr)
  {
    UE_LOG(
        LogCarla,
        Error,
        TEXT("SpawnRegisteredSensorMount failed to spawn '%s' (status=%s)"),
        *Definition->Id,
        *FActorSpawnResult::StatusToString(Result.Key));
    return nullptr;
  }

  return Result.Value->GetActor();
}

int32 UCarlaRegisteredWalkerBlueprintLibrary::GetCarlaActorId(
    const UObject *WorldContextObject,
    const AActor *Actor)
{
  if (Actor == nullptr)
  {
    return 0;
  }

  UCarlaEpisode *Episode = ResolveEpisode(WorldContextObject);
  if (Episode == nullptr)
  {
    return 0;
  }

  FCarlaActor *CarlaActor = Episode->FindCarlaActor(const_cast<AActor *>(Actor));
  return CarlaActor != nullptr ? static_cast<int32>(CarlaActor->GetActorId()) : 0;
}

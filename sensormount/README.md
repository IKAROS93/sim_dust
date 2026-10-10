# Sensor Attachment in CARLA Using a Dedicated Mount

We share a sensor attachment method for CARLA that uses a dedicated sensor mount as the parent Actor. This replaces the workaround of using a pedestrian Actor as a sensor carrier. The mount can be spawned from a UE4 Blueprint and is registered with CARLA, allowing it to be used in an existing sensor attachment workflow.

`Spawn Registered Sensor Mount` creates `static.prop.sensormount` at the specified pose, sets its role name, and returns the registered Actor. Sensor creation and attachment are handled separately through your existing workflow.

## Installation

Use a compatible CARLA source build. The intended environment is CARLA 0.9.12 with UE4 4.26; this package has not been tested in a clean third-party project.

1. Close the UE4 editor. Copy this folder's `Unreal/` contents into the matching directories of your CARLA source checkout. Back up any existing files with the same names.

   ```text
   Unreal/CarlaUE4/
   ├── Plugins/Carla/Source/Carla/Util/
   │   ├── CarlaRegisteredWalkerBlueprintLibrary.h
   │   └── CarlaRegisteredWalkerBlueprintLibrary.cpp
   └── Content/Carla/
       ├── Config/SensorMount.Package.json
       └── Static/SM_sensormount.uasset
   ```

2. Rebuild CARLA. On the configured Linux source build, run `make launch` from the CARLA root.
3. Open `SM_sensormount` in the Content Browser, assign any available material in **Material Slots**, and save. Its original local material is not included.

The separate JSON file registers the mount; there is no need to replace `Default.Package.json`.

## Blueprint usage

Search for **Spawn Registered Sensor Mount** under **CARLA → Prop** and connect it to your execution flow, such as `Event BeginPlay`.

- **Spawn Transform:** the mount's world pose (UE4 position units: centimeters).
- **Role Name:** a name matching your setup, for example `sensor_mount`.
- **Return Value:** the mount Actor. Check it with **Is Valid** before use.

Use **Get Carla Actor Id** with the returned Actor to check registration; a nonzero ID indicates success. To follow a vehicle or machinery component, attach the returned Actor using **Attach Actor To Component**.

In your existing CARLA ROS Bridge workflow, create the sensors and attach them to the registered mount instead of a pedestrian Actor. This function does not automatically create sensors or publish ROS topics.

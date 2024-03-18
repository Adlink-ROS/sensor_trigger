# sensor_trigger

## Introduction

This repository provides a ROS2 package for generating sensor trigger signals on a Jetson AGX Orin. It is specifically designed for use with the ADLINK ROScube RQX-59G, providing pin mapping for the GPIO which are internally connected to the FPGA which controls FSYNC for the built-in GMSL deserializers.

## Requirements

- ECU: RQX-59G (Jetson AGX Orin)
  - The software in this package is pre-configured for use with ROScube RQX-59G from ADLINK.
- OS: Ubuntu 20.04 or higher
- ROS2: Galactic Geochelone

## Installation

1. Create or change into your workspace directory, and execute the following:

    ```bash
    mkdir -p src
    git clone git@github.com:tier4/sensor_trigger.git src
    colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release --packages-up-to sensor_trigger
    ```

2. Copy the ROScube udev rules to the right place and then reboot system to take effect.

    ```bash
    cd sensor_trigger
    sudo cp 99-roscube-gpio.rules /dev/udev/rules.d/
    sudo reboot
    ```
    
    - If you do not copy the udev rules, you will have no permission to access the FSYNC GPIO.

3. The sensor trigger node requires as close to real-time operation as possible to maintain reliable trigger timing under heavy CPU load. If this is required, it is recommended to allow thread schedule priority setting to the ROS2 user by adding the following line to `/etc/security/limits.conf`:

    ```
    <username>    -   rtprio   98
    ```

    After saving the edited file (as sudo), a reboot will be required.

    Some notes about thread scheduling:
    - If you do not make the settings in (2), the node will run but the timing thread will not be scheduled at any higher priority that other user threads and interruption may occur, resulting in high jitter in the trigger output.
    
    - When running in a ROS2 docker, the settings in (2) are not required as the docker user is by default the root user.

## Usage

The sensor trigger node will output trigger pulses on the specified GPIO pin at the configured frequency.
The phase parameter is used to control the timing of the pulse relative to Top of Second (ToS), allowing fine-grained control of the trigger timing relative to other sensors and ECUs when all connected devices have been synchronized using Precision Time Protocol (PTP).

Setting the CPU core for operation is recommended to improve timing stability. As CPU 0 is used for system interrupts, a CPU core >0 is recommended. If multiple sensor trigger nodes are instantiated, make sure they each use a different CPU core. You can check how many CPU cores are available on your system with `nproc --all`.

The node can be launched with the default parameters as follows:

```bash
ros2 launch sensor_trigger sensor_trigger.launch
```

## Inputs / Outputs

### Input

This node does not take any inputs.

### Output

| Name             | Type                            | Description                                              |
| ---------------- | ------------------------------- | -------------------------------------------------------- |
| `~/trigger_time` | `builtin_interfaces::msg::Time` | Time that the trigger output was requested (system time) |

## Parameters

### Core Parameters

| Name          | Type   | Description                                                  |
| ------------- | ------ | ------------------------------------------------------------ |
| `fsync_index`        | int    | Output FSYNC GPIO pin - see below for assigned pins on RQX-59G     |
| `phase`       | double | Desired phase of the trigger relative to ToS (Top of Second) |
| `frame_rate`  | double | Desired frequency of the trigger in Hz                       |
| `cpu_core_id` | int    | Desired CPU core for execution\*                             |
| `pulse_width_ms` | int | Desired pulse width (the time period in milliseconds for the GPIO pin to remain high) |

\*This is indexed from CPU core 0 (which will be CPU 1 in `htop`).

### Included Pin Mappings (RQX-59G)

| Pin | GPIO (sysfs) | Description                                                             |
| --- | ------------ | ----------------------------------------------------------------------- |
| 0  | gpio-440 (PP.00)          | FSYNC-0 on the RQX-59G, internally connected to FSYNC on deserializer #1 |
| 1  | gpio-397 (PH.06)          | FSYNC-1 on the RQX-59G, internally connected to FSYNC on deserializer #2 |
| 2  | gpio-487 (PAC.01)          | FSYNC-2 on the RQX-59G, internally connected to FSYNC on deserializer #3 |
| 3  | gpio-486 (PAC.00)          | FSYNC-2 on the RQX-59G, internally connected to FSYNC on deserializer #4 |

## Related Repositories

- [tier4/perception_ecu_container](https://github.com/tier4/perception_ecu_container)
  - Meta-repository containing `.repos` file to construct perception-ecu workspace
- [tier4/perception_ecu_launch](https://github.com/tier4/perception_ecu_launch.git)
- [tier4/ros2_v4l2_camera](https://github.com/tier4/ros2_v4l2_camera.git)
- [tier4/perception_ecu_individual_params](https://github.com/tier4/perception_ecu_individual_params)
- [autowarefoundation/autoware.universe](https://github.com/autowarefoundation/autoware.universe.git)

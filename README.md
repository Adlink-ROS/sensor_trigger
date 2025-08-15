# sensor_trigger

## Introduction

This repository provides a ROS 2 package for generating sensor trigger signals on a Jetson AGX Orin. It is specifically designed for use with the ADLINK ROScube RQX-59G, providing pin mapping for the GPIO which are internally connected to the FPGA which controls FSYNC for the built-in GMSL deserializers.

## Requirements

- ECU: RQX-59G (Jetson AGX Orin)
  - The software in this package is pre-configured for use with ROScube RQX-59G from ADLINK.
- OS: Ubuntu 22.04 (JetPack 6.1 / L4T R36.4)
- ROS 2: Humble

## Installation

1. Create or change into your workspace directory, and execute the following:

    ```bash
    # Create a workspace and download the repo
    mkdir -p ~/ros2_ws/src
    cd ~/ros2_ws/src
    git clone https://github.com/Adlink-ROS/sensor_trigger.git -b l4t-r36.4
    
    # Install the dependent packages
    cd ~/ros2_ws/
    rosdep install --from-paths src --ignore-src -r -y
    
    # Compile the repo
    colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release --packages-up-to sensor_trigger
    ```

2. Grant $USER permission to acess GPIO:

    ```bash
    sudo groupadd gpio
    sudo usermod -aG gpio $USER
    ```
    
    - The changes will take effect after a reboot.
    
3. The sensor trigger node requires as close to real-time operation as possible to maintain reliable trigger timing under heavy CPU load. If this is required, it is recommended to allow thread schedule priority setting to the ROS2 user by adding the following line to `/etc/security/limits.conf`:

    ```
    <username>    -   rtprio   98
    ```

    After saving the edited file (as sudo), a reboot will be required.

    Some notes about thread scheduling:
    - If you do not make the settings in (3), the node will run but the timing thread will not be scheduled at any higher priority that other user threads and interruption may occur, resulting in high jitter in the trigger output.
    - If the error `Failed to set schedule parameters` occurs, this indicates a bug introduced after installing NVIDIA JetPack. To work around this issue, execute the following command:
        ```bash
        echo $$ | sudo tee /sys/fs/cgroup/cpu/tasks

        # And then continue run sensor trigger program in this terminal. e.g.
        ros2 launch sensor_trigger multiple_sensor_trigger.launch.xml
        ```
    - When running in a ROS2 docker, the settings in (3) are not required as the docker user is by default the root user.

## Usage

The sensor trigger node will output trigger pulses on the specified GPIO pin at the configured frequency.
The phase parameter is used to control the timing of the pulse relative to Top of Second (ToS), allowing fine-grained control of the trigger timing relative to other sensors and ECUs when all connected devices have been synchronized using Precision Time Protocol (PTP).

Setting the CPU core for operation is recommended to improve timing stability. As CPU 0 is used for system interrupts, a CPU core >0 is recommended. If multiple sensor trigger nodes are instantiated, make sure they each use a different CPU core. You can check how many CPU cores are available on your system with `nproc --all`.

The node can be launched with the specific parameters as follows:

```bash
# Use default parameters
ros2 launch sensor_trigger rqx590_single_trigger.launch.xml

# Run FSYNC2 on CPU5
ros2 launch sensor_trigger rqx590_single_trigger.launch.xml gpio_name:=fsync2 cpu_core_id:=5

# Run FSYNC3 on CPU6 with frequency=20.0 Hz
ros2 launch sensor_trigger rqx590_single_trigger.launch.xml gpio_name:=fsync3 cpu_core_id:=6 frame_rate:=20.0

# Run FSYNC1-FSYNC4 on CPU4-7
ros2 launch sensor_trigger rqx590_multiple_trigger.launch.xml
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
| `gpio_name`        | string    | Output FSYNC GPIO pin - see below for assigned pins on RQX-59G     |
| `phase`       | double | Desired phase of the trigger relative to ToS (Top of Second) |
| `frame_rate`  | double | Desired frequency of the trigger in Hz                       |
| `cpu_core_id` | int    | Desired CPU core for execution\*                             |
| `pulse_width_ms` | int | Desired pulse width (the time period in milliseconds for the GPIO pin to remain high) |

\*This is indexed from CPU core, started from 0. Please use `lscpu` to get online CPU list.

### Included RQX-59G Pin Mappings

| GPIO Name | gpiochip0 | Description                                                             |
| --- | ------------ | ----------------------------------------------------------------------- |
| fsync1  | line 92: PP.00    | FSYNC-1 on the RQX-59G, internally connected to FSYNC on deserializer #1 |
| fsync2  | line 49: PH.06    | FSYNC-2 on the RQX-59G, internally connected to FSYNC on deserializer #2 |
| fsync3  | line 139: PAC.01  | FSYNC-3 on the RQX-59G, internally connected to FSYNC on deserializer #3 |
| fsync4  | line 138: PAC.00  | FSYNC-4 on the RQX-59G, internally connected to FSYNC on deserializer #4 |

## Related Repositories

- [tier4/perception_ecu_container](https://github.com/tier4/perception_ecu_container)
  - Meta-repository containing `.repos` file to construct perception-ecu workspace
- [tier4/perception_ecu_launch](https://github.com/tier4/perception_ecu_launch.git)
- [tier4/ros2_v4l2_camera](https://github.com/tier4/ros2_v4l2_camera.git)
- [tier4/perception_ecu_individual_params](https://github.com/tier4/perception_ecu_individual_params)
- [autowarefoundation/autoware.universe](https://github.com/autowarefoundation/autoware.universe.git)

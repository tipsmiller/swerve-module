# Swerve-testing
This is a project exploring the control of a swerve module using a
linux-based controlling computer networked to two VESC motor controllers.
The motor controllers are in turn controlling one motor each on the
swerve module. There is additionally one hall-effect sensor plugged
into one of the VESC controllers which is used to find the zero
position of the steering motor.

This project is also a learning experience in C++. I'm still very new
to the language, so this project should help me explore the use of
system libraries and gain general familiarity with language features.


# Steps to get CANBUS active:
One interface option for VESC hardware is over CANbus.
Using something like a CANable provides access to CANbus
from a linux USB port.

## manually
- plug in CANable
- Set the network device "up": `sudo ip link set can0 up type can bitrate 500000`

## automatically
- Load kernel modules at boot
  - `/etc/modules-load.d/can.conf` should contain:
  - `can` and `can_raw`
- Use `systemd-networkd` to manage the connection. Make sure the service starts on boot:
  -     `sudo systemctl start systemd-networkd`
  -     `sudo systemctl enable systemd-networkd`
- Set the interface settings:
  - `/etc/systemd/network/80-can.network`
  -     [Match]
        Name=can0
        [CAN]
        BitRate=500K
        RestartSec=100ms
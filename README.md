# Step 1: Clone the QEMU repository

git clone https://github.com/qemu/qemu.git
cd qemu
git checkout v6.0.0
./configure --target-list=riscv64-softmmu
make

# Step 2: Clone the Buildroot repository

git clone https://github.com/buildroot/buildroot.git
cd buildroot/
make qemu_riscv64_virt_defconfig
make

# Step 3: Modify files for the sensor_device

# hw/misc/Kconfig
# Add the following configuration for the sensor_device:
#
# config SENSOR_DEVICE
#     bool

# hw/misc/meson.build
# Add the following for the sensor_device:
#
# softmmu_ss.add(when: 'CONFIG_SENSOR_DEVICE', if_true: files('sensor_device.c'))

# hw/riscv/Kconfig
# Update the configuration for the RISC-V architecture to include sensor_device:
#
# select SENSOR_DEVICE

# hw/riscv/virt.c
# Update the virt.c file to use sensor_device:
#
# #include "hw/misc/sensor_device.h"
#
# [VIRT_SENSOR_DEVICE] = { 0x4000000, 0x100 },
#
# sensor_device_create(memmap[VIRT_SENSOR_DEVICE].base);

# include/hw/misc/sensor_device.h
# Create the sensor_device header file:
#
# #ifndef HW_SENSOR_DEVICE_H
# #define HW_SENSOR_DEVICE_H
# #include "qom/object.h"
#
# DeviceState *sensor_device_create(hwaddr);
#
# #endif

# include/hw/riscv/virt.h
# Update the virtual machine header to include sensor_device:
#
# enum { VIRT_SENSOR_DEVICE };

# Step 4: Testing the virtual device in QEMU

../qemu/build/qemu-system-riscv64 \
  -M virt \
  -bios fw_dynamic.bin \
  -kernel Image \
  -append "rootwait root=/dev/vda ro" \
  -drive file=rootfs.ext2,format=raw,id=hd0 \
  -device virtio-blk-device,drive=hd0 \
  -netdev user,id=net0 \
  -device virtio-net-device,netdev=net0 \
  -nographic

# Step 5: Running the client application

./client

# Step 1: Clone the QEMU repository
# QEMU v6.0.0 Setup for RISC-V 64 Architecture

git clone https://github.com/qemu/qemu.git
cd qemu
git checkout v6.0.0
./configure --target-list=riscv64-softmmu
make


# Buildroot repository
git clone https://github.com/buildroot/buildroot.git
cd buildroot/
make qemu_riscv64_virt_defconfig
make


# Modified Files for the sensor_device

# hw/misc/Kconfig
# Add the following configuration for the sensor_device:
# Original file line number Diff line number Diff line change
# @@ -11,6 +11,9 @@ config ARMSSE_MHU
# config ARMSSE_CPU_PWRCTRL
#     bool
#
# config SENSOR_DEVICE
#     bool
#
# config MAX111X
#     bool

# hw/misc/meson.build
# Add the following for the sensor_device:
# Original file line number Diff line number Diff line change
# @@ -1,4 +1,5 @@
# softmmu_ss.add(when: 'CONFIG_APPLESMC', if_true: files('applesmc.c'))
# softmmu_ss.add(when: 'CONFIG_SENSOR_DEVICE', if_true: files('sensor_device.c'))
# softmmu_ss.add(when: 'CONFIG_EDU', if_true: files('edu.c'))
# softmmu_ss.add(when: 'CONFIG_FW_CFG_DMA', if_true: files('vmcoreinfo.c'))
# softmmu_ss.add(when: 'CONFIG_ISA_DEBUG', if_true: files('debugexit.c'))

# hw/riscv/Kconfig
# Update the configuration for the RISC-V architecture to include sensor_device:
# Original file line number Diff line number Diff line change
# @@ -34,6 +34,7 @@ config RISCV_VIRT
#    select SIFIVE_TEST
#    select VIRTIO_MMIO
#    select FW_CFG_DMA
#    select SENSOR_DEVICE
#
# config SIFIVE_E
#    bool

# hw/riscv/virt.c
# Update the virt.c file to use sensor_device:
# Original file line number Diff line number Diff line change
# @@ -36,6 +36,7 @@
# #include "hw/intc/sifive_clint.h"
# #include "hw/intc/sifive_plic.h"
# #include "hw/misc/sifive_test.h"
# #include "hw/misc/sensor_device.h"
# #include "chardev/char.h"
# #include "sysemu/arch_init.h"
# #include "sysemu/device_tree.h"
# @@ -51,6 +52,7 @@ static const MemMapEntry virt_memmap[] = {
#    [VIRT_RTC] =         {   0x101000,        0x1000 },
#    [VIRT_CLINT] =       {  0x2000000,       0x10000 },
#    [VIRT_PCIE_PIO] =    {  0x3000000,       0x10000 },
#    [VIRT_SENSOR_DEVICE] =  {  0x4000000,       0x100 },
#    [VIRT_PLIC] =        {  0xc000000, VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2) },
#    [VIRT_UART0] =       { 0x10000000,         0x100 },
#    [VIRT_VIRTIO] =      { 0x10001000,        0x1000 },
#
# static void virt_machine_init(MachineState *machine)
# {
#    /* SiFive Test MMIO device */
#    sifive_test_create(memmap[VIRT_TEST].base);
#
#    /* Sensor device */
#    sensor_device_create(memmap[VIRT_SENSOR_DEVICE].base);
#
#    /* VirtIO MMIO devices */
#    for (i = 0; i < VIRTIO_COUNT; i++) {
#        sysbus_create_simple("virtio-mmio",
# 8 changes: 8 additions & 0 deletions

# include/hw/misc/sensor_device.h
# Create the sensor_device header file:
# Original file line number Diff line number Diff line change
# @@ -0,0 +1,8 @@
# #ifndef HW_SENSOR_DEVICE_H
# #define HW_SENSOR_DEVICE_H
#
# #include "qom/object.h"
#
# DeviceState *sensor_device_create(hwaddr);
#
# #endif

# include/hw/riscv/virt.h
# Update the virtual machine header to include sensor_device:
# Original file line number Diff line number Diff line change
# @@ -59,7 +59,8 @@ enum {
#    VIRT_DRAM,
#    VIRT_PCIE_MMIO,
#    VIRT_PCIE_PIO,
#    VIRT_PCIE_ECAM
#    VIRT_PCIE_ECAM,
#    VIRT_SENSOR_DEVICE
# };

# enum {

# Testing the virtual device in QEMU

# Run the following command to start the virtual device in QEMU:
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

# Running the client application
./client

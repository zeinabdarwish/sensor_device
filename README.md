# QEMU-RISCV

## Prerequisites for Installing the QEMU Emulator

To install the required dependencies for QEMU, run the following command:

sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build device-tree-compiler

### QEMU v6.0.0 Setup for RISC-V 64 Architecture

1. **Clone the QEMU repository and checkout v6.0.0**:

git clone https://github.com/qemu/qemu.git
cd qemu
git checkout v6.0.0

2. **Configure and build QEMU for RISC-V 64 architecture**:

./configure --target-list=riscv64-softmmu
make

### Buildroot Repository Setup

1. **Clone the Buildroot repository**:

git clone https://github.com/buildroot/buildroot.git
cd buildroot/

2. **Configure Buildroot for RISC-V QEMU target**:

make qemu_riscv64_virt_defconfig
make

---

### Adding a Peripheral Device to QEMU

1. **Create a folder for the SENSOR_DEVICE in the `hw` directory**.

2. In **hw/misc/meson.build**, add the following line:

softmmu_ss.add(when: 'CONFIG_SENSOR_DEVICE', if_true: files('sensor_device.c'))

3. In **hw/misc/Kconfig**, add the following lines to define the new configuration for the sensor device:

config SENSOR_DEVICE
    bool

4. In **hw/riscv/Kconfig**, include the following to select the sensor device configuration:

config RISCV_VIRT
   select SIFIVE_TEST
   select VIRTIO_MMIO
   select FW_CFG_DMA
   /* Adding the sensor device */
   select SENSOR_DEVICE
};

5. In **hw/riscv/virt.c**:
   
   - Include the sensor device header:

   #include "hw/misc/sensor_device.h"

   - Define the address range for the sensor device:

   static const MemMapEntry virt_memmap[] = {
       [VIRT_RTC] =         {   0x101000,        0x1000 },
       [VIRT_CLINT] =       {  0x2000000,       0x10000 },
       [VIRT_PCIE_PIO] =    {  0x3000000,       0x10000 },
       [VIRT_SENSOR_DEVICE] =  {  0x4000000,       0x100 },
       [VIRT_PLIC] =        {  0xc000000, VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2) },
       [VIRT_UART0] =       { 0x10000000,         0x100 },
       [VIRT_VIRTIO] =      { 0x10001000,        0x1000 },
   };

   The sensor device is mapped to the address range `0x40000000`, with 2KB reserved for it.

   - Call the sensor device initialization function within `virt_machine_init`:

   /* Sensor device */
   sensor_device_create(memmap[VIRT_SENSOR_DEVICE].base);

6. In **include/hw/riscv/virt.h**, add the following entry for the sensor device:

enum {
   VIRT_DRAM,
   VIRT_PCIE_MMIO,
   VIRT_PCIE_PIO,
   VIRT_PCIE_ECAM,
   VIRT_SENSOR_DEVICE
};

# Testing the Virtual Device in QEMU

To start the virtual device in QEMU, run the following command:

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

# Running the Client Application

To run the client application that interacts with the virtual device, execute:

./client

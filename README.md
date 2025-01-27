# QEMU-RISCV

## Prerequisites for Installing the QEMU Emulator

To install the required dependencies for QEMU, run the following command: 


```
sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build device-tree-compiler```  
```

### QEMU v6.0.0 Setup for RISC-V 64 Architecture


```bash
git clone https://github.com/qemu/qemu.git
cd qemu
git checkout v6.0.0
./configure --target-list=riscv64-softmmu
make
```

2. Buildroot Repository Setup
```bash 
git clone https://github.com/buildroot/buildroot.git
cd buildroot/
make qemu_riscv64_virt_defconfig
make
```


### Adding a Peripheral Device to QEMU

*Create a sensor_device.c in the `hw` directory

В hw/misc/meson.build add the following line:
 @@ -1,4 +1,5 @@

```softmmu_ss.add(when: 'CONFIG_SENSOR_DEVICE', if_true: files('sensor_device.c'))```

И  hw/misc/Kconfig:
@@ -11,6 +11,9 @@ config ARMSSE_MHU

```
config SENSOR_DEVICE
    bool
```


В hw/riscv/Kconfig:

```c
@@ -34,6 +34,7 @@ config RISCV_VIRT
   select SIFIVE_TEST
   select VIRTIO_MMIO
   select FW_CFG_DMA
    

   select SENSOR_DEVICE
};
```

В include/hw/misc/sensor_device.h
 Create the sensor_device header file:

```c
 @@ -0,0 +1,8 @@
 #ifndef HW_SENSOR_DEVICE_H
 #define HW_SENSOR_DEVICE_H

 #include "qom/object.h"

 DeviceState *sensor_device_create(hwaddr);

#endif

```

2.  Define the address range for the sensor device:'

В  hw/riscv/virt.c:

```c
@@ -51,6 +52,7 @@ static const MemMapEntry virt_memmap[] = {
#    [VIRT_RTC] =         {   0x101000,        0x1000 },
#    [VIRT_CLINT] =       {  0x2000000,       0x10000 },
#    [VIRT_PCIE_PIO] =    {  0x3000000,       0x10000 },
#    [VIRT_SENSOR_DEVICE] =  {  0x4000000,       0x100 },
#    [VIRT_PLIC] =        {  0xc000000, VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2) },
#    [VIRT_UART0] =       { 0x10000000,         0x100 },
#    [VIRT_VIRTIO] =      { 0x10001000,        0x1000 },

```

 The sensor device is mapped to the address range `0x40000000`, with 2KB reserved for it.


3.  Call the sensor device initialization function within ```virt_machine_init```:

```c

#    /* Sensor device */
#    sensor_device_create(memmap[VIRT_SENSOR_DEVICE].base);
```


В include/hw/riscv/virt.h:

```c
@@ -59,7 +59,8 @@ enum {
   VIRT_DRAM,
   VIRT_PCIE_MMIO,
   VIRT_PCIE_PIO,
   VIRT_PCIE_ECAM
   VIRT_PCIE_ECAM,
   VIRT_SENSOR_DEVICE
 };
```

# Testing the virtual device in QEMU

# Run the following command to start the virtual device in QEMU:
```c
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

```
# Running the client application
```c
./client
```

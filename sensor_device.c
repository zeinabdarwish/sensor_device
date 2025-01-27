#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/sysbus.h"
#include "hw/misc/sensor_device.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define TYPE_SENSOR_DEVICE "sensor_device"

typedef struct SensorDeviceState SensorDeviceState;

DECLARE_INSTANCE_CHECKER(SensorDeviceState, SENSOR_DEVICE, TYPE_SENSOR_DEVICE)

#define REG_TEMP 0x0
#define SERVER_PORT 8080  // The port the server will listen on

// Forward declaration of start_server function
void start_server(SensorDeviceState *s);

struct SensorDeviceState {
    SysBusDevice parent_obj;
    MemoryRegion iomem;
    int temperature;  // Temperature variable
};

// Function to read data from the device
static uint64_t sensor_device_read(void *opaque, hwaddr addr, unsigned int size) {
    SensorDeviceState *s = opaque;

    // Print the address being read
    printf("Reading from address: 0x%lx, size: %u\n", addr, size);

    switch (addr) {
    case REG_TEMP:
        // Return the temperature value
        return s->temperature;
    default:
        // Return a default value if the address is unknown
        printf("Unknown address: 0x%lx, returning 0xDEADBEEF\n", addr);
        return 0xDEADBEEF;
    }
}

// Memory operations (read/write)
static const MemoryRegionOps sensor_device_ops = {
    .read = sensor_device_read,
    .endianness = DEVICE_NATIVE_ENDIAN,  // Set byte order
};

// Function to start the server using sockets
void start_server(SensorDeviceState *s) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char message[100];

    // Create a new socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address
    address.sin_port = htons(SERVER_PORT); // Use the specified port

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connections on port %d...\n", SERVER_PORT);

    // Accept a connection from a client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Send the temperature to the client
    snprintf(message, sizeof(message), "Temperature: %d°C\n", s->temperature);
    send(new_socket, message, strlen(message), 0);
    printf("Message sent to client\n");

    // Close the connection after sending the data
    close(new_socket);
    close(server_fd);
}

// Device initialization function
static void sensor_device_instance_init(Object *obj) {
    SensorDeviceState *s = SENSOR_DEVICE(obj);

    // Initialize memory for the device
    memory_region_init_io(&s->iomem, obj, &sensor_device_ops, s, TYPE_SENSOR_DEVICE, 0x100);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

    // Set the default temperature
    s->temperature = 25;  // Default temperature is 25°C

    // Start the server using the socket
    start_server(s);
}

// Device information (name, size, functions)
static const TypeInfo sensor_device_info = {
    .name = TYPE_SENSOR_DEVICE,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(SensorDeviceState),
    .instance_init = sensor_device_instance_init,
};

// Register the device with QEMU
static void sensor_device_register_types(void) {
    type_register_static(&sensor_device_info);
}

type_init(sensor_device_register_types)

// Function to create the device
DeviceState *sensor_device_create(hwaddr addr) {
    DeviceState *dev = qdev_new(TYPE_SENSOR_DEVICE);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, addr);  // Set the address
    return dev;
}

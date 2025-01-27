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

#define SERVER_PORT 8080  // المنفذ الذي سيستمع عليه الخادم



// إضافة تعريف أولي لدالة start_server

void start_server(SensorDeviceState *s);



struct SensorDeviceState {

    SysBusDevice parent_obj;

    MemoryRegion iomem;

    int temperature;  // متغير درجة الحرارة

};



// دالة لقراءة البيانات من الجهاز

static uint64_t sensor_device_read(void *opaque, hwaddr addr, unsigned int size) {

    SensorDeviceState *s = opaque;



    // طباعة العنوان المطلوب قراءته

    printf("Reading from address: 0x%lx, size: %u\n", addr, size);



    switch (addr) {

    case REG_TEMP:

        // إرجاع درجة الحرارة

        return s->temperature;

    default:

        // إرجاع قيمة افتراضية في حالة العنوان غير معروف

        printf("Unknown address: 0x%lx, returning 0xDEADBEEF\n", addr);

        return 0xDEADBEEF;

    }

}



// العمليات المتعلقة بالذاكرة (قراءة / كتابة)

static const MemoryRegionOps sensor_device_ops = {

    .read = sensor_device_read,

    .endianness = DEVICE_NATIVE_ENDIAN,  // تحديد ترتيب البايتات

};



// دالة لبدء الخادم عبر المقبس

void start_server(SensorDeviceState *s) {

    int server_fd, new_socket;

    struct sockaddr_in address;

    int addrlen = sizeof(address);

    char message[100];



    // إنشاء مقبس جديد

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {

        perror("Socket creation failed");

        exit(EXIT_FAILURE);

    }



    address.sin_family = AF_INET;

    address.sin_addr.s_addr = INADDR_ANY; // استقبال الاتصالات من أي عنوان

    address.sin_port = htons(SERVER_PORT); // استخدام المنفذ المحدد



    // ربط المقبس بالعناوين

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {

        perror("Bind failed");

        exit(EXIT_FAILURE);

    }



    // الاستماع للاتصالات الواردة

    if (listen(server_fd, 3) < 0) {

        perror("Listen failed");

        exit(EXIT_FAILURE);

    }



    printf("Waiting for connections on port %d...\n", SERVER_PORT);



    // قبول الاتصال من العميل

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {

        perror("Accept failed");

        exit(EXIT_FAILURE);

    }



    // إرسال درجة الحرارة إلى العميل

    snprintf(message, sizeof(message), "Temperature: %d°C\n", s->temperature);

    send(new_socket, message, strlen(message), 0);

    printf("Message sent to client\n");



    // إغلاق الاتصال بعد إرسال البيانات

    close(new_socket);

    close(server_fd);

}



// تهيئة الجهاز

static void sensor_device_instance_init(Object *obj) {

    SensorDeviceState *s = SENSOR_DEVICE(obj);



    // تخصيص ذاكرة للجهاز

    memory_region_init_io(&s->iomem, obj, &sensor_device_ops, s, TYPE_SENSOR_DEVICE, 0x100);

    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);



    // تعيين درجة الحرارة الافتراضية

    s->temperature = 25;  // درجة الحرارة الافتراضية 25 درجة مئوية



    // بدء الخادم عبر المقبس

    start_server(s);

}



// معلومات الجهاز (الاسم، الحجم، الوظائف)

static const TypeInfo sensor_device_info = {

    .name = TYPE_SENSOR_DEVICE,

    .parent = TYPE_SYS_BUS_DEVICE,

    .instance_size = sizeof(SensorDeviceState),

    .instance_init = sensor_device_instance_init,

};



// تسجيل الجهاز في QEMU

static void sensor_device_register_types(void) {

    type_register_static(&sensor_device_info);

}



type_init(sensor_device_register_types)



// دالة لإنشاء الجهاز

DeviceState *sensor_device_create(hwaddr addr) {

    DeviceState *dev = qdev_new(TYPE_SENSOR_DEVICE);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, addr);  // تعيين العنوان

    return dev;

}


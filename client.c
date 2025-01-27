

#include <stdio.h>



#include <stdlib.h>



#include <string.h>



#include <unistd.h>



#include <sys/types.h>



#include <sys/socket.h>



#include <netinet/in.h>



#include <arpa/inet.h>  // إضافة المكتبة







#define SERVER_IP "127.0.0.1"  // عنوان الخادم (نفس الجهاز في هذه الحالة)



#define SERVER_PORT 8080       // المنفذ الذي يستمع عليه الخادم







int main() {



    int sockfd;



    struct sockaddr_in server_addr;



    char buffer[256];



    



    // إنشاء مقبس (Socket)



    sockfd = socket(AF_INET, SOCK_STREAM, 0);



    if (sockfd < 0) {



        perror("Error opening socket");



        exit(1);



    }







    // إعداد عنوان الخادم



    server_addr.sin_family = AF_INET;



    server_addr.sin_port = htons(SERVER_PORT);



    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);







    // الاتصال بالخادم



    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {



        perror("Connection failed");



        exit(1);



    }







    // إرسال طلب الخادم لقراءة درجة الحرارة



    send(sockfd, "GET_TEMP", 8, 0);







    // استقبال البيانات (درجة الحرارة)



    memset(buffer, 0, 256);  // مسح المصفوفة



    int n = recv(sockfd, buffer, 255, 0);



    if (n < 0) {



        perror("Error reading from socket");



    }







    // طباعة درجة الحرارة التي تم استلامها



    printf("Temperature from server: %s\n", buffer);







    // إغلاق الاتصال



    close(sockfd);



    return 0;



}


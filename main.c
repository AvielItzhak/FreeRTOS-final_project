#include "init.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Shared_Configuration.h"
#include "Server/Server_Task.h"
#include "Client/Dispatcher_Task.h"


int main(void) {
    init_main(); // האיתחול שלך

  
    // 2. יצירת משימות השרת
    xTaskCreate(vServerTask, "Server_Gen", 2048, NULL, 2, NULL);

    // 3. יצירת משימות הלקוח
    xTaskCreate(vDispatcherTask, "Dispatcher", 2048, NULL, 3, NULL);
    

    vTaskStartScheduler();
    return 0;
}
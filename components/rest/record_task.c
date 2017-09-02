#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "baidu_rest.h"
#include "http.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "hal_i2s.h"
#include "wm8978.h"
#include "http_parser.h"
#include "url_parser.h"
#include "spiram_fifo.h"
#include "baidu_rest.h"

#define GPIO_OUTPUT_IO_0    16
#define GPIO_OUTPUT_PIN_SEL  ((1<<GPIO_OUTPUT_IO_0))


#define TAG "RECORD_TASK"
int32_t record_cnt;
void record_task(void *pvParameters){

	//codec init
	hal_i2c_init(0,5,17);
    hal_i2s_init(0,8000,16,1); //8k 16bit 1 channel
    WM8978_Init();
    WM8978_ADDA_Cfg(1,1); 
    WM8978_Input_Cfg(1,0,0);     
    WM8978_Output_Cfg(1,0); 
    WM8978_MIC_Gain(35);
    WM8978_AUX_Gain(0);
    WM8978_LINEIN_Gain(0);
    WM8978_SPKvol_Set(0);
    WM8978_HPvol_Set(20,20);
    WM8978_EQ_3D_Dir(1);
    WM8978_EQ1_Set(0,24);
    WM8978_EQ2_Set(0,24);
    WM8978_EQ3_Set(0,24);
    WM8978_EQ4_Set(0,24);
    WM8978_EQ5_Set(0,0);
    //gpio init 
	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    //fifo init 
    spiRamFifoInit();
    //call a buf
    char* sample_data=malloc(1024);
    int16_t* value=(int16_t*)sample_data;
    uint8_t vad_cnt=0;
    for(;;){
    	hal_i2s_read(0,sample_data,1024,portMAX_DELAY);
        if(vad_check(value,512)==1)
        	vad_cnt++;
        else
        	vad_cnt=0;
        if(vad_cnt==5){
        	//start_record_audio();
        	break;
        }
    }
    gpio_set_level(GPIO_OUTPUT_IO_0,1);
    ESP_LOGI(TAG, "start record");
    record_cnt=1;
    //call http api task
    xTaskCreate(baidu_rest_task, "baidu_rest_task", 8192, NULL, 5, NULL);
    while(1){
    	hal_i2s_read(0,sample_data,1024,portMAX_DELAY);
    	spiRamFifoWrite(sample_data,1024);
    	if(vad_check(value,512)==0)
			vad_cnt++;
		else
			vad_cnt=0;
		if(vad_cnt>5){
			record_cnt=0;
			break;
		}
	}
	gpio_set_level(GPIO_OUTPUT_IO_0,0);
	free(sample_data);
	vTaskDelete(NULL);	
}
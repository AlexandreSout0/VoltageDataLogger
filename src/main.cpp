
#include <arduino.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "driver/adc.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Instanciação do objeto da classe AsyncWebServer
AsyncWebServer server(80);

// Constantes das credenciais do WiFi
const char* ssid = "master";
const char* password = "sorvetedequeijo";

int bufferSize = 0; // Tamanho do buffer para armazenar os últimos valores
float valores[1000];
int alx = 0; // Índice atual no buffer

#define R1 150000
#define R2 750000

int count = 0;

void Task1(void * params){
    while(1){
      count++;
      int value = adc1_get_raw(ADC1_CHANNEL_4);
        float sum = 0;
        for (int i = 0; i < 10; i++) {
           int value = adc1_get_raw(ADC1_CHANNEL_4);
            float vout = value*(3.6/4095);
            sum = vout;
        } 
        char str[8];
        // char min[8];
        //sprintf(str, "%.2f", sum);
        // sprintf(min, "%d", count);
        int valuee = adc1_get_raw(ADC1_CHANNEL_4);
            float voutt = (valuee / 4095.0) * 3.06 * (150000 + 750000) / 750000; //valuee*(3.06/4095);
        //Serial.println(str);
        valores[bufferSize++] = voutt;


        //300000
        vTaskDelay (30*60000 / portTICK_PERIOD_MS);

    }
}








void setup() {

  // Conecta-se ao Ponto de acesso com as credenciais fornecidas
  WiFi.begin(ssid, password);
  Serial.begin(9600);
 server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body>";
    html += "<h1>Data Logger RAMWAY</h1>";

    for (int i = 0; i < bufferSize; i++) {
      //int idx = (alx + i) % bufferSize;
      html += "<br>" + String(valores[i]);
    }
    html += "<script>setTimeout(function(){ location.reload(); }, 10000);</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });
  // Servidor começa à ouvir os clientes
  server.begin();
  
  xTaskCreate(&Task1,"Leitura de tensao",2048, NULL,5,NULL);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_0); ////porta analógica utilizada é a ADC1_CHANNEL_4, que está conectada ao pino 32 do ESP32

}

void loop() {
  // // Atualizar a variável a cada 10 segundos
  // int novoValor = random(100); // Substitua por sua lógica para obter o novo valor
  // valores[alx] = novoValor;
  // alx = (alx + 1) % bufferSize;
  delay(10000);
}

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

#define R1 150000
#define R2 750000

const int numSamples = 10;  // Número de amostras para a média móvel
int samples[numSamples];
int indice = 0;

int FilterMovingAverage(int inputValue) {
  samples[indice] = inputValue;// Adicionar a nova amostra à matriz
  indice = (indice + 1) % numSamples;// Atualizar o índice para a próxima amostra
  int sum = 0; // Calcular a média das amostras
  for (int i = 0; i < numSamples; i++) {
    sum += samples[i];
  }

  return sum / numSamples;
}



void Task1(void * params){
    while(1){
      int value = 0;
      int filteredValue = 0;
      
      for (int i = 0; i < 20; i++) {
        value = adc1_get_raw(ADC1_CHANNEL_7);
        filteredValue = FilterMovingAverage(value);
      } 
      
      float vout = (filteredValue / 4095.0) * 3.27 * (150000 + 750000) / 750000;

      // Serial.print("Leitura: ");
      // Serial.print(value);
      // Serial.print(" | ");
      // Serial.print(filteredValue);
      // Serial.print(" -> ");
      // Serial.println(vout);

      valores[bufferSize++] = vout;



        //30*60000
        vTaskDelay ( 2000/ portTICK_PERIOD_MS);

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
      html += "<br>" + String(valores[i]);
    }
    html += "<script>setTimeout(function(){ location.reload(); }, 2000);</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });
  server.begin();

  
   adc1_config_width(ADC_WIDTH_BIT_12);
   adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    
    gpio_config_t io_conf = {0};
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_35);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

  vTaskDelay( 2000/ portTICK_PERIOD_MS);
  Serial.println("");
  Serial.println(WiFi.localIP());
  xTaskCreate(&Task1,"Leitura de tensao",2048, NULL,5,NULL);
  

}

void loop() {
  // // Atualizar a variável a cada 10 segundos
  // int novoValor = random(100); // Substitua por sua lógica para obter o novo valor
  // valores[alx] = novoValor;
  // alx = (alx + 1) % bufferSize;
  delay(10);
}
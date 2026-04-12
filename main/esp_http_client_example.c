/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_http_client.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "HTTP_CLIENT";

extern void set_mylabel(char *text);

typedef struct{
    char *url;
    char *json_str;
    char *response_str;
    esp_err_t err;
}http_data;

static EventGroupHandle_t http_event_group;

#define HTTP_GET_BIT BIT0
#define HTTP_POST_BIT      BIT1

void init_http_event_group()
{
    http_event_group = xEventGroupCreate();
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // Clean the buffer in case of a new request
            if (output_len == 0 && evt->user_data) {
                // we are just starting to copy the output data into the use
                memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
            }
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                int copy_len = 0;
                if (evt->user_data) {
                    // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } 
                output_len += copy_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
         
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
           
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
    }
    return ESP_OK;
}



static void http_get_task(void *pvParameters)
{
    http_data *hdata = (http_data *)pvParameters;
   
    //  "http://httpbin.org/headers"
    esp_http_client_config_t config = {
        .url = hdata->url,
        .event_handler = _http_event_handler,
        .user_data = hdata->response_str,        // Pass address of local buffer to get response
        .disable_auto_redirect = true,
    };
    ESP_LOGI(TAG, "HTTP request with url =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    hdata->err = esp_http_client_perform(client);
   

    xEventGroupSetBits(http_event_group, HTTP_GET_BIT);

    esp_http_client_cleanup(client);

    vTaskDelete(NULL);
}


static void http_post_task(void *pvParameters)
{
    http_data *hdata = (http_data *)pvParameters;
   
    //  "http://httpbin.org/post"
    esp_http_client_config_t config = {
        .url = hdata->url,
        .event_handler = _http_event_handler,
        .user_data = hdata->response_str,        // Pass address of local buffer to get response
        .disable_auto_redirect = true,
    };
    ESP_LOGI(TAG, "HTTP request with url =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // POST
    const char *post_data = hdata->json_str;
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    hdata->err = esp_http_client_perform(client);

    xEventGroupSetBits(http_event_group, HTTP_POST_BIT);

    esp_http_client_cleanup(client);

    vTaskDelete(NULL);
}

esp_err_t http_get(char *url,char *response_str)
{
    http_data hdata = {0};
    hdata.url = url;
    hdata.response_str = response_str;
    xTaskCreate(&http_get_task, "http_get_task", 8192, &hdata, 5, NULL);
    EventBits_t bits = xEventGroupWaitBits(http_event_group,
            HTTP_GET_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);
    return hdata.err;

}

esp_err_t http_post(char *url,char *json_str,char *response_str)
{
    http_data hdata = {0};
    hdata.url = url;
    hdata.response_str = response_str;
    hdata.json_str = json_str;
    xTaskCreate(&http_post_task, "http_post_task", 8192, &hdata, 5, NULL);
    EventBits_t bits = xEventGroupWaitBits(http_event_group,
            HTTP_POST_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);
    return hdata.err;
}
#include "include/uconfy.h"

char response_payload[1024];
const char *OK_RESPONSE = "HTTP/1.1 200 OK";

void uconfy_fetch_configs(void (*configs_fetched_callback)()) {
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG_UCONFIG, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
    }

    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG_UCONFIG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG_UCONFIG, "... Failed to allocate socket.");
        freeaddrinfo(res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG_UCONFIG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }

    ESP_LOGI(TAG_UCONFIG, "... connected");
    freeaddrinfo(res);

    char *request = malloc(sizeof(char) * (100 + strlen(REQUEST) + strlen(uconfig_device_id) + strlen(uconfig_api_key)));

    sprintf(request, "GET /api/device/%s/config HTTP/1.0\r\n%sAuthorization: apikey %s\r\nContent-Type: text/plain\r\n\r\n",
        uconfig_device_id,
        REQUEST,
        uconfig_api_key);

    printf("----------------------\n");
    printf(request);
    printf("\n");
    printf("----------------------\n");

    if (write(s, request, strlen(request)) < 0) {
        ESP_LOGE(TAG_UCONFIG, "... socket send failed");
        close(s);
        free(request);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... socket send success");
    free(request);

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG_UCONFIG, "... failed to set socket receiving timeout");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... set socket receiving timeout success");

    response_payload[0] = '\0';
    int content_length = 0;
    do {
        bzero(recv_buf, sizeof(recv_buf));
        r = read(s, recv_buf, sizeof(recv_buf)-1);
        for(int i = 0; i < r; i++) {

            if (content_length < 500) {
                response_payload[content_length] = recv_buf[i];
                content_length++;
            }
        }
    } while(r > 0);
    response_payload[content_length] = '\0';

    ESP_LOGI(TAG_UCONFIG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
    close(s);

    if (strncmp(response_payload, OK_RESPONSE, strlen(OK_RESPONSE)) == 0) {
        uconfig_parse_http_response();
        if (configs_fetched_callback != NULL) {
            configs_fetched_callback();
        }
        ESP_LOGI(TAG_UCONFIG, "Configs fetched from uconfy server");
    } else {
        ESP_LOGE(TAG_UCONFIG, "Configs could not be fetched from uconfy server, expected 200 OK response. Received full payload:");
        ESP_LOGE(TAG_UCONFIG, "%s", response_payload);
    }
}

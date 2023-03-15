#include "include/uconfy.h"

char request[7500];
const char *NO_CONTENT_RESPONSE = "HTTP/1.1 204 No Content";

void uconfy_flush_logs()
{
    if (!UCONFIG_IS_WIFI_CONNECTED) {
        printf("No wifi connection, ignore uconfy request\n");
        return;
    }

    if (strlen(uconfig_tmp_log) == 0) {
        ESP_LOGI(TAG_UCONFIG, "No logs to flush");
        return;
    }

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
        return;
    }

    /* Code to print the resolved IP.
    Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG_UCONFIG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG_UCONFIG, "... Failed to allocate socket.");
        freeaddrinfo(res);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG_UCONFIG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        return;
    }

    ESP_LOGI(TAG_UCONFIG, "... connected");
    freeaddrinfo(res);

    size_t out_len;
    // printf("[base64 encoding] message: '%s'\n", uconfig_tmp_log);
    char *base64_encoded = base64_encode(uconfig_tmp_log, strlen(uconfig_tmp_log), &out_len);
    if (base64_encoded == NULL) {
        ESP_LOGE(TAG_UCONFIG, "Unable to base64 encode payload, memory is full; left space from heap: %d bytes, log site: %d bytes", xPortGetFreeHeapSize(), strlen(uconfig_tmp_log));
        close(s);
        return;
    }

    /*strncat(content, base64_encoded, out_len);
    strcat(content, "\"}");
    strcat(content, "\r\n\0");*/

    sprintf(request, "POST /api/device/%s/logs HTTP/1.0\r\n%sAuthorization: apikey %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n{ \"Logs\": \"%s\" }\r\n",
        uconfig_device_id,
        REQUEST,
        uconfig_api_key,
        strlen(base64_encoded) + 14,
        base64_encoded);

    /*printf("----------------------\n");
    printf(request);
    printf("\n");
    printf("----------------------\n");*/

    if (write(s, request, strlen(request)) < 0) {
        ESP_LOGE(TAG_UCONFIG, "... socket send failed");
        close(s);
        free(base64_encoded);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... socket send success");
    uconfig_tmp_log[0] = '\0';
    free(base64_encoded);

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG_UCONFIG, "... failed to set socket receiving timeout");
        close(s);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... set socket receiving timeout success");

    /* Read HTTP response */
    // response_payload[0] = '\0';
    char response_header[500];
    int content_length = 0;
    do {
        bzero(recv_buf, sizeof(recv_buf));
        r = read(s, recv_buf, sizeof(recv_buf)-1);
        for(int i = 0; i < r; i++) {

            if (content_length < 500) {
                response_header[content_length] = recv_buf[i];
                content_length++;
            }
        }
    } while(r > 0);
    response_header[content_length] = '\0';

    ESP_LOGI(TAG_UCONFIG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
    close(s);

    if (strncmp(response_header, NO_CONTENT_RESPONSE, strlen(NO_CONTENT_RESPONSE)) == 0) {
        ESP_LOGI(TAG_UCONFIG, "Logs flushed to uconfy server");
    } else {
        ESP_LOGE(TAG_UCONFIG, "Logs could not be flushed to uconfy server, response status is not '204 No Content', full response:");
        ESP_LOGE(TAG_UCONFIG, "%s", response_header);
    }
}

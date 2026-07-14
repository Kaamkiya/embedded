#include <esp_log.h>
#include <esp_http_server.h>

#ifndef PROJ_WS_H
#define PROJ_WS_H

esp_err_t ws_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);

#endif

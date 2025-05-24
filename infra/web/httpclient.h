#pragma once

#include <hv/HttpClient.h>

namespace infra::web {

using HttpMethod = http_method;
using HttpStatus = http_status;

class HttpReq : public HttpRequest {
public:
    void set_method(HttpMethod input_method);
    void set_url(const std::string &i_url);
};

class HttpResp : public HttpResponse {
public:
    HttpStatus status() const;
    const char *status_msg();
    std::string dump();
};

int http_client_write(HttpReq *req, HttpResp *res);

// class HttpClient : protected hv::HttpClient {
// public:
//     HttpClient(const char* host = NULL, int port = DEFAULT_HTTP_PORT, int https = 0);


//     const char* get_header(const char* key);

//     // http_proxy
//     int set_http_proxy(const char* host, int port);
//     // https_proxy
//     int set_https_proxy(const char* host, int port);
//     // no_proxy
//     int addNoProxy(const char* host);

//     // sync
//     int send(HttpRequest* req, HttpResponse* resp);

//     // async
//     int sendAsync(HttpRequestPtr req, HttpResponseCallback resp_cb = NULL);

//     // low-level api
//     int connect(const char* host, int port = DEFAULT_HTTP_PORT, int https = 0, int timeout = DEFAULT_HTTP_CONNECT_TIMEOUT);
//     int sendHeader(HttpRequest* req);
//     int sendData(const char* data, int size);
//     int recvData(char* data, int size);
//     int recvResponse(HttpResponse* resp);
//     int close();
// };

}
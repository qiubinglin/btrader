#include "httpclient.h"

namespace infra::web {

void HttpReq::set_method(HttpMethod input_method) { method = input_method; }

void HttpReq::set_url(const std::string &i_url) { url = i_url; }

HttpStatus HttpResp::status() const { return status_code; }

const char *HttpResp::status_msg() { return HttpResponse::status_message(); }

std::string HttpResp::dump() { return HttpResponse::Dump(false, true); }

int http_client_write(HttpReq *req, HttpResp *res) { return http_client_send(req, res); }

} // namespace infra::web
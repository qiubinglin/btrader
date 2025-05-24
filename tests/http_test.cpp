#include <iostream>

#include "infra/web/httpclient.h"

int main() {
    std::string url = "https://api.binance.com/api/v3/time";
    infra::web::HttpReq req;
    req.set_method(infra::web::HttpMethod::HTTP_GET);
    req.set_url(url);
    infra::web::HttpResp res;
    infra::web::http_client_write(&req, &res);
    std::cout << res.status() << " " << res.dump() << std::endl;
    return 0;
}
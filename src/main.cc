#include<drogon/drogon.h>

int main(){
    /**
     * @brief 注册预路由拦截器处理 CORS(Cross-Origin Resource Sharing) 跨域请求的 OPTIONS 预检
     *
     * 该 Lambda 函数作为预路由建议被调用，用于拦截所有进入的 HTTP 请求。
     * 对于 OPTIONS 方法的预检请求，直接构造响应并终止处理流程；
     * 对于其他请求，则放行到后续的处理环节。
     *
     * @param req HTTP 请求指针，包含请求方法、头信息等完整信息
     * @param stop 停止回调函数，接收一个 HttpResponsePtr 参数，用于终止请求处理并返回指定响应
     * @param pass 通过回调函数，无参数，用于继续将请求传递到下一个处理阶段
     */
    // Register pre-routing advice for CORS
    drogon::app().registerPreRoutingAdvice([](
        const drogon::HttpRequestPtr &req, 
        std::function<void(const drogon::HttpResponsePtr &)> &&stop,
        std::function<void()> &&pass) {
        /**
         * @brief 处理 CORS 预检 OPTIONS 请求
         *
         * 当检测到请求方法为 OPTIONS 时，构造 CORS 允许响应头并立即返回，
         * 阻止请求继续向下处理。这是浏览器发起跨域请求前的标准预检流程。
         */
        // Handle preflight OPTIONS requests
        if (req->method() == drogon::Options) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->addHeader("Access-Control-Allow-Origin", "*");// 允许所有源的请求
            resp->addHeader("Access-Control-Allow-Methods", "*");// 允许所有 HTTP 方法
            resp->addHeader("Access-Control-Allow-Headers", "*, Authorization");// 允许所有请求头，显式包含用于 JWT 的 Authorization
            resp->addHeader("Access-Control-Max-Age", std::to_string(86400));// 将预检结果缓存 24 小时
            stop(resp); // Send response and stop further processing
            return;
        }
        pass(); // Continue processing for non-OPTIONS requests
    });
    /**
     * @brief 注册后处理拦截器为所有响应添加 CORS 头
     *
     * 该 Lambda 函数在请求处理完成后被调用，确保所有 HTTP 响应都包含
     * Access-Control-Allow-Origin 头，允许跨域访问。
     *
     * @param req HTTP 请求指针，包含请求的完整信息
     * @param resp HTTP 响应指针，可修改响应内容和头信息
     */
    // Register post-handling advice to add CORS headers to all responses
    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req,
        const drogon::HttpResponsePtr &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");// 允许所有源的请求
    });

    /*以上：预处理路由和后置处理
     *预路由建议（pre-routing advice）拦截 OPTIONS 预检请求并返回相应的头部信息，
     *而处理后建议（post-handling advice），则将 Access-Control-Allow-Origin: 附加到每个响应
     */
    std::cout<<"ENV: JWT_SECRET="<<std::getenv("JWT_SECRET")<<std::endl;
    std::cout<<"Server is running!"<<std::endl;
    drogon::app().loadConfigFile("config.json").run();
    return 0;
}
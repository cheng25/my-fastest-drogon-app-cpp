/**
 *
 *  auth.cc
 *
 */

#include "auth.h"
#include<jsoncpp/json/json.h>
#include"../utils/AppError.h"
#include"../utils/token.h"

using namespace drogon;

void auth::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    try
    {
        //1.头部提取
        /*提取 Authorization 头部，验证 Bearer 前缀，解码并验证 JWT 令牌，并将 userId 和 email 作为请求参数注入，以供下游控制器访问 */
        auto authHeader=req->getHeader("Authorization");
        if(authHeader.empty()) throw AppError("token not found!",k401Unauthorized);
        //2.Bearer 前缀检查
        // 验证头部是否以Bearer 方案开头。这等效于 starts_with。
        if(authHeader.rfind("Bearer ",0)!=0) throw AppError("Bearer token not found!",k401Unauthorized);
        //3.令牌提取 — 去除 7 个字符的 Bearer 前缀，以分离出原始的 JWT 字符串。
        std::string token=authHeader.substr(7);
        //4.令牌验证
        auto [userId,email]=Auth::decodeAndverifyToken(token);
        if(!userId){
            throw AppError("Unauthorised",k401Unauthorized);
        }
        //5.参数注入
        req->setParameter("userId",std::to_string(userId));
        req->setParameter("email",email);
        fccb(); // 沿过滤器链向下传递
    }
    catch(const AppError& e)
    {
        Json::Value res;
        res["success"]=false;
        res["message"]=e.what();
        auto resp=HttpResponse::newHttpJsonResponse(res);
        resp->setStatusCode(e.statusCode);
        fcb(resp);// 通过错误响应进行短路处理
    }
    catch(const std::exception& e)
    {
        Json::Value res;
        res["success"]=false;
        res["message"]=e.what();
        auto resp=HttpResponse::newHttpJsonResponse(res);
        resp->setStatusCode(k500InternalServerError);
        fcb(resp);// 通过错误响应进行短路处理
    }
}

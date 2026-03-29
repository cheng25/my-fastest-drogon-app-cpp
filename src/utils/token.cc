#include"jwt-cpp/jwt.h"
#include"./token.h"
#include<jsoncpp/json/json.h>


std::string Auth::createToken(const int userId,const std::string &email){
    /*使用 JWT_SECRET 环境变量以 HS256 算法签署 JWT，将签发者设置为 "suryansh"，类型设置为 "JWS"，过期时间设置为 24 小时*/
    std::string token=jwt::create()
    .set_issuer("suryansh") // 签发者
    .set_type("JWS") // 类型
    .set_expires_at(std::chrono::system_clock::now()+std::chrono::hours{24}) // 过期时间
    .set_payload_claim("userId",jwt::claim(std::to_string(userId))) //载荷声明
    .set_payload_claim("email",jwt::claim(email))
    .sign(jwt::algorithm::hs256{std::getenv("JWT_SECRET")});// 签名

    return token;
}

data Auth::decodeAndverifyToken(const std::string &token){
    try
    {
        /*会解码 JWT，针对 JWT_SECRET（环境变量）验证签名，检查签发者（suryansh）和类型（JWS），并提取 userId 和 email 声明。*/
        auto decodedToken=jwt::decode(token);
        auto verifier = jwt::verify()
        .with_issuer("suryansh") // 确保令牌是由本应用创建的
        .with_type("JWS") // 拒绝非预期的令牌格式
        .allow_algorithm(jwt::algorithm::hs256{std::getenv("JWT_SECRET")});// 加密签名验证

        verifier.verify(decodedToken);
        //从解码令牌的 JSON 载荷中提取声明
        auto mp = decodedToken.get_payload_json();
        const std::string email=mp["email"].get<std::string>();
        const std::string userId=mp["userId"].get<std::string>();
        return {std::stoi(userId),email};
    }
    catch(const std::exception& e)
    {
        return {0,""};
    }
    
}
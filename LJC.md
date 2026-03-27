
# my_fastest_drogon_app_cpp
1. 修改docker-compose.yml
    指定卷目录
    ```yml
    - /drogon_dev/my-fastest-drogon-app-cpp/init.sql:/docker-entrypoint-initdb.d/init.sql:ro
    - /data/usershare/PG/pgdata:/var/lib/postgresql/data
    ```
2. 启动数据库
    ```bash
    sudo docker-compose -f /drogon_dev/my-fastest-drogon-app-cpp/docker-compose.yml up -d postgres
    ```

3. 启动开发环境
    ```bash
   sudo docker run -it \
    -p 8010:22 \
    -p 3100:3100 \
    -p 8100:8100 \
    -p 8083:8083 \
    -p 3001:3001 \
    -e JWT_SECRET=mysecretkey \
    -v /drogon_dev/:/install/drogon_dev \
    --name dx_drogon_dev \
    -v /etc/localtime:/etc/localtime:ro \
    -d dx_drogon_dev:1.0
    ```
   
4. 忘记加入子网，再加入子网
    ```bash
   sudo docker network create my-net
   sudo docker network connect my-net dx_drogon_dev
   sudo docker network connect my-net postgres_prod
   sudo docker network inspect my-net
    ```

5. 进入开发环境
    ```bash
   sudo docker start dx_drogon_dev && sudo docker exec -it dx_drogon_dev bash
    ```
   
6. 使用CLion调试后端时，需要添加环境变量`JWT_SECRET=mysecretkey`
   针对my_drogon_app->编辑配置->环境变量->添加JWT_SECRET=mysecretkey->确定
  
7. 编译启动应用报错，提示数据库连接失败 ，修改config.json
   指定数据库连接信息`postgres_prod`
    ```json
    {
        "db_clients": [
            {
                "rdbms": "postgresql",
                "host": "postgres_prod",
                "port": 5432,
                "dbname": "userdb",
                "user": "postgres",
                "password": "postgres",
                "charset": "utf8mb4"
            }
        ]
    }
    ```
   - 再次编译启动应用，成功连接数据库
   - 健康检测
    ```bash
    http://192.168.250.101:3001/api/v1/health/
    ```
   输出
    ```json
    {"message":"healthy","success":true}
    ```
 - 如果不使用CLion调试 
   - 配置并构建应用：
   ```bash
   mkdir -p build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   cmake --build . -j
   ```
   
   - 设置 JWT 密钥并运行：
   ```bash
   export JWT_SECRET="your-secret-key"
   ./my_drogon_app
   ```

8. 宿主机安装安装 Node.js 和 npm 
    ```bash
    sudo apt install -y nodejs npm
    # 验证安装
    node -v
    npm -v
    ```
9. 添加环境变量 NEXT_PUBLIC_BACKEND_URL
   前端项目根目录的 .env.local 文件中正确配置了环境变量：
   
   ```bash
   NEXT_PUBLIC_BACKEND_URL=http://192.168.250.101:3001
   ```
   > 注意：localhost / 127.0.0.1 在开发环境中都指向宿主机，使用NET网络不能访问到虚拟机中的服务
   > 改为 `http://192.168.250.101:3001`
   

10. 启动前端
    ```bash
    cd /drogon_dev/my-fastest-drogon-app-cpp/frontend
    npm install
    npm run dev
    ```
    
    - `npm install` 报错ERR! code EACCES
        ```bash
        # 更改目录所有权
        sudo chown -R $(whoami):$(whoami) /drogon_dev/my-fastest-drogon-app-cpp
        
        # 进入项目目录（不要用 sudo！）
        cd /drogon_dev/my-fastest-drogon-app-cpp/frontend

        # 删除旧依赖（可能已损坏）
        rm -rf node_modules package-lock.json
    
        # 安装 nvm
        curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash
        
        # 重新加载终端
        source ~/.bashrc  # 或 source ~/.zshrc
        
        # 安装 Node.js
        nvm install --lts
        
        # 验证安装
        node -v && npm -v
    ```
    
    - 再次执行`npm install`，成功
    - 执行`npm run dev`，成功`

11. 访问前端
    ```bash
    http://192.168.250.101:3000
    ```
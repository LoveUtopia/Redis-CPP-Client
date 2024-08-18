# Redis Setup and C++ Client Implementation Guide



## Table of Contents



1. [Redis Server Installation](https://github.com/LoveUtopia/RedisForCppPlayer#redis-server-installation)
2. [Setting Up Redis Client in C++](https://github.com/LoveUtopia/RedisForCppPlayer#setting-up-redis-client-in-c++)
3. [Compiling and Running the C++ Client](https://github.com/LoveUtopia/RedisForCppPlayer#compiling-and-running-the-c-client)
4. [Demo Usage Example](https://github.com/LoveUtopia/RedisForCppPlayer#demo-usage-example)

------

## 1. Redis Server Installation



### On Ubuntu Server



1. **Update Package List:**

   ```
   sudo apt update
   ```

   

2. **Install Redis:**

   ```
   sudo apt install redis-server
   ```

   

3. **Start Redis Service:**

   ```
   sudo systemctl start redis
   ```

   

4. **Enable Redis to Start on Boot:**

   ```
   sudo systemctl enable redis
   ```

   

5. **Verify Redis is Running:**

   ```
   sudo systemctl status redis
   ```

   

6. **Configure Redis (Optional):**

   Redis configuration file is located at `/etc/redis/redis.conf`. You can edit this file to change settings such as binding IP, port, etc.

   ```
   sudo nano /etc/redis/redis.conf
   ```

   

### On Windows



1. **Download Redis:**

   Download the Redis ZIP package from the Redis website or use a Windows port like [Memurai](https://memurai.com/) for easier installation.

2. **Extract and Install Redis:**

   Extract the downloaded ZIP package and follow the instructions provided in the README file or installer.

3. **Run Redis:**

   Open a command prompt and navigate to the Redis directory. Run Redis using:

   ```
   redis-server
   ```

   

4. **Verify Redis is Running:**

   Use the Redis CLI to check:

   ```
   redis-cli
   ```

   

------

## 2. Setting Up Redis Client in C++



### Prerequisites



- **Install `hiredis` Library:**

  On Ubuntu:

  ```
  sudo apt install libhiredis-dev
  ```

  

  On Windows:

  Follow the instructions from the [hiredis GitHub repository](https://github.com/redis/hiredis).

### RedisClient Header (`RedisClient.h`)



Create a file named `RedisClient.h` with the following content:

```
#ifndef REDISCLIENT_H
#define REDISCLIENT_H

#include <hiredis/hiredis.h>
#include <string>

class RedisClient {
public:
    static RedisClient& getInstance();
    RedisClient(const RedisClient&) = delete;
    RedisClient& operator=(const RedisClient&) = delete;

    bool set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    bool del(const std::string& key);
    bool incr(const std::string& key);

    bool hset(const std::string& key, const std::string& field, const std::string& value);
    std::string hget(const std::string& key, const std::string& field);
    bool hdel(const std::string& key, const std::string& field);
    std::string hgetall(const std::string& key);

    bool lpush(const std::string& key, const std::string& value);
    bool rpush(const std::string& key, const std::string& value);
    std::string lpop(const std::string& key);
    std::string rpop(const std::string& key);
    std::string lrange(const std::string& key, int start, int stop);

    bool sadd(const std::string& key, const std::string& member);
    bool srem(const std::string& key, const std::string& member);
    std::string smembers(const std::string& key);
    bool sismember(const std::string& key, const std::string& member);

    bool zadd(const std::string& key, double score, const std::string& member);
    std::string zrange(const std::string& key, int start, int stop);
    bool zrem(const std::string& key, const std::string& member);
    double zscore(const std::string& key, const std::string& member);

    bool multi();
    bool exec();
    bool discard();
    bool watch(const std::string& key);

    bool publish(const std::string& channel, const std::string& message);
    bool subscribe(const std::string& channel);
    bool unsubscribe(const std::string& channel);

    std::string info();
    std::string configGet(const std::string& parameter);
    bool configSet(const std::string& parameter, const std::string& value);
    bool flushdb();
    bool flushall();

    std::string keys(const std::string& pattern);
    bool expire(const std::string& key, int seconds);
    int ttl(const std::string& key);

private:
    RedisClient();
    ~RedisClient();
    redisContext* context;
    bool connect();
};

#endif // REDISCLIENT_H
```



### RedisClient Implementation (`RedisClient.cpp`)



Create a file named `RedisClient.cpp` with the following content:

```
#include "RedisClient.h"
#include <hiredis/hiredis.h>
#include <stdexcept>
#include <sstream>

RedisClient& RedisClient::getInstance() {
    static RedisClient instance;
    return instance;
}

RedisClient::RedisClient() : context(nullptr) {
    if (!connect()) {
        throw std::runtime_error("Failed to connect to Redis server.");
    }
}

RedisClient::~RedisClient() {
    if (context) {
        redisFree(context);
    }
}

bool RedisClient::connect() {
    context = redisConnect("127.0.0.1", 6379);
    return context && !context->err;
}

// Implement other methods following the pattern shown in previous messages

// Example of `set` method
bool RedisClient::set(const std::string& key, const std::string& value) {
    redisReply* reply = (redisReply*)redisCommand(context, "SET %s %s", key.c_str(), value.c_str());
    bool success = reply && (reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK");
    freeReplyObject(reply);
    return success;
}

// Implement remaining methods similarly
```



------

## 3. Compiling and Running the C++ Client



### On Ubuntu



1. **Create a Build Directory:**

   ```
   mkdir build
   cd build
   ```

   

2. **Compile the Program:**

   ```
   g++ -o redis_client main.cpp RedisClient.cpp -lhiredis
   ```

   

   Make sure `main.cpp` contains your test or demo code.

3. **Run the Program:**

   ```
   ./redis_client
   ```

   

### On Windows



1. **Open Developer Command Prompt:**

   Open a Developer Command Prompt for Visual Studio or MinGW command prompt.

2. **Compile the Program:**

   ```
   g++ -o redis_client.exe main.cpp RedisClient.cpp -lhiredis
   ```

   

3. **Run the Program:**

   ```
   redis_client.exe
   ```

   

------

## 4. Demo Usage Example



### Create `main.cpp`



Create a file named `main.cpp` with the following demo code:

```
#include <iostream>
#include "RedisClient.h"

int main() {
    RedisClient& redis = RedisClient::getInstance();

    // Set a key-value pair
    if (redis.set("mykey", "myvalue")) {
        std::cout << "Set operation successful!" << std::endl;
    } else {
        std::cout << "Set operation failed!" << std::endl;
    }

    // Get the value of a key
    std::string value = redis.get("mykey");
    std::cout << "Value for 'mykey': " << value << std::endl;

    // Delete a key
    if (redis.del("mykey")) {
        std::cout << "Delete operation successful!" << std::endl;
    } else {
        std::cout << "Delete operation failed!" << std::endl;
    }

    return 0;
}
```



This simple example sets a key-value pair in Redis, retrieves it, and then deletes it.


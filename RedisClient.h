#ifndef REDISCLIENT_H
#define REDISCLIENT_H

#include <hiredis/hiredis.h>
#include <string>

class RedisClient {
public:
    // Get the singleton instance of RedisClient
    static RedisClient& getInstance();

    // Delete copy constructor and assignment operator
    RedisClient(const RedisClient&) = delete;
    RedisClient& operator=(const RedisClient&) = delete;

    // Redis string operations
    bool set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    bool del(const std::string& key);
    bool incr(const std::string& key);

    // Redis hash operations
    bool hset(const std::string& key, const std::string& field, const std::string& value);
    std::string hget(const std::string& key, const std::string& field);
    bool hdel(const std::string& key, const std::string& field);
    std::string hgetall(const std::string& key);

    // Redis list operations
    bool lpush(const std::string& key, const std::string& value);
    bool rpush(const std::string& key, const std::string& value);
    std::string lpop(const std::string& key);
    std::string rpop(const std::string& key);
    std::string lrange(const std::string& key, int start, int stop);

    // Redis set operations
    bool sadd(const std::string& key, const std::string& member);
    bool srem(const std::string& key, const std::string& member);
    std::string smembers(const std::string& key);
    bool sismember(const std::string& key, const std::string& member);

    // Redis sorted set operations
    bool zadd(const std::string& key, double score, const std::string& member);
    std::string zrange(const std::string& key, int start, int stop);
    bool zrem(const std::string& key, const std::string& member);
    double zscore(const std::string& key, const std::string& member);

    // Redis transaction operations
    bool multi();
    bool exec();
    bool discard();
    bool watch(const std::string& key);

    // Redis publish/subscribe operations
    bool publish(const std::string& channel, const std::string& message);
    bool subscribe(const std::string& channel);
    bool unsubscribe(const std::string& channel);

    // Redis server management operations
    std::string info();
    std::string configGet(const std::string& parameter);
    bool configSet(const std::string& parameter, const std::string& value);
    bool flushdb();
    bool flushall();

    // Redis key operations
    std::string keys(const std::string& pattern);
    bool expire(const std::string& key, int seconds);
    int ttl(const std::string& key);

private:
    // Private constructor for singleton pattern
    RedisClient();
    ~RedisClient();

    // Redis context pointer
    redisContext* context;
    
    // Connect to the Redis server
    bool connect();
};

#endif // REDISCLIENT_H

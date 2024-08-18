#include "RedisClient.h"
#include <hiredis/hiredis.h>
#include <stdexcept>
#include <sstream>

// Singleton instance
RedisClient& RedisClient::getInstance() {
    static RedisClient instance;
    return instance;
}

// Private constructor
RedisClient::RedisClient() : context(nullptr) {
    if (!connect()) {
        throw std::runtime_error("Failed to connect to Redis server.");
    }
}

// Private destructor
RedisClient::~RedisClient() {
    if (context) {
        redisFree(context);
    }
}

// Connect to Redis server
bool RedisClient::connect() {
    context = redisConnect("127.0.0.1", 6379);
    return context && !context->err;
}

// Set a string value
bool RedisClient::set(const std::string& key, const std::string& value) {
    redisReply* reply = (redisReply*)redisCommand(context, "SET %s %s", key.c_str(), value.c_str());
    bool success = reply && (reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK");
    freeReplyObject(reply);
    return success;
}

// Get a string value
std::string RedisClient::get(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "GET %s", key.c_str());
    std::string value = (reply && reply->type == REDIS_REPLY_STRING) ? reply->str : "";
    freeReplyObject(reply);
    return value;
}

// Delete a key
bool RedisClient::del(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "DEL %s", key.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

// Increment a key's value
bool RedisClient::incr(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "INCR %s", key.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER;
    freeReplyObject(reply);
    return success;
}

// Set hash field
bool RedisClient::hset(const std::string& key, const std::string& field, const std::string& value) {
    redisReply* reply = (redisReply*)redisCommand(context, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    bool success = reply && (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
    freeReplyObject(reply);
    return success;
}

// Get hash field value
std::string RedisClient::hget(const std::string& key, const std::string& field) {
    redisReply* reply = (redisReply*)redisCommand(context, "HGET %s %s", key.c_str(), field.c_str());
    std::string value = (reply && reply->type == REDIS_REPLY_STRING) ? reply->str : "";
    freeReplyObject(reply);
    return value;
}

// Delete hash field
bool RedisClient::hdel(const std::string& key, const std::string& field) {
    redisReply* reply = (redisReply*)redisCommand(context, "HDEL %s %s", key.c_str(), field.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

// Get all hash fields and values
std::string RedisClient::hgetall(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "HGETALL %s", key.c_str());
    std::string result;
    if (reply && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; i += 2) {
            result += reply->element[i]->str;
            result += ": ";
            result += reply->element[i + 1]->str;
            result += "\n";
        }
    }
    freeReplyObject(reply);
    return result;
}

// Push a value to the left of the list
bool RedisClient::lpush(const std::string& key, const std::string& value) {
    redisReply* reply = (redisReply*)redisCommand(context, "LPUSH %s %s", key.c_str(), value.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER;
    freeReplyObject(reply);
    return success;
}

// Push a value to the right of the list
bool RedisClient::rpush(const std::string& key, const std::string& value) {
    redisReply* reply = (redisReply*)redisCommand(context, "RPUSH %s %s", key.c_str(), value.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER;
    freeReplyObject(reply);
    return success;
}

// Pop a value from the left of the list
std::string RedisClient::lpop(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "LPOP %s", key.c_str());
    std::string value = (reply && reply->type == REDIS_REPLY_STRING) ? reply->str : "";
    freeReplyObject(reply);
    return value;
}

// Pop a value from the right of the list
std::string RedisClient::rpop(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "RPOP %s", key.c_str());
    std::string value = (reply && reply->type == REDIS_REPLY_STRING) ? reply->str : "";
    freeReplyObject(reply);
    return value;
}

// Get a range of values from the list
std::string RedisClient::lrange(const std::string& key, int start, int stop) {
    redisReply* reply = (redisReply*)redisCommand(context, "LRANGE %s %d %d", key.c_str(), start, stop);
    std::string result;
    if (reply && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            result += reply->element[i]->str;
            result += "\n";
        }
    }
    freeReplyObject(reply);
    return result;
}

// Add a member to the set
bool RedisClient::sadd(const std::string& key, const std::string& member) {
    redisReply* reply = (redisReply*)redisCommand(context, "SADD %s %s", key.c_str(), member.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

// Remove a member from the set
bool RedisClient::srem(const std::string& key, const std::string& member) {
    redisReply* reply = (redisReply*)redisCommand(context, "SREM %s %s", key.c_str(), member.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

// Get all members of the set
std::string RedisClient::smembers(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "SMEMBERS %s", key.c_str());
    std::string result;
    if (reply && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            result += reply->element[i]->str;
            result += "\n";
        }
    }
    freeReplyObject(reply);
    return result;
}

// Check if a member is in the set
bool RedisClient::sismember(const std::string& key, const std::string& member) {
    redisReply* reply = (redisReply*)redisCommand(context, "SISMEMBER %s %s", key.c_str(), member.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

// Add a member to a sorted set with a score
bool RedisClient::zadd(const std::string& key, double score, const std::string& member) {
    redisReply* reply = (redisReply*)redisCommand(context, "ZADD %s %f %s", key.c_str(), score, member.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

// Get a range of members from the sorted set
std::string RedisClient::zrange(const std::string& key, int start, int stop) {
    redisReply* reply = (redisReply*)redisCommand(context, "ZRANGE %s %d %d", key.c_str(), start, stop);
    std::string result;
    if (reply && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            result += reply->element[i]->str;
            result += "\n";
        }
    }
    freeReplyObject(reply);
    return result;
}

// Remove a member from the sorted set
bool RedisClient::zrem(const std::string& key, const std::string& member) {
    redisReply* reply = (redisReply*)redisCommand(context, "ZREM %s %s", key.c_str(), member.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

// Get the score of a member in the sorted set
double RedisClient::zscore(const std::string& key, const std::string& member) {
    redisReply* reply = (redisReply*)redisCommand(context, "ZSCORE %s %s", key.c_str(), member.c_str());
    double score = (reply && reply->type == REDIS_REPLY_STRING) ? std::stod(reply->str) : 0.0;
    freeReplyObject(reply);
    return score;
}

// Start a transaction
bool RedisClient::multi() {
    redisReply* reply = (redisReply*)redisCommand(context, "MULTI");
    bool success = reply && reply->type == REDIS_REPLY_ARRAY;
    freeReplyObject(reply);
    return success;
}

// Execute a transaction
bool RedisClient::exec() {
    redisReply* reply = (redisReply*)redisCommand(context, "EXEC");
    bool success = reply && reply->type == REDIS_REPLY_ARRAY;
    freeReplyObject(reply);
    return success;
}

// Discard a transaction
bool RedisClient::discard() {
    redisReply* reply = (redisReply*)redisCommand(context, "DISCARD");
    bool success = reply && reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK";
    freeReplyObject(reply);
    return success;
}

// Watch a key for changes
bool RedisClient::watch(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "WATCH %s", key.c_str());
    bool success = reply && reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK";
    freeReplyObject(reply);
    return success;
}

// Publish a message to a channel
bool RedisClient::publish(const std::string& channel, const std::string& message) {
    redisReply* reply = (redisReply*)redisCommand(context, "PUBLISH %s %s", channel.c_str(), message.c_str());
    bool success = reply && reply->type == REDIS_REPLY_INTEGER;
    freeReplyObject(reply);
    return success;
}

// Subscribe to a channel
bool RedisClient::subscribe(const std::string& channel) {
    redisReply* reply = (redisReply*)redisCommand(context, "SUBSCRIBE %s", channel.c_str());
    bool success = reply && reply->type == REDIS_REPLY_ARRAY;
    freeReplyObject(reply);
    return success;
}

// Unsubscribe from a channel
bool RedisClient::unsubscribe(const std::string& channel) {
    redisReply* reply = (redisReply*)redisCommand(context, "UNSUBSCRIBE %s", channel.c_str());
    bool success = reply && reply->type == REDIS_REPLY_ARRAY;
    freeReplyObject(reply);
    return success;
}

// Get server information
std::string RedisClient::info() {
    redisReply* reply = (redisReply*)redisCommand(context, "INFO");
    std::string info = (reply && reply->type == REDIS_REPLY_STRING) ? reply->str : "";
    freeReplyObject(reply);
    return info;
}

// Get a configuration parameter value
std::string RedisClient::configGet(const std::string& parameter) {
    redisReply* reply = (redisReply*)redisCommand(context, "CONFIG GET %s", parameter.c_str());
    std::string result;
    if (reply && reply->type == REDIS_REPLY_ARRAY && reply->elements > 1) {
        result = reply->element[1]->str;
    }
    freeReplyObject(reply);
    return result;
}

// Set a configuration parameter value
bool RedisClient::configSet(const std::string& parameter, const std::string& value) {
    redisReply* reply = (redisReply*)redisCommand(context, "CONFIG SET %s %s", parameter.c_str(), value.c_str());
    bool success = reply && reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK";
    freeReplyObject(reply);
    return success;
}

// Flush all keys from the current database
bool RedisClient::flushdb() {
    redisReply* reply = (redisReply*)redisCommand(context, "FLUSHDB");
    bool success = reply && reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK";
    freeReplyObject(reply);
    return success;
}

// Flush all keys from all databases
bool RedisClient::flushall() {
    redisReply* reply = (redisReply*)redisCommand(context, "FLUSHALL");
    bool success = reply && reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK";
    freeReplyObject(reply);
    return success;
}

// Get all keys matching a pattern
std::string RedisClient::keys(const std::string& pattern) {
    redisReply* reply = (redisReply*)redisCommand(context, "KEYS %s", pattern.c_str());
    std::string result;
    if (reply && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            result += reply->element[i]->str;
            result += "\n";
        }
    }
    freeReplyObject(reply);
    return result;
}

// Set the expiration time for a key
bool RedisClient::expire(const std::string& key, int seconds) {
    redisReply* reply = (redisReply*)redisCommand(context, "EXPIRE %s %d", key.c_str(), seconds);
    bool success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1;
    freeReplyObject(reply);
    return success;
}

// Get the remaining time to live for a key
int RedisClient::ttl(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context, "TTL %s", key.c_str());
    int ttl = (reply && reply->type == REDIS_REPLY_INTEGER) ? reply->integer : -1;
    freeReplyObject(reply);
    return ttl;
}

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <sstream>

namespace edge_infra {

// 简单的JSON解析器和构建器
class JsonValue {
public:
    enum Type {
        NULL_TYPE,
        BOOL_TYPE,
        INT_TYPE,
        DOUBLE_TYPE,
        STRING_TYPE,
        ARRAY_TYPE,
        OBJECT_TYPE
    };
    
private:
    Type type_;
    union {
        bool bool_val_;
        int int_val_;
        double double_val_;
    };
    std::string string_val_;
    std::vector<std::shared_ptr<JsonValue>> array_val_;
    std::map<std::string, std::shared_ptr<JsonValue>> object_val_;
    
public:
    JsonValue() : type_(NULL_TYPE) {}
    JsonValue(bool val) : type_(BOOL_TYPE), bool_val_(val) {}
    JsonValue(int val) : type_(INT_TYPE), int_val_(val) {}
    JsonValue(double val) : type_(DOUBLE_TYPE), double_val_(val) {}
    JsonValue(const std::string& val) : type_(STRING_TYPE), string_val_(val) {}
    
    Type getType() const { return type_; }
    
    bool asBool() const { return type_ == BOOL_TYPE ? bool_val_ : false; }
    int asInt() const { return type_ == INT_TYPE ? int_val_ : 0; }
    double asDouble() const { return type_ == DOUBLE_TYPE ? double_val_ : 0.0; }
    std::string asString() const { return type_ == STRING_TYPE ? string_val_ : ""; }
    
    // 数组操作
    void pushBack(std::shared_ptr<JsonValue> val) {
        if (type_ != ARRAY_TYPE) {
            type_ = ARRAY_TYPE;
            array_val_.clear();
        }
        array_val_.push_back(val);
    }
    
    std::shared_ptr<JsonValue> at(size_t index) const {
        if (type_ == ARRAY_TYPE && index < array_val_.size()) {
            return array_val_[index];
        }
        return nullptr;
    }
    
    size_t size() const {
        if (type_ == ARRAY_TYPE) return array_val_.size();
        if (type_ == OBJECT_TYPE) return object_val_.size();
        return 0;
    }
    
    // 对象操作
    void set(const std::string& key, std::shared_ptr<JsonValue> val) {
        if (type_ != OBJECT_TYPE) {
            type_ = OBJECT_TYPE;
            object_val_.clear();
        }
        object_val_[key] = val;
    }
    
    std::shared_ptr<JsonValue> get(const std::string& key) const {
        if (type_ == OBJECT_TYPE) {
            auto it = object_val_.find(key);
            if (it != object_val_.end()) {
                return it->second;
            }
        }
        return nullptr;
    }
    
    bool hasKey(const std::string& key) const {
        return type_ == OBJECT_TYPE && object_val_.find(key) != object_val_.end();
    }
    
    // 序列化为JSON字符串
    std::string toString() const {
        std::ostringstream oss;
        serialize(oss);
        return oss.str();
    }
    
private:
    void serialize(std::ostringstream& oss) const {
        switch (type_) {
            case NULL_TYPE:
                oss << "null";
                break;
            case BOOL_TYPE:
                oss << (bool_val_ ? "true" : "false");
                break;
            case INT_TYPE:
                oss << int_val_;
                break;
            case DOUBLE_TYPE:
                oss << double_val_;
                break;
            case STRING_TYPE:
                oss << "\"" << string_val_ << "\"";
                break;
            case ARRAY_TYPE:
                oss << "[";
                for (size_t i = 0; i < array_val_.size(); ++i) {
                    if (i > 0) oss << ",";
                    array_val_[i]->serialize(oss);
                }
                oss << "]";
                break;
            case OBJECT_TYPE:
                oss << "{";
                bool first = true;
                for (const auto& pair : object_val_) {
                    if (!first) oss << ",";
                    oss << "\"" << pair.first << "\":";
                    pair.second->serialize(oss);
                    first = false;
                }
                oss << "}";
                break;
        }
    }
};

// JSON构建器辅助类
class JsonBuilder {
private:
    std::shared_ptr<JsonValue> root_;
    
public:
    JsonBuilder() : root_(std::make_shared<JsonValue>()) {}
    
    static std::shared_ptr<JsonValue> createObject() {
        auto obj = std::make_shared<JsonValue>();
        // 通过设置一个空键值对来初始化为对象类型
        obj->set("", std::make_shared<JsonValue>());
        obj->get("")->~JsonValue(); // 移除临时键值对
        return obj;
    }
    
    static std::shared_ptr<JsonValue> createArray() {
        auto arr = std::make_shared<JsonValue>();
        // 通过添加一个空值来初始化为数组类型
        arr->pushBack(std::make_shared<JsonValue>());
        return arr;
    }
    
    static std::shared_ptr<JsonValue> createString(const std::string& val) {
        return std::make_shared<JsonValue>(val);
    }
    
    static std::shared_ptr<JsonValue> createInt(int val) {
        return std::make_shared<JsonValue>(val);
    }
    
    static std::shared_ptr<JsonValue> createBool(bool val) {
        return std::make_shared<JsonValue>(val);
    }
};

} // namespace edge_infra

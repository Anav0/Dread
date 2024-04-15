#pragma once

#include <iostream>
#include <map>
#include <stdexcept>

template <typename KeyType, typename ValueType>
class BiMap {
private:
    std::map<KeyType, ValueType> key_to_value;
    std::map<ValueType, KeyType> value_to_key;

public:
    void Insert(KeyType key, ValueType value)
    {
        key_to_value.insert(std::pair(key, value));
        value_to_key.insert(std::pair(value, key));
    }

    ValueType GetValue(const KeyType& key) const
    {
        auto it = key_to_value.find(key);
        if (it != key_to_value.end()) {
            return it->second;
        } else {
            throw std::out_of_range("Key not found");
        }
    }

    KeyType GetKey(const ValueType& value) const
    {
        auto it = value_to_key.find(value);
        if (it != value_to_key.end()) {
            return it->second;
        } else {
            throw std::out_of_range("Value not found");
        }
    }

    bool ContainsKey(const KeyType& key) const
    {
        return key_to_value.find(key) != key_to_value.end();
    }

    bool ContainsValue(const ValueType& value) const
    {
        return value_to_key.find(value) != value_to_key.end();
    }

    void Remove(const KeyType& key)
    {
        auto it = key_to_value.find(key);
        if (it != key_to_value.end()) {
            value_to_key.erase(it->second);
            key_to_value.erase(it);
        }
    }

    void Clear()
    {
        key_to_value.clear();
        value_to_key.clear();
    }
};

#ifndef MAP_H
#define MAP_H

#include <node.h>
#include <string>
#ifdef __APPLE__
#include <tr1/unordered_map>
#define unordered_map std::tr1::unordered_map
#else
#include <unordered_map>
#define unordered_map std::unordered_map
#endif
#include "v8_value_hasher.h"

typedef unordered_map<v8::Handle<v8::Value>,v8::Handle<v8::Value>, v8_value_hash, v8_value_equal_to> MapType;

class NodeMap : public node::ObjectWrap {
public:
    static void init(v8::Handle<v8::Object> exports);

private:
    NodeMap();
    ~NodeMap();

    MapType map;

    // new NodeMap()
    static v8::Handle<v8::Value> Constructor(const v8::Arguments &args);

    // map.set(key, value) : map
    static v8::Handle<v8::Value> Set(const v8::Arguments &args);

    // map.get(key) : value
    static v8::Handle<v8::Value> Get(const v8::Arguments &args);

    // map.has(key) : boolean
    static v8::Handle<v8::Value> Has(const v8::Arguments &args);

    // map.entries() : iterator
    static v8::Handle<v8::Value> Entries(const v8::Arguments &args);

    // map.keys() : iterator
    static v8::Handle<v8::Value> Keys(const v8::Arguments &args);

    // map.values() : iterator
    static v8::Handle<v8::Value> Values(const v8::Arguments &args);

    // map.size : number of elements
    static v8::Handle<v8::Value> Size(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // map.delete(key) : boolean
    static v8::Handle<v8::Value> Delete(const v8::Arguments &args);

    // map.clear() : undefined
    static v8::Handle<v8::Value> Clear(const v8::Arguments &args);

    // map.forEach(function (key, value, map) {...}, context) : undefined
    static v8::Handle<v8::Value> ForEach(const v8::Arguments &args);
};

#endif

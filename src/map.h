#ifndef MAP_H
#define MAP_H

#include <string>
#include <iostream>
#ifdef __APPLE__
#include <tr1/unordered_map>
#define unordered_map std::tr1::unordered_map
#else
#include <unordered_map>
#define unordered_map std::unordered_map
#endif
#include <node.h>
#include <nan.h>
#include "v8_value_hasher.h"

typedef unordered_map<v8::UniquePersistent<v8::Value> *, v8::UniquePersistent<v8::Value> *, v8_value_hash, v8_value_equal_to> MapType;

class NodeMap : public node::ObjectWrap {
public:
    static void init(v8::Handle<v8::Object> exports);

private:
    NodeMap();
    ~NodeMap();

    MapType map;

    // new NodeMap()
    static NAN_METHOD(Constructor);

    // map.set(key, value) : map
    static NAN_METHOD(Set);

    // map.get(key) : value
    static NAN_METHOD(Get);

    // map.has(key) : boolean
    static NAN_METHOD(Has);

    // map.entries() : iterator
    static NAN_METHOD(Entries);

    // map.keys() : iterator
    static NAN_METHOD(Keys);

    // map.values() : iterator
    static NAN_METHOD(Values);

    // map.size : number of elements
    static NAN_GETTER(Size);

    // map.delete(key) : boolean
    static NAN_METHOD(Delete);

    // map.clear() : undefined
    static NAN_METHOD(Clear);

    // map.forEach(function (key, value, map) {...}, context) : undefined
    static NAN_METHOD(ForEach);
};

#endif

#ifndef MAP_H
#define MAP_H

#include <string>
#include <iostream>
#ifdef __APPLE__
#include <tr1/unordered_set>
#define unordered_set std::tr1::unordered_set
#else
#include <unordered_set>
#define unordered_set std::unordered_set
#endif
#include <node.h>
#include <nan.h>
#include "v8_value_hasher.h"

typedef unordered_set<VersionedPersistent, v8_value_hash, v8_value_equal_to> SetType;

class NodeMap : public Nan::ObjectWrap {
public:
    static void init(v8::Local<v8::Object> target);

    uint32_t StartIterator();
    void StopIterator();
    SetType::const_iterator GetBegin();
    SetType::const_iterator GetEnd();

private:
    NodeMap();
    ~NodeMap();

    SetType _set;
    // each time an iterator starts, the _version gets incremented
    // it is used so that items added after an iterator starts are
    // not visited in the iterator
    uint32_t _version;
    // we keep track of how many running iterators there are so that
    // we can clean up when the last iterator is done
    uint32_t _iterator_count;
    // we store the load factor here before the first iterator starts
    // and then set the load factor to infinity so that rehashes of
    // the set don't happen (from inserts that happen inside an iterator)
    // once the last iterator finishes, the load factor is reset, and
    // a rehash might happen
    float _old_load_factor;

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

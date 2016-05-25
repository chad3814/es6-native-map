#ifndef NODE_ITERATOR_H
#define NODE_ITERATOR_H

#include <string>
#include <iostream>
#include <node.h>
#include <nan.h>
#include "map.h"

class PairNodeIterator : public Nan::ObjectWrap {
public:
    static void init(v8::Local<v8::Object> target);
    static v8::Local<v8::Object> New(int type, NodeMap *obj);

    const static int KEY_TYPE = 1;
    const static int VALUE_TYPE = 1 << 1;

private:
    static Nan::Persistent<v8::FunctionTemplate> _constructor;

    PairNodeIterator(int type, NodeMap *map_obj);
    ~PairNodeIterator();

    uint32_t _version;
    MapType::const_iterator _iter;
    MapType::const_iterator _end;
    NodeMap *_map_obj;
    int _type = KEY_TYPE & VALUE_TYPE;

    // iterator[Symbol.iterator]() : this
    static NAN_METHOD(GetThis);

    // iterator.next() : {value:, done:}
    static NAN_METHOD(Next);
};

#endif

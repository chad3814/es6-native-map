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
    static Nan::Persistent<v8::FunctionTemplate> k_constructor;
    static Nan::Persistent<v8::FunctionTemplate> v_constructor;
    static Nan::Persistent<v8::FunctionTemplate> kv_constructor;

    PairNodeIterator(NodeMap *map_obj);
    ~PairNodeIterator();

    uint32_t _version;
    SetType::const_iterator _iter;
    SetType::const_iterator _end;
    NodeMap *_map_obj;

    // iterator.done : boolean
    static NAN_GETTER(GetDone);

    // iterator.key : value
    static NAN_GETTER(GetKey);

    // iterator.value : value
    static NAN_GETTER(GetValue);

    // iterator.next() : undefined
    static NAN_METHOD(Next);
};

#endif

#include "iterator.h"
#include <iostream>

using namespace v8;

Nan::Persistent<FunctionTemplate> PairNodeIterator::_constructor;

void PairNodeIterator::init(Local<Object> target) {
    Local<FunctionTemplate> tmplt = Nan::New<FunctionTemplate>();
    tmplt->SetClassName(Nan::New("NodeMapIterator").ToLocalChecked());
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    _constructor.Reset(tmplt);
    Nan::SetPrototypeMethod(tmplt, "next", Next);

    // got to do the Symbol.iterator function by hand, no Nan support
    Local<Symbol> symbol_iterator = Symbol::GetIterator(Isolate::GetCurrent());
    Local<FunctionTemplate> get_this_templt = Nan::New<FunctionTemplate>(
        GetThis
        , Local<Value>()
        , Nan::New<Signature>(tmplt));
    tmplt->PrototypeTemplate()->Set(symbol_iterator, get_this_templt);
    get_this_templt->SetClassName(Nan::New("Symbol(Symbol.iterator)").ToLocalChecked());
}

Local<Object> PairNodeIterator::New(int type, NodeMap *map_obj) {
    Local<FunctionTemplate> constructor = Nan::New<FunctionTemplate>(_constructor);
    Local<Object> obj;
    PairNodeIterator *iter = new PairNodeIterator(type, map_obj);

    obj = constructor->InstanceTemplate()->NewInstance();

    iter->Wrap(obj);

    return obj;
}

PairNodeIterator::PairNodeIterator(int type, NodeMap *map_obj) {
    this->_map_obj = map_obj;
    this->_version = map_obj->StartIterator();
    this->_iter = map_obj->GetBegin();
    this->_end = map_obj->GetEnd();
    this->_type = type;
}

PairNodeIterator::~PairNodeIterator() {
    this->_map_obj->StopIterator();
}

// iterator[Symbol.iterator]() : this
NAN_METHOD(PairNodeIterator::GetThis) {
    Nan::HandleScope scope;

    info.GetReturnValue().Set(info.This());
}

// iterator.next() : {value:, done:}
NAN_METHOD(PairNodeIterator::Next) {
    Nan::HandleScope scope;

    PairNodeIterator *iter = ObjectWrap::Unwrap<PairNodeIterator >(info.This());
    Local<String> value = Nan::New("value").ToLocalChecked();
    Local<String> done = Nan::New("done").ToLocalChecked();
    Local<Object> obj = Nan::New<Object>();
    Local<Array> arr;

    if (iter->_iter == iter->_end) {
        Nan::Set(obj, value, Nan::Undefined());
        Nan::Set(obj, done, Nan::True());
        info.GetReturnValue().Set(obj);
        return;
    }

    if (iter->_type == KEY_TYPE) {
        obj->Set(value, iter->_iter->GetLocalKey());
    } else if (iter->_type == VALUE_TYPE) {
        obj->Set(value, iter->_iter->GetLocalValue());
    } else {
        arr = Nan::New<Array>(2);
        arr->Set(0, iter->_iter->GetLocalKey());
        arr->Set(1, iter->_iter->GetLocalValue());
        obj->Set(value, arr);
    }
    obj->Set(done, Nan::False());

    iter->_iter++;
    info.GetReturnValue().Set(obj);
    return;
}

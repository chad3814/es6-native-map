#include "iterator.h"
#include <iostream>

using namespace v8;

Nan::Persistent<FunctionTemplate> PairNodeIterator::k_constructor;
Nan::Persistent<FunctionTemplate> PairNodeIterator::v_constructor;
Nan::Persistent<FunctionTemplate> PairNodeIterator::kv_constructor;

void PairNodeIterator::init(Local<Object> target) {
    Local<String> key = Nan::New("key").ToLocalChecked();
    Local<String> value = Nan::New("value").ToLocalChecked();
    Local<String> done = Nan::New("done").ToLocalChecked();
    Local<FunctionTemplate> k_tmplt = Nan::New<FunctionTemplate>();
    Local<FunctionTemplate> v_tmplt = Nan::New<FunctionTemplate>();
    Local<FunctionTemplate> kv_tmplt = Nan::New<FunctionTemplate>();

    k_tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    k_tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    k_constructor.Reset(k_tmplt);
    Nan::SetAccessor(k_tmplt->InstanceTemplate(), key, GetKey);
    Nan::SetAccessor(k_tmplt->InstanceTemplate(), done, GetDone);
    Nan::SetPrototypeMethod(k_tmplt, "next", Next);

    v_tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    v_tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    v_constructor.Reset(v_tmplt);
    Nan::SetAccessor(v_tmplt->InstanceTemplate(), value, GetValue);
    Nan::SetAccessor(v_tmplt->InstanceTemplate(), done, GetDone);
    Nan::SetPrototypeMethod(v_tmplt, "next", Next);

    kv_tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    kv_tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    kv_constructor.Reset(kv_tmplt);
    Nan::SetAccessor(kv_tmplt->InstanceTemplate(), key, GetKey);
    Nan::SetAccessor(kv_tmplt->InstanceTemplate(), value, GetValue);
    Nan::SetAccessor(kv_tmplt->InstanceTemplate(), done, GetDone);
    Nan::SetPrototypeMethod(kv_tmplt, "next", Next);

}

Local<Object> PairNodeIterator::New(int type, NodeMap *map_obj) {
    Local<FunctionTemplate> constructor;
    Local<Object> obj;
    PairNodeIterator *iter = new PairNodeIterator(map_obj);

    if (PairNodeIterator::KEY_TYPE & type) {
        if (PairNodeIterator::VALUE_TYPE & type) {
            constructor = Nan::New<FunctionTemplate>(kv_constructor);
        } else {
            constructor = Nan::New<FunctionTemplate>(k_constructor);
        }
    } else {
        constructor = Nan::New<FunctionTemplate>(v_constructor);
    }

    obj = constructor->InstanceTemplate()->NewInstance();

    iter->Wrap(obj);

    return obj;
}

PairNodeIterator::PairNodeIterator(NodeMap *map_obj) {
    this->_map_obj = map_obj;
    this->_version = map_obj->StartIterator();
    this->_iter = map_obj->GetBegin();
    this->_end = map_obj->GetEnd();
}

PairNodeIterator::~PairNodeIterator() {
    this->_map_obj->StopIterator();
}

// iterator.done : boolean
NAN_GETTER(PairNodeIterator::GetDone) {
    Nan::HandleScope scope;

    PairNodeIterator *obj = Nan::ObjectWrap::Unwrap<PairNodeIterator>(info.This());

    while (obj->_iter != obj->_end &&
           (obj->_iter->IsDeleted() || !obj->_iter->IsValid(obj->_version))) {
        obj->_iter++;
    }

    if (obj->_iter == obj->_end) {
        info.GetReturnValue().Set(Nan::True());
        return;
    }
    info.GetReturnValue().Set(Nan::False());
    return;
}


// iterator.key : value
NAN_GETTER(PairNodeIterator::GetKey) {
    Nan::HandleScope scope;

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.This());

    while (obj->_iter != obj->_end &&
           (obj->_iter->IsDeleted() || !obj->_iter->IsValid(obj->_version))) {
        obj->_iter++;
    }

    if (obj->_iter == obj->_end) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    info.GetReturnValue().Set(obj->_iter->GetLocalKey());
    return;
}

// iterator.value : value
NAN_GETTER(PairNodeIterator::GetValue) {
    Nan::HandleScope scope;

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.This());

    while (obj->_iter != obj->_end &&
           (obj->_iter->IsDeleted() || !obj->_iter->IsValid(obj->_version))) {
        obj->_iter++;
    }

    if (obj->_iter == obj->_end) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    info.GetReturnValue().Set(obj->_iter->GetLocalValue());
    return;
}

// iterator.next() : undefined
NAN_METHOD(PairNodeIterator::Next) {
    Nan::HandleScope scope;

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator >(info.This());

    if (obj->_iter == obj->_end) {
        info.GetReturnValue().Set(info.This());
        return;
    }

    obj->_iter++;
    info.GetReturnValue().Set(info.This());
    return;
}

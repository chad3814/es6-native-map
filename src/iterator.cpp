#include "iterator.h"
#include <iostream>

using namespace v8;

Nan::Persistent<FunctionTemplate> PairNodeIterator::constructor;

void PairNodeIterator::init(Local<Object> target) {
    Local<FunctionTemplate> tmplt = Nan::New<FunctionTemplate>();
    constructor.Reset(tmplt);
    tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetPrototypeMethod(tmplt, "next", Next);
}

Local<Object> PairNodeIterator::New(int type, MapType::const_iterator new_iter, MapType::const_iterator new_end) {
    Local<FunctionTemplate> constr = Nan::New<FunctionTemplate>(constructor);
    Local<Object> obj = constr->InstanceTemplate()->NewInstance();
    PairNodeIterator *iter = new PairNodeIterator(new_iter, new_end);

    iter->Wrap(obj);

    Local<String> key = Nan::New("key").ToLocalChecked();
    Local<String> value = Nan::New("value").ToLocalChecked();
    Local<String> done = Nan::New("done").ToLocalChecked();

    if (PairNodeIterator::KEY_TYPE & type) {
        Nan::SetAccessor(obj, key, GetKey);
    }
    if (PairNodeIterator::VALUE_TYPE & type) {
        Nan::SetAccessor(obj, value, GetValue);
    }

    Nan::SetAccessor(obj, done, GetDone);

    return obj;
}

PairNodeIterator::PairNodeIterator(MapType::const_iterator new_iter, MapType::const_iterator new_end) : iter(new_iter), end(new_end) {}


// iterator.done : boolean
NAN_GETTER(PairNodeIterator::GetDone) {
    Nan::HandleScope scope;

    PairNodeIterator *obj = Nan::ObjectWrap::Unwrap<PairNodeIterator>(info.This());

    if (obj->iter == obj->end) {
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

    if (obj->iter == obj->end) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    info.GetReturnValue().Set(Local<Value>::New(Isolate::GetCurrent(), *obj->iter->first));
    return;
}

// iterator.value : value
NAN_GETTER(PairNodeIterator::GetValue) {
    Nan::HandleScope scope;

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.This());

    if (obj->iter == obj->end) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    info.GetReturnValue().Set(Local<Value>::New(Isolate::GetCurrent(), *obj->iter->second));
    return;
}

// iterator.next() : undefined
NAN_METHOD(PairNodeIterator::Next) {
    Nan::HandleScope scope;

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator >(info.This());

    if (obj->iter == obj->end) {
        info.GetReturnValue().Set(info.This());
        return;
    }

    obj->iter++;
    info.GetReturnValue().Set(info.This());
    return;
}

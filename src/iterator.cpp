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

Local<Object> PairNodeIterator::New(int type, MapType::const_iterator new_iter, MapType::const_iterator new_end) {
    Local<FunctionTemplate> constructor;
    Local<Object> obj;
    PairNodeIterator *iter = new PairNodeIterator(new_iter, new_end);

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

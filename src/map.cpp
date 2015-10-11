#include "map.h"
#include <iostream>
#include "iterator.h"

using namespace v8;

void NodeMap::init(Local<Object> target) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> constructor = Nan::New<FunctionTemplate>(Constructor);

    constructor->SetClassName(Nan::New("NodeMap").ToLocalChecked());
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(constructor, "set", Set);
    Nan::SetPrototypeMethod(constructor, "get", Get);
    Nan::SetPrototypeMethod(constructor, "has", Has);
    Nan::SetPrototypeMethod(constructor, "entries", Entries);
    Nan::SetPrototypeMethod(constructor, "keys", Keys);
    Nan::SetPrototypeMethod(constructor, "values", Values);
    Nan::SetPrototypeMethod(constructor, "delete", Delete);
    Nan::SetPrototypeMethod(constructor, "clear", Clear);
    Nan::SetPrototypeMethod(constructor, "forEach", ForEach);

    target->Set(Nan::New("NodeMap").ToLocalChecked(), constructor->GetFunction());

    PairNodeIterator::init(target);
}

NodeMap::NodeMap() {}

NodeMap::~NodeMap() {
    for(MapType::const_iterator itr = this->map.begin(); itr != this->map.end(); ) {
        itr->first->Reset();
        itr->second->Reset();

        delete itr->first;
        delete itr->second;

        itr = this->map.erase(itr);
    }
}

NAN_METHOD(NodeMap::Constructor) {
    Nan::HandleScope scope;
    NodeMap *obj = new NodeMap();

    Local<String> set = Nan::New("set").ToLocalChecked();
    Local<String> next = Nan::New("next").ToLocalChecked();
    Local<String> done = Nan::New("done").ToLocalChecked();
    Local<String> key = Nan::New("key").ToLocalChecked();
    Local<String> value = Nan::New("value").ToLocalChecked();
    Local<Object> iter;
    Local<Value> func_args[2];

    obj->Wrap(info.This());
    Nan::SetAccessor(info.This(), Nan::New("size").ToLocalChecked(), Size);
    info.GetReturnValue().Set(info.This());

    if(info.Length() > 0) {
        if (!info.This()->Has(set) || !Nan::Get(info.This(), set).ToLocalChecked()->IsFunction()) {
            Nan::ThrowTypeError("Invalid set method");
            return;

        }
        Nan::Callback setter(Nan::Get(info.This(), set).ToLocalChecked().As<Function>());
        if (info[0]->IsObject()) {
            iter = Nan::To<Object>(info[0]).ToLocalChecked();
            if (iter->Has(next) && iter->Get(next)->IsFunction() && iter->Has(key) && iter->Has(value) && iter->Has(done)) {
                Nan::Callback next_func(Nan::Get(iter, next).ToLocalChecked().As<Function>());
                // a value iterator
                while(!Nan::Get(iter, done).ToLocalChecked()->BooleanValue()) {
                    func_args[0] = Nan::Get(iter, key).ToLocalChecked();
                    func_args[1] = Nan::Get(iter, value).ToLocalChecked();
                    setter.Call(info.This(), 2, func_args);
                    next_func.Call(iter, 0, 0);
                }
            }
        }
    }
    return;
}

NAN_METHOD(NodeMap::Get) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || info[0]->IsUndefined() || info[0]->IsNull()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());
    CopyablePersistent *persistent = new Nan::Persistent<Value, Nan::CopyablePersistentTraits<v8::Value> >(info[0]);

    MapType::const_iterator itr = obj->map.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->map.end()) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    Local<Value> ret = Local<Value>::New(Isolate::GetCurrent(), *itr->second);
    info.GetReturnValue().Set(ret);
    return;
}

NAN_METHOD(NodeMap::Has) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || info[0]->IsUndefined() || info[0]->IsNull()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());
    CopyablePersistent *persistent = new Nan::Persistent<Value, Nan::CopyablePersistentTraits<v8::Value> >(info[0]);

    MapType::const_iterator itr = obj->map.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->map.end()) {
        info.GetReturnValue().Set(Nan::False());
        return;
    }

    info.GetReturnValue().Set(Nan::True());
    return;
}

NAN_METHOD(NodeMap::Set) {
    Nan::HandleScope scope;

    if (info.Length() < 2 || info[0]->IsUndefined() || info[0]->IsNull()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());
    CopyablePersistent *pkey = new CopyablePersistent(info[0]);
    CopyablePersistent *pvalue = new CopyablePersistent(info[1]);

    MapType::const_iterator itr = obj->map.find(pkey);

    //overwriting an existing value
    if(itr != obj->map.end()) {
        itr->first->Reset();
        itr->second->Reset();

        delete itr->first;
        delete itr->second;

        obj->map.erase(itr);
    }

    obj->map.insert(std::pair<CopyablePersistent *, CopyablePersistent *>(pkey, pvalue));

    //Return this
    info.GetReturnValue().Set(info.This());
    return;
}

NAN_METHOD(NodeMap::Entries) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE | PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    info.GetReturnValue().Set(iter);
    return;
}

NAN_METHOD(NodeMap::Keys) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE, obj->map.begin(), obj->map.end());

    info.GetReturnValue().Set(iter);
    return;
}

NAN_METHOD(NodeMap::Values) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    info.GetReturnValue().Set(iter);
    return;
}


NAN_METHOD(NodeMap::Delete) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || info[0]->IsUndefined() || info[0]->IsNull()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());
    CopyablePersistent *persistent = new CopyablePersistent(info[0]);

    MapType::const_iterator itr = obj->map.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->map.end()) {
        //do nothing and return false
        info.GetReturnValue().Set(Nan::False());
        return;
    }

    itr->first->Reset();
    itr->second->Reset();

    delete itr->first;
    delete itr->second;

    obj->map.erase(itr);

    info.GetReturnValue().Set(Nan::True());
    return;
}

NAN_METHOD(NodeMap::Clear) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());

    for(MapType::const_iterator itr = obj->map.begin(); itr != obj->map.end(); ) {
        itr->first->Reset();
        itr->second->Reset();

        delete itr->first;
        delete itr->second;

        itr = obj->map.erase(itr);
    }

    info.GetReturnValue().Set(Nan::Undefined());
    return;
}

NAN_GETTER(NodeMap::Size) {
    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());
    uint32_t size = obj->map.size();

    info.GetReturnValue().Set(Nan::New<Integer>(size));
    return;
}

NAN_METHOD(NodeMap::ForEach) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());

    if (info.Length() < 1 || !info[0]->IsFunction()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }
    Local<Function> cb = info[0].As<v8::Function>();

    Local<Object> ctx;
    if (info.Length() > 1 && info[1]->IsObject()) {
        ctx = info[1]->ToObject();
    } else {
        ctx = Nan::GetCurrentContext()->Global();
    }

    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[2] = info.This();

    MapType::const_iterator itr = obj->map.begin();

    while (itr != obj->map.end()) {
        argv[0] = Local<Value>::New(Isolate::GetCurrent(), *itr->second);
        argv[1] = Local<Value>::New(Isolate::GetCurrent(), *itr->first);
        cb->Call(ctx, argc, argv);
        itr++;
    }

    info.GetReturnValue().Set(Nan::Undefined());
    return;
}


extern "C" void
init (Local<Object> target) {
    Nan::HandleScope scope;

    NodeMap::init(target);
}

NODE_MODULE(map, init);

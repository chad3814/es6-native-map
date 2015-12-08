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
    Nan::SetAccessor(constructor->InstanceTemplate(), Nan::New("size").ToLocalChecked(), Size);

    target->Set(Nan::New("NodeMap").ToLocalChecked(), constructor->GetFunction());

    PairNodeIterator::init(target);
}

NodeMap::NodeMap() {
    this->_version = 0;
    this->_iterator_count = 0;
}

NodeMap::~NodeMap() {
    for(MapType::const_iterator itr = this->_set.begin(); itr != this->_set.end(); ) {
        itr = this->_set.erase(itr);
    }
}

uint32_t NodeMap::StartIterator() {
    uint32_t version = this->_version;
    this->_version++;
    if (this->_iterator_count == 0) {
        // if this is the first iterator, set the max load facto to infinity
        // so that a rehash doesn't happen while iterating
        this->_old_load_factor = this->_set.max_load_factor();
        this->_set.max_load_factor(std::numeric_limits<float>::infinity());
    }
    this->_iterator_count++;

    // return the latest version that should be valid for this iterator
    return version;
}

void NodeMap::StopIterator() {
    this->_iterator_count--;
    if (this->_iterator_count != 0) {
        return;
    }
    // that was the last iterator running, so now go through the whole set
    // and actually delete anything marked for deletion
    for(MapType::const_iterator itr = this->_set.begin(); itr != this->_set.end(); ) {
        if (itr->IsDeleted()) {
            itr = this->_set.erase(itr);
        } else {
            itr++;
        }
    }
    // since it was the last iterator, reset the max load factor back
    // to what it was before the first iterator, this might cause a
    // rehash to happen
    this->_set.max_load_factor(this->_old_load_factor);
}

MapType::const_iterator NodeMap::GetBegin() {
    return this->_set.begin();
}

MapType::const_iterator NodeMap::GetEnd() {
    return this->_set.end();
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
    Local<Function> setter;
    Local<Function> next_func;

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());

    if(info.Length() == 0) {
        return;
    }

    if (!info.This()->Has(set) || !Nan::Get(info.This(), set).ToLocalChecked()->IsFunction()) {
        Nan::ThrowTypeError("Invalid set method");
        return;

    }
    setter = Nan::Get(info.This(), set).ToLocalChecked().As<Function>();
    if (info[0]->IsObject()) {
        iter = Nan::To<Object>(info[0]).ToLocalChecked();
        if (iter->Has(next) && iter->Get(next)->IsFunction() && iter->Has(key) && iter->Has(value) && iter->Has(done)) {
            next_func = Nan::Get(iter, next).ToLocalChecked().As<Function>();
            // a value iterator
            while(!Nan::Get(iter, done).ToLocalChecked()->BooleanValue()) {
                func_args[0] = Nan::Get(iter, key).ToLocalChecked();
                func_args[1] = Nan::Get(iter, value).ToLocalChecked();
                setter->Call(info.This(), 2, func_args);
                next_func->Call(iter, 0, 0);
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
    VersionedPersistentPair persistent(obj->_version, info[0]);

    MapType::const_iterator itr = obj->_set.find(persistent);
    MapType::const_iterator end = obj->_set.end();

    while(itr != end && itr->IsDeleted()) {
        itr++;
    }

    if(itr == end || !info[0]->StrictEquals(itr->GetLocalKey())) {
        //do nothing and return undefined
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    info.GetReturnValue().Set(itr->GetLocalValue());
    return;
}

NAN_METHOD(NodeMap::Has) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || info[0]->IsUndefined() || info[0]->IsNull()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());
    VersionedPersistentPair persistent(obj->_version, info[0]);

    MapType::const_iterator itr = obj->_set.find(persistent);
    MapType::const_iterator end = obj->_set.end();

    while(itr != end && itr->IsDeleted()) {
        itr++;
    }

    if(itr == end || !info[0]->StrictEquals(itr->GetLocalKey())) {
        //do nothing and return false
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
    VersionedPersistentPair *persistent = new VersionedPersistentPair(obj->_version, info[0], info[1]);

    MapType::const_iterator itr = obj->_set.find(*persistent);
    MapType::const_iterator end = obj->_set.end();

    while(itr != end && itr->IsDeleted()) {
        itr++;
    }

    if(itr != end && info[0]->StrictEquals(itr->GetLocalKey())) {
        itr->ReplaceValue(obj->_version, info[1]);
        delete persistent;
    } else {
        obj->_set.insert(*persistent);
    }

    //Return this
    info.GetReturnValue().Set(info.This());
    return;
}

NAN_METHOD(NodeMap::Entries) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE | PairNodeIterator::VALUE_TYPE, obj);

    info.GetReturnValue().Set(iter);
    return;
}

NAN_METHOD(NodeMap::Keys) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE, obj);

    info.GetReturnValue().Set(iter);
    return;
}

NAN_METHOD(NodeMap::Values) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::VALUE_TYPE, obj);

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
    VersionedPersistentPair persistent(obj->_version, info[0]);
    bool using_iterator = (obj->_iterator_count != 0);
    bool ret;

    if (using_iterator) {
        obj->StartIterator();
    }

    MapType::const_iterator itr = obj->_set.find(persistent);
    MapType::const_iterator end = obj->_set.end();

    while(itr != end && itr->IsDeleted()) {
        itr++;
    }

    ret = (itr != end && info[0]->StrictEquals(itr->GetLocalKey()));

    if (using_iterator) {
        if (ret) {
            itr->Delete();
        }
        obj->StopIterator();
    } else {
        if (ret) {
            obj->_set.erase(itr);
        }
    }

    if (ret) {
        info.GetReturnValue().Set(Nan::True());
    } else {
        info.GetReturnValue().Set(Nan::False());
    }
    return;
}

NAN_METHOD(NodeMap::Clear) {
    Nan::HandleScope scope;

    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());
    bool using_iterator = (obj->_iterator_count != 0);

    if (using_iterator) {
        obj->StartIterator();
    }

    for(MapType::const_iterator itr = obj->_set.begin(); itr != obj->_set.end(); ) {
        if (using_iterator) {
            itr->Delete();
            itr++;
        } else {
            itr = obj->_set.erase(itr);
        }
    }

    if (using_iterator) {
        obj->StopIterator();
    }

    info.GetReturnValue().Set(Nan::Undefined());
    return;
}

NAN_GETTER(NodeMap::Size) {
    NodeMap *obj = Nan::ObjectWrap::Unwrap<NodeMap>(info.This());
    uint32_t size = 0;
    if (obj->_iterator_count == 0) {
        size = obj->_set.size();
        info.GetReturnValue().Set(Nan::New<Integer>(size));
        return;
    }

    MapType::const_iterator itr = obj->_set.begin();
    MapType::const_iterator end = obj->_set.end();
    for (; itr != end; itr++) {
        if (itr->IsValid(obj->_version)) {
            size += 1;
        }
    }

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

    uint32_t version = obj->StartIterator();
    MapType::const_iterator itr = obj->_set.begin();
    MapType::const_iterator end = obj->_set.end();

    while (itr != end) {
        if (itr->IsValid(version)) {
            argv[0] = itr->GetLocalValue();
            argv[1] = itr->GetLocalKey();
            cb->Call(ctx, argc, argv);
        }
        itr++;
    }
    obj->StopIterator();

    info.GetReturnValue().Set(Nan::Undefined());
    return;
}


extern "C" void
init (Local<Object> target) {
    Nan::HandleScope scope;

    NodeMap::init(target);
}

NODE_MODULE(map, init);

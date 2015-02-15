#include "map.h"
#include <iostream>
#include "iterator.h"

using namespace v8;

void NodeMap::init(Handle<Object> exports) {
    Local<FunctionTemplate> constructor = FunctionTemplate::New(Constructor);
    constructor->SetClassName(String::NewSymbol("NodeMap"));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Local<ObjectTemplate> prototype = constructor->PrototypeTemplate();
    prototype->Set("set", FunctionTemplate::New(Set)->GetFunction());
    prototype->Set("get", FunctionTemplate::New(Get)->GetFunction());
    prototype->Set("has", FunctionTemplate::New(Has)->GetFunction());
    prototype->Set("keys", FunctionTemplate::New(Keys)->GetFunction());
    prototype->Set("values", FunctionTemplate::New(Values)->GetFunction());
    prototype->Set("entries", FunctionTemplate::New(Entries)->GetFunction());
    prototype->Set("delete", FunctionTemplate::New(Delete)->GetFunction());
    prototype->Set("clear", FunctionTemplate::New(Clear)->GetFunction());
    prototype->Set("forEach", FunctionTemplate::New(ForEach)->GetFunction());

    exports->Set(String::NewSymbol("NodeMap"), Persistent<Function>::New(constructor->GetFunction()));

    PairNodeIterator::init();
}

NodeMap::NodeMap() {}

NodeMap::~NodeMap() {
    for(MapType::const_iterator itr = this->map.begin(); itr != this->map.end(); ) {
        Persistent<Value> key = Persistent<Value>::Persistent(itr->first);
        key.Dispose();
        key.Clear();

        Persistent<Value> value = Persistent<Value>::Persistent(itr->second);
        value.Dispose();
        value.Clear();

        itr = this->map.erase(itr);
    }
}

Handle<Value> NodeMap::Constructor(const Arguments& args) {
    HandleScope scope;
    NodeMap *obj = new NodeMap();
    MapType::const_iterator itr;
    Local<Function> setter;

    Local<String> set = String::New("set");
    Local<String> next = String::New("next");
    Local<String> done = String::New("done");
    Local<String> key = String::New("key");
    Local<String> value = String::New("value");
    Local<Object> iter;
    Local<Function> next_func;
    Local<Value> func_args[2];
    Local<Value> empty_args[0];

    obj->Wrap(args.This());
    args.This()->SetAccessor(String::New("size"), Size);

    if(args.Length() > 0) {
        if (!args.This()->Has(set) || !args.This()->Get(set)->IsFunction()) {
            ThrowException(Exception::TypeError(String::New("Invalid set method")));
            return scope.Close(args.This());
        }
        setter = Local<Function>::Cast(args.This()->Get(set));
        if (args[0]->IsObject()) {
            iter = Local<Object>::Cast(args[0]);
            if (iter->Has(next) && iter->Get(next)->IsFunction() && iter->Has(key) && iter->Has(value) && iter->Has(done)) {
                next_func = Local<Function>::Cast(iter->Get(next));
                // a value iterator
                while(!iter->Get(done)->BooleanValue()) {
                    func_args[0] = iter->Get(key);
                    func_args[1] = iter->Get(value);
                    setter->Call(args.This(), 2, func_args);
                    next_func->Call(iter, 0, empty_args);
                }
            }
        }
    }

    return scope.Close(args.This());
}

Handle<Value> NodeMap::Get(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    MapType::const_iterator itr = obj->map.find(args[0]);

    if(itr == obj->map.end()) {
        return scope.Close(Undefined()); //return undefined
    }

    Persistent<Value> value = Persistent<Value>::Persistent(itr->second);

    return scope.Close(value);
}

Handle<Value> NodeMap::Has(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    MapType::const_iterator itr = obj->map.find(args[0]);

    if(itr == obj->map.end()) {
        return scope.Close(False()); //return undefined
    }

    return scope.Close(True());
}

Handle<Value> NodeMap::Set(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 2) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    MapType::const_iterator itr = obj->map.find(args[0]);

    //overwriting an existing value
    if(itr != obj->map.end()) {
        Persistent<Value> oldKey = Persistent<Value>::Persistent(itr->first);
        oldKey.Dispose();
        oldKey.Clear();

        Persistent<Value> oldValue = Persistent<Value>::Persistent(itr->second);
        oldValue.Dispose(); //release the handle to the GC
        oldValue.Clear();

        obj->map.erase(itr);
    }

    Persistent<Value> key = Persistent<Value>::New(args[0]);
    Persistent<Value> value = Persistent<Value>::New(args[1]);

    obj->map.insert(std::pair<Persistent<Value>, Persistent<Value> >(key, value));

    //Return this
    return scope.Close(args.This());
}

Handle<Value> NodeMap::Entries(const Arguments& args) {
    HandleScope scope;

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE | PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    return scope.Close(iter);
}

Handle<Value> NodeMap::Keys(const Arguments& args) {
    HandleScope scope;

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE, obj->map.begin(), obj->map.end());

    return scope.Close(iter);
}

Handle<Value> NodeMap::Values(const Arguments& args) {
    HandleScope scope;

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    return scope.Close(iter);
}


Handle<Value> NodeMap::Delete(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Boolean::New(false));
    }

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    MapType::const_iterator itr = obj->map.find(args[0]);

    if(itr == obj->map.end()) {
        return scope.Close(Boolean::New(false)); //do nothing and return false
    }

    Persistent<Value> key = Persistent<Value>::Persistent(itr->first);
    key.Dispose();
    key.Clear();

    Persistent<Value> value = Persistent<Value>::Persistent(itr->second);
    value.Dispose();
    value.Clear();

    obj->map.erase(itr);

    return scope.Close(Boolean::New(true));
}

Handle<Value> NodeMap::Clear(const Arguments& args) {
    HandleScope scope;

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    for(MapType::const_iterator itr = obj->map.begin(); itr != obj->map.end(); ) {
        Persistent<Value> key = Persistent<Value>::Persistent(itr->first);
        key.Dispose();
        key.Clear();

        Persistent<Value> value = Persistent<Value>::Persistent(itr->second);
        value.Dispose();
        value.Clear();

        itr = obj->map.erase(itr);
    }

    return scope.Close(Undefined());
}

Handle<Value> NodeMap::Size(Local<String> property, const AccessorInfo &info) {
    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(info.Holder());

    return Integer::New(obj->map.size());
}

Handle<Value> NodeMap::ForEach(const Arguments& args) {
    HandleScope scope;

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    if (args.Length() < 1 || !args[0]->IsFunction()) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }
    Local<Function> cb = Local<Function>::Cast(args[0]);

    Local<Object> ctx;
    if (args.Length() > 1 && args[1]->IsObject()) {
        ctx = args[1]->ToObject();
    } else {
        ctx = Context::GetCurrent()->Global();
    }

    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[2] = args.This();

    MapType::const_iterator itr = obj->map.begin();

    while (itr != obj->map.end()) {
        argv[0] = Local<Value>::New(itr->second);
        argv[1] = Local<Value>::New(itr->first);
        cb->Call(ctx, argc, argv);
        itr++;
    }

    return scope.Close(Undefined());
}

#include "map.h"
#include <iostream>
#include "iterator.h"

using namespace v8;

#define PROTO(proto, js_name, cpp_name) \
    (proto)->Set(NanNew<String>(#js_name), \
                 NanNew<FunctionTemplate>(cpp_name)->GetFunction())


void NodeMap::init(Handle<Object> exports) {
    Local<FunctionTemplate> constructor = NanNew<FunctionTemplate>(Constructor);
    constructor->SetClassName(NanNew<String>("NodeMap"));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Local<ObjectTemplate> prototype = constructor->PrototypeTemplate();
    PROTO(prototype, set, Set);
    PROTO(prototype, get, Get);
    PROTO(prototype, has, Has);
    PROTO(prototype, entries, Entries);
    PROTO(prototype, keys, Keys);
    PROTO(prototype, values, Values);
    PROTO(prototype, delete, Delete);
    PROTO(prototype, clear, Clear);
    PROTO(prototype, forEach, ForEach);

    exports->Set(NanNew<String>("NodeMap"), constructor->GetFunction());

    PairNodeIterator::init();
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
    NanScope();
    NodeMap *obj = new NodeMap();
    Local<Function> setter;

    Local<String> set = NanNew<String>("set");
    Local<String> next = NanNew<String>("next");
    Local<String> done = NanNew<String>("done");
    Local<String> key = NanNew<String>("key");
    Local<String> value = NanNew<String>("value");
    Local<Object> iter;
    Local<Function> next_func;
    Local<Value> func_args[2];
    Local<Value> empty_args[0];

    obj->Wrap(args.This());
    args.This()->SetAccessor(NanNew<String>("size"), Size);

    if(args.Length() > 0) {
        if (!args.This()->Has(set) || !args.This()->Get(set)->IsFunction()) {
            NanThrowTypeError("Invalid set method");
            NanReturnThis();
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

    NanReturnThis();
}

NAN_METHOD(NodeMap::Get) {
    NanScope();

    if (args.Length() < 1 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());
    Isolate* isolate = Isolate::GetCurrent();
    UniquePersistent<Value> *persistent = new UniquePersistent<Value>(isolate, args[0]);

    MapType::const_iterator itr = obj->map.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->map.end()) {
        NanReturnUndefined();
    }

    Local<Value> ret = Local<Value>::New(isolate, *itr->second);
    NanReturnValue(ret);
}

NAN_METHOD(NodeMap::Has) {
    NanScope();

    if (args.Length() < 1 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnValue(NanFalse());
    }

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());
    Isolate* isolate = Isolate::GetCurrent();
    UniquePersistent<Value> *persistent = new UniquePersistent<Value>(isolate, args[0]);

    MapType::const_iterator itr = obj->map.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->map.end()) {
        NanReturnValue(NanFalse());
    }

    NanReturnValue(NanTrue());
}

NAN_METHOD(NodeMap::Set) {
    NanScope();

    if (args.Length() < 2 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());
    Isolate* isolate = Isolate::GetCurrent();
    UniquePersistent<Value> *pkey = new UniquePersistent<Value>(isolate, args[0]);
    UniquePersistent<Value> *pvalue = new UniquePersistent<Value>(isolate, args[1]);

    MapType::const_iterator itr = obj->map.find(pkey);

    //overwriting an existing value
    if(itr != obj->map.end()) {
        itr->first->Reset();
        itr->second->Reset();

        delete itr->first;
        delete itr->second;

        obj->map.erase(itr);
    }

    obj->map.insert(std::pair<UniquePersistent<Value> *, UniquePersistent<Value> *>(pkey, pvalue));

    //Return this
    NanReturnThis();
}

NAN_METHOD(NodeMap::Entries) {
    NanScope();

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE | PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    NanReturnValue(iter);
}

NAN_METHOD(NodeMap::Keys) {
    NanScope();

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE, obj->map.begin(), obj->map.end());

    NanReturnValue(iter);
}

NAN_METHOD(NodeMap::Values) {
    NanScope();

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    NanReturnValue(iter);
}


NAN_METHOD(NodeMap::Delete) {
    NanScope();

    if (args.Length() < 1 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnValue(NanFalse());
    }

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());
    Isolate* isolate = Isolate::GetCurrent();
    UniquePersistent<Value> *persistent = new UniquePersistent<Value>(isolate, args[0]);

    MapType::const_iterator itr = obj->map.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->map.end()) {
        //do nothing and return false
        NanReturnValue(NanFalse());
    }

    itr->first->Reset();
    itr->second->Reset();

    delete itr->first;
    delete itr->second;

    obj->map.erase(itr);

    NanReturnValue(NanTrue());
}

NAN_METHOD(NodeMap::Clear) {
    NanScope();

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());

    for(MapType::const_iterator itr = obj->map.begin(); itr != obj->map.end(); ) {
        itr->first->Reset();
        itr->second->Reset();

        delete itr->first;
        delete itr->second;

        itr = obj->map.erase(itr);
    }

    NanReturnUndefined();
}

NAN_GETTER(NodeMap::Size) {
    NanScope();

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());
    uint32_t size = obj->map.size();

    NanReturnValue(NanNew<Integer>(size));
}

NAN_METHOD(NodeMap::ForEach) {
    NanScope();

    NodeMap *obj = ObjectWrap::Unwrap<NodeMap>(args.This());
    Isolate* isolate = Isolate::GetCurrent();

    if (args.Length() < 1 || !args[0]->IsFunction()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }
    Local<Function> cb = Local<Function>::Cast(args[0]);

    Local<Object> ctx;
    if (args.Length() > 1 && args[1]->IsObject()) {
        ctx = args[1]->ToObject();
    } else {
        ctx = NanGetCurrentContext()->Global();
    }

    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[2] = args.This();

    MapType::const_iterator itr = obj->map.begin();

    while (itr != obj->map.end()) {
        argv[0] = Local<Value>::New(isolate, *itr->second);
        argv[1] = Local<Value>::New(isolate, *itr->first);
        cb->Call(ctx, argc, argv);
        itr++;
    }

    NanReturnUndefined();
}

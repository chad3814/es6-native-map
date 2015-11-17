#ifndef V8_VALUE_HASHER_H
#define V8_VALUE_HASHER_H

#include <string>
#include <iostream>
#include <node.h>
#ifdef __APPLE__
#include <tr1/unordered_set>
#define hash std::tr1::hash
#else
#include <unordered_set>
#define hash std::hash
#endif
#include <nan.h>

class VersionedPersistentPair {
public:
    VersionedPersistentPair(uint32_t version, v8::Local<v8::Value> key) : _version(version), _is_deleted(false) {
        _persistent_key.Reset(key);
    }

    VersionedPersistentPair(uint32_t version, v8::Local<v8::Value> key, v8::Local<v8::Value> value) : _version(version), _is_deleted(false) {
        _persistent_key.Reset(key);
        _persistent_value.Reset(value);
    }

    VersionedPersistentPair(const VersionedPersistentPair &copy) {
        Nan::HandleScope scope;
        _is_deleted = copy._is_deleted;
        _version = copy._version;
        _persistent_key.Reset(copy.GetLocalKey());
        _persistent_value.Reset(copy.GetLocalValue());
    }

    ~VersionedPersistentPair() {
        this->Delete();
    }

    void Delete() const {
        if (_is_deleted) {
            return;
        }

        _is_deleted = true;
        _persistent_key.Reset();
        _persistent_value.Reset();
    }

    bool IsDeleted() const {
        return _is_deleted;
    }

    bool IsValid(uint32_t version) const {
        return !_is_deleted && (_version <= version);
    }

    v8::Local<v8::Value> GetLocalKey() const {
        return v8::Local<v8::Value>::New(v8::Isolate::GetCurrent(), this->_persistent_key);
    }

    v8::Local<v8::Value> GetLocalValue() const {
        return v8::Local<v8::Value>::New(v8::Isolate::GetCurrent(), this->_persistent_value);
    }

private:
    mutable uint32_t _version;
    mutable bool _is_deleted;
    mutable Nan::Persistent<v8::Value> _persistent_key;
    mutable Nan::Persistent<v8::Value> _persistent_value;
};


struct v8_value_hash
{
    size_t operator()(VersionedPersistentPair k) const {
        Nan::HandleScope scope;
        v8::Local<v8::Value> key = k.GetLocalKey();

        std::string s;
        if (key->IsString() || key->IsBoolean() || key->IsNumber()) {
            return hash<std::string>()(*Nan::Utf8String(key));
        }
        return hash<int>()(Nan::To<v8::Object>(key).ToLocalChecked()->GetIdentityHash());
    }
};

struct v8_value_equal_to
{
    bool operator()(VersionedPersistentPair pa, VersionedPersistentPair pb) const {
        Nan::HandleScope scope;

        if (pa.IsDeleted() || pb.IsDeleted()) {
            return false;
        }

        v8::Local<v8::Value> a = pa.GetLocalKey();
        v8::Local<v8::Value> b = pb.GetLocalKey();

        if (a->StrictEquals(b)) {          /* same as JS === */
            return true;
        }

        return Nan::To<v8::Object>(a).ToLocalChecked()->GetIdentityHash() == Nan::To<v8::Object>(b).ToLocalChecked()->GetIdentityHash();
    }
};

#endif

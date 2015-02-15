#include <node.h>
#include <v8.h>
#include "map.h"

using namespace v8;

void init(Handle<Object> exports) {
  NodeMap::init(exports);
}

NODE_MODULE(native, init);

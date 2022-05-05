#include "key_value_store.h"

// Fetch value from the db.
optional<string> KV_store::get_value(string key) {
  unordered_map<string, unique_ptr<string>>::const_iterator it =
    kv_map.find (key);
  if (it != kv_map.end()) {
    return *(it->second);
  }
  return nullopt;
}

// Insert value into the db.
void KV_store::insert_value(string key, string value) {
  if (kv_map.find (key) == kv_map.end()) {
    kv_map.emplace(key, make_unique<string>(value));
  } else {
    /* Modify the existing key, since value could be of different size, let
       dealloc and allocate again */
    kv_map[key].reset();
    kv_map[key] = make_unique<string>(value);
  }
}

// Delete value in the db.
bool KV_store::delete_key(string key) {
  // Unimplemented
  return true;
}

int KV_store::get_kv_map_size() {
  return kv_map.size();
}

shared_ptr<KV_store> KV_store::kv_obj = nullptr;

mutex KV_store::mtx;

shared_ptr<KV_store> KV_store::get_kv_obj() {
  if (kv_obj == nullptr) {
    // Mutex is only called 1 time.
    if (kv_obj == nullptr) {
      mtx.lock();
      kv_obj = make_shared<KV_store>();
      mtx.unlock();
    }
  }
  return kv_obj;
}

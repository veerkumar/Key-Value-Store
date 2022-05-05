#ifndef _KEY_VALUE_STORE_H_
#define _KEY_VALUE_STORE_H_

#include <memory>
#include <optional>
#include <iostream>
#include <unordered_map>
#include <mutex>

using namespace std;
/* Since DB is readonly, getter/setter function in this
  class are not thread safe. Throug KV_store object is thread-safe */
class KV_store {
  private:
    // Volatile ensured that every thread read explicitly.
    static  shared_ptr<KV_store> kv_obj;
    static mutex mtx;
    // Since we only have readonly type of queries (as data is always loaded
    // at the server start time). Also queries always provides exact key
    // (i.e not regular expression), and there is no deletion of keys as well
    // (hence no fregment), Hence, having an LSM tree (or B+ trees) will be an
    // overkill.
    // To make hash-table fit in memory and give linear predection on memory
    // consumption, we store pointer to the values.
    unordered_map<string, std::unique_ptr<string>> kv_map;

  public:
    KV_store() {
      kv_map.reserve(1000000);
    }
    static shared_ptr<KV_store> get_kv_obj();

    // Fetch value from the db.
    optional<string> get_value(string key);

    //Insert value in the db
    void insert_value(string key, string value);

    // Delete value in the db
    bool delete_key(string key);

    int get_kv_map_size();
};

#endif

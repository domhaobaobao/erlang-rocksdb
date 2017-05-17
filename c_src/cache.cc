// Copyright (c) 2016-2017 Benoit Chesneau
//
// This file is provided to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file
// except in compliance with the License.  You may obtain
// a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <vector>

#include "rocksdb/db.h"
#include "rocksdb/cache.h"

#include "erocksdb.h"
#include "cache.h"

#ifndef ATOMS_H
    #include "atoms.h"
#endif

#ifndef INCL_UTIL_H
    #include "util.h"
#endif


namespace erocksdb {

ErlNifResourceType * Cache::m_Cache_RESOURCE(NULL);

void
Cache::CreateCacheType( ErlNifEnv * env)
{
    ErlNifResourceFlags flags = (ErlNifResourceFlags)(ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER);
    m_Cache_RESOURCE = enif_open_resource_type(env, NULL, "erocksdb_Cache",
                                            &Cache::CacheResourceCleanup,
                                            flags, NULL);
    return;
}   // Cache::CreateCacheType


void
Cache::CacheResourceCleanup(ErlNifEnv *Env, void * Arg)
{
    return;
}   // Cache::CacheResourceCleanup


Cache *
Cache::CreateCacheResource(std::shared_ptr<rocksdb::Cache> cache)
{
    Cache * ret_ptr;
    void * alloc_ptr;

    alloc_ptr=enif_alloc_resource(m_Cache_RESOURCE, sizeof(Cache));
    ret_ptr=new (alloc_ptr) Cache(cache);
    return(ret_ptr);
}

Cache *
Cache::RetrieveCacheResource(ErlNifEnv * Env, const ERL_NIF_TERM & CacheTerm)
{
    Cache * ret_ptr;
    if (!enif_get_resource(Env, CacheTerm, m_Cache_RESOURCE, (void **)&ret_ptr))
        return NULL;
    return ret_ptr;
}

Cache::Cache(std::shared_ptr<rocksdb::Cache> Cache) : cache_(Cache) {}

Cache::~Cache()
{
    if(cache_)
    {
        cache_ = NULL;
    }

    return;
}

std::shared_ptr<rocksdb::Cache> Cache::cache() { return cache_; }


ERL_NIF_TERM
NewLRUCache(
        ErlNifEnv* env,
        int argc,
        const ERL_NIF_TERM argv[])
{
    ErlNifUInt64 capacity;
    Cache* cache_ptr;
    if(!enif_get_uint64(env, argv[0], &capacity))
        return enif_make_badarg(env);
    std::shared_ptr<rocksdb::Cache> cache = rocksdb::NewLRUCache(capacity);
    cache_ptr = Cache::CreateCacheResource(cache);
    // create a resource reference to send erlang
    ERL_NIF_TERM result = enif_make_resource(env, cache_ptr);
    // clear the automatic reference from enif_alloc_resource in EnvObject
    enif_release_resource(cache_ptr);
    cache = NULL;
    return enif_make_tuple2(env, ATOM_OK, result);
}

ERL_NIF_TERM
NewClockCache(
        ErlNifEnv* env,
        int argc,
        const ERL_NIF_TERM argv[])
{
    ErlNifUInt64 capacity;
    Cache* cache_ptr;
    if(!enif_get_uint64(env, argv[0], &capacity))
        return enif_make_badarg(env);
    std::shared_ptr<rocksdb::Cache> cache = rocksdb::NewClockCache(capacity);
    cache_ptr = Cache::CreateCacheResource(cache);
    // create a resource reference to send erlang
    ERL_NIF_TERM result = enif_make_resource(env, cache_ptr);
    // clear the automatic reference from enif_alloc_resource in EnvObject
    enif_release_resource(cache_ptr);
    cache = NULL;
    return enif_make_tuple2(env, ATOM_OK, result);
}

}

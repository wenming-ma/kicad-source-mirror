
#pragma once
#ifndef INCLUDE_THREAD_POOL_H_
#define INCLUDE_THREAD_POOL_H_

#include <bs_thread_pool.hpp>
#include <import_export.h>

using thread_pool = BS::thread_pool;

APIEXPORT thread_pool& GetKiCadThreadPool();


#endif /* INCLUDE_THREAD_POOL_H_ */


/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_PALLOC_H_INCLUDED_
#define _NGX_PALLOC_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define NGX_MAX_ALLOC_FROM_POOL  (ngx_pagesize - 1)

#define NGX_DEFAULT_POOL_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16
#define NGX_MIN_POOL_SIZE                                                     \
    ngx_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)),            \
              NGX_POOL_ALIGNMENT)


typedef void (*ngx_pool_cleanup_pt)(void *data);

typedef struct ngx_pool_cleanup_s  ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler;
    void                 *data;
    ngx_pool_cleanup_t   *next;
};


typedef struct ngx_pool_large_s  ngx_pool_large_t;

struct ngx_pool_large_s {
    ngx_pool_large_t     *next;
    void                 *alloc;
};


typedef struct {
    u_char               *last;         // 表示内存池中下一块可用内存的起始地址，也就是内存池中已经使用的内存的结束地址
    u_char               *end;          // 表示内存池的结束地址，也就是内存池中已经分配的内存的结束地址。
    ngx_pool_t           *next;         // 表示下一个内存池，当当前内存池中的内存不足以满足请求时，会从下一个内存池中继续分配内存。
    ngx_uint_t            failed;       // 表示当前内存池中已经失败的次数，也就是内存池中已经没有足够的内存可用的次数，当failed达到一定的值后，会自动创建一个新的内存池。
} ngx_pool_data_t;


struct ngx_pool_s {
    ngx_pool_data_t       d;            // 内存池数据区域
    size_t                max;          // 内存池数据区域的大小，用于限制内存池最多可以分配的内存大小
    ngx_pool_t           *current;      // 指向内存池数据区域中当前可分配内存的位置，即指向一个空闲内存块的指针。
    ngx_chain_t          *chain;        // 一条链表，用于管理内存池分配的小内存块，每个链表节点ngx_chain_t都包含一个内存块指针和下一个链表节点指针。
    ngx_pool_large_t     *large;        // 指向当前内存池中所有大内存块的链表头部。
    ngx_pool_cleanup_t   *cleanup;      // 用于记录内存池中需要在销毁时执行的清理函数。
    ngx_log_t            *log;          // 内存池所属的日志对象。
};


typedef struct {
    ngx_fd_t              fd;
    u_char               *name;
    ngx_log_t            *log;
} ngx_pool_cleanup_file_t;


ngx_pool_t *ngx_create_pool(size_t size, ngx_log_t *log);
void ngx_destroy_pool(ngx_pool_t *pool);
void ngx_reset_pool(ngx_pool_t *pool);

void *ngx_palloc(ngx_pool_t *pool, size_t size);
void *ngx_pnalloc(ngx_pool_t *pool, size_t size);
void *ngx_pcalloc(ngx_pool_t *pool, size_t size);
void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment);
ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);


ngx_pool_cleanup_t *ngx_pool_cleanup_add(ngx_pool_t *p, size_t size);
void ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd);
void ngx_pool_cleanup_file(void *data);
void ngx_pool_delete_file(void *data);


#endif /* _NGX_PALLOC_H_INCLUDED_ */

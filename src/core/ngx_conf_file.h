
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CONF_FILE_H_INCLUDED_
#define _NGX_CONF_FILE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 *        AAAA  number of arguments
 *      FF      command flags
 *    TT        command type, i.e. HTTP "location" or "server" command
 */

// 不需要参数
#define NGX_CONF_NOARGS      0x00000001
// 需要1个参数
#define NGX_CONF_TAKE1       0x00000002
#define NGX_CONF_TAKE2       0x00000004
#define NGX_CONF_TAKE3       0x00000008
#define NGX_CONF_TAKE4       0x00000010
#define NGX_CONF_TAKE5       0x00000020
#define NGX_CONF_TAKE6       0x00000040
#define NGX_CONF_TAKE7       0x00000080

// 支持的最大参数个数
#define NGX_CONF_MAX_ARGS    8

// 需要不同个数参数的组合
#define NGX_CONF_TAKE12      (NGX_CONF_TAKE1|NGX_CONF_TAKE2)
#define NGX_CONF_TAKE13      (NGX_CONF_TAKE1|NGX_CONF_TAKE3)

#define NGX_CONF_TAKE23      (NGX_CONF_TAKE2|NGX_CONF_TAKE3)

#define NGX_CONF_TAKE123     (NGX_CONF_TAKE1|NGX_CONF_TAKE2|NGX_CONF_TAKE3)
#define NGX_CONF_TAKE1234    (NGX_CONF_TAKE1|NGX_CONF_TAKE2|NGX_CONF_TAKE3   \
                              |NGX_CONF_TAKE4)

// 参数个数掩码
#define NGX_CONF_ARGS_NUMBER 0x000000ff
// 块级配置指令
#define NGX_CONF_BLOCK       0x00000100
// bool类型配置指令
#define NGX_CONF_FLAG        0x00000200
// 接受任意个参数
#define NGX_CONF_ANY         0x00000400
// 至少需要一个参数
#define NGX_CONF_1MORE       0x00000800
// 至少需要两个参数
#define NGX_CONF_2MORE       0x00001000

// 只能在配置文件中使用，不能在命令行中使用
#define NGX_DIRECT_CONF      0x00010000

// 用于 core 模块，表示指令只能在主配置文件中出现一次
#define NGX_MAIN_CONF        0x01000000
// 用于配置指令的类型，匹配任何类型的指令
#define NGX_ANY_CONF         0xFF000000



// 未设置的值
#define NGX_CONF_UNSET       -1
#define NGX_CONF_UNSET_UINT  (ngx_uint_t) -1
#define NGX_CONF_UNSET_PTR   (void *) -1
#define NGX_CONF_UNSET_SIZE  (size_t) -1
#define NGX_CONF_UNSET_MSEC  (ngx_msec_t) -1


// 配置文件解析成功的标志
#define NGX_CONF_OK          NULL
// 配置文件解析失败的标志
#define NGX_CONF_ERROR       (void *) -1

// 块级指令相关的标志
#define NGX_CONF_BLOCK_START 1
#define NGX_CONF_BLOCK_DONE  2
#define NGX_CONF_FILE_DONE   3

// 核心模块和配置模块的标志
#define NGX_CORE_MODULE      0x45524F43  /* "CORE" */
#define NGX_CONF_MODULE      0x464E4F43  /* "CONF" */


// 配置文件解析错误信息的最大长度
#define NGX_MAX_CONF_ERRSTR  1024


// 配置文件中配置项的信息
struct ngx_command_s {
    ngx_str_t             name;             // 配置项的名称，也就是在配置文件中使用的关键字
    ngx_uint_t            type;             // 配置项的类型，是一个整数，定义在 ngx_conf_enum_t 中，
                                            // 表示这个配置项应该是什么类型的参数，比如是字符串、数字、枚举等等。
    // 一个函数指针，指向一个函数，用于解析并处理配置项的参数。
    char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
    ngx_uint_t            conf;             // 这个字段不是直接使用的，是给 set 函数指针使用的，通常用于存储配置项的值。
    ngx_uint_t            offset;           // 这个字段不是直接使用的，是给 set 函数指针使用的，
                                            // 表示在 ngx_http_conf_ctx_t 结构体中的偏移量，用于存储配置项的值。
    void                 *post;             // 这个字段不是必须的，如果需要在解析完这个配置项之后执行一些操作，可以设置这个字段为一个函数指针。
};

#define ngx_null_command  { ngx_null_string, 0, NULL, 0, 0, NULL }


struct ngx_open_file_s {
    ngx_fd_t              fd;
    ngx_str_t             name;

    void                (*flush)(ngx_open_file_t *file, ngx_log_t *log);
    void                 *data;
};


typedef struct {
    ngx_file_t            file;
    ngx_buf_t            *buffer;
    ngx_buf_t            *dump;
    ngx_uint_t            line;
} ngx_conf_file_t;


typedef struct {
    ngx_str_t             name;
    ngx_buf_t            *buffer;
} ngx_conf_dump_t;


typedef char *(*ngx_conf_handler_pt)(ngx_conf_t *cf,
    ngx_command_t *dummy, void *conf);

// Nginx 配置解析上下文
struct ngx_conf_s {
    char                 *name;             // 当前解析的配置项名称
    ngx_array_t          *args;             // 当前解析的配置项参数列表，包括名称和值

    ngx_cycle_t          *cycle;            // Nginx运行时 ngx_cycle_t 指针
    ngx_pool_t           *pool;             // 当前配置项的内存池指针
    ngx_pool_t           *temp_pool;        // 当前配置项的临时内存池指针
    ngx_conf_file_t      *conf_file;        // 当前解析配置文件的结构体指针
    ngx_log_t            *log;              // Nginx 日志模块

    void                 *ctx;              // 当前解析模块上下文
    ngx_uint_t            module_type;      // 当前解析的模块类型
    ngx_uint_t            cmd_type;         // 当前解析的命令类型

    ngx_conf_handler_pt   handler;          // 当前解析命令的处理函数
    void                 *handler_conf;     // 当前解析命令处理函数的配置信息
};


typedef char *(*ngx_conf_post_handler_pt) (ngx_conf_t *cf,
    void *data, void *conf);

// 用于为一些配置项设置一个后置处理函数
typedef struct {
    // 指向一个在解析完指定配置项后需要执行的后置处理函数
    ngx_conf_post_handler_pt  post_handler;
} ngx_conf_post_t;


// 用于标记某个配置项已经过时，不再建议使用
typedef struct {
    // 指向一个需要在解析完指定配置项后执行的后置处理函数
    ngx_conf_post_handler_pt  post_handler;
    // 成员变量是一个字符串，代表被标记为过时的配置项的名称
    char                     *old_name;
    // 代表替代被标记为过时的配置项的新名称
    char                     *new_name;
} ngx_conf_deprecated_t;

// 用于表示某个配置项应该是一个整数类型的值，并限定了该整数类型值的取值范围
typedef struct {
    // 指向一个需要在解析完指定配置项后执行的后置处理函数
    ngx_conf_post_handler_pt  post_handler;
    ngx_int_t                 low;      // 代表该整数类型值的最小取值
    ngx_int_t                 high;     // 该整数类型值的最大取值
} ngx_conf_num_bounds_t;


// 用于表示某个配置项应该是一个枚举类型的值
typedef struct {
    ngx_str_t                 name;
    ngx_uint_t                value;
} ngx_conf_enum_t;


#define NGX_CONF_BITMASK_SET  1

// 表示某个配置项应该是一个位掩码类型的值
typedef struct {
    ngx_str_t                 name;
    ngx_uint_t                mask;
} ngx_conf_bitmask_t;



char * ngx_conf_deprecated(ngx_conf_t *cf, void *post, void *data);
char *ngx_conf_check_num_bounds(ngx_conf_t *cf, void *post, void *data);


#define ngx_get_conf(conf_ctx, module)  conf_ctx[module.index]



#define ngx_conf_init_value(conf, default)                                   \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = default;                                                      \
    }

#define ngx_conf_init_ptr_value(conf, default)                               \
    if (conf == NGX_CONF_UNSET_PTR) {                                        \
        conf = default;                                                      \
    }

#define ngx_conf_init_uint_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_UINT) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_init_size_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_SIZE) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_init_msec_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_MSEC) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_merge_value(conf, prev, default)                            \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_ptr_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET_PTR) {                                        \
        conf = (prev == NGX_CONF_UNSET_PTR) ? default : prev;                \
    }

#define ngx_conf_merge_uint_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_UINT) {                                       \
        conf = (prev == NGX_CONF_UNSET_UINT) ? default : prev;               \
    }

#define ngx_conf_merge_msec_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_MSEC) {                                       \
        conf = (prev == NGX_CONF_UNSET_MSEC) ? default : prev;               \
    }

#define ngx_conf_merge_sec_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_size_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_SIZE) {                                       \
        conf = (prev == NGX_CONF_UNSET_SIZE) ? default : prev;               \
    }

#define ngx_conf_merge_off_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_str_value(conf, prev, default)                        \
    if (conf.data == NULL) {                                                 \
        if (prev.data) {                                                     \
            conf.len = prev.len;                                             \
            conf.data = prev.data;                                           \
        } else {                                                             \
            conf.len = sizeof(default) - 1;                                  \
            conf.data = (u_char *) default;                                  \
        }                                                                    \
    }

#define ngx_conf_merge_bufs_value(conf, prev, default_num, default_size)     \
    if (conf.num == 0) {                                                     \
        if (prev.num) {                                                      \
            conf.num = prev.num;                                             \
            conf.size = prev.size;                                           \
        } else {                                                             \
            conf.num = default_num;                                          \
            conf.size = default_size;                                        \
        }                                                                    \
    }

#define ngx_conf_merge_bitmask_value(conf, prev, default)                    \
    if (conf == 0) {                                                         \
        conf = (prev == 0) ? default : prev;                                 \
    }


// 获取配置文件中的下一个参数，返回一个指向参数字符串的指针
// 这个函数在解析配置文件中的指令时使用
char *ngx_conf_param(ngx_conf_t *cf);

// 解析一个配置文件，并将解析后的指令添加到配置上下文中
// filename参数指定要解析的配置文件名
char *ngx_conf_parse(ngx_conf_t *cf, ngx_str_t *filename);

// 解析include指令，并将include文件中的指令添加到配置上下文中
// cmd参数是一个ngx_command_t结构体指针，它包含了当前解析的include指令的信息
char *ngx_conf_include(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


// 将一个相对路径转换为绝对路径，并添加一个前缀
// name参数指定要转换的相对路径，conf_prefix参数指定要添加的前缀
// 这个函数在解析配置文件中的路径时使用
ngx_int_t ngx_conf_full_name(ngx_cycle_t *cycle, ngx_str_t *name,
    ngx_uint_t conf_prefix);
// 打开一个文件，返回一个指向ngx_open_file_t结构体的指针。
// cycle参数是一个ngx_cycle_t结构体指针，它指定了nginx的运行环境。
ngx_open_file_t *ngx_conf_open_file(ngx_cycle_t *cycle, ngx_str_t *name);
// 打印一个错误日志。
// level参数指定日志的级别，
// cf参数是当前的配置上下文，
// err参数是一个错误码，
// fmt参数是一个格式化字符串，
// 后面的可变参数是格式化字符串中使用的变量。
void ngx_cdecl ngx_conf_log_error(ngx_uint_t level, ngx_conf_t *cf,
    ngx_err_t err, const char *fmt, ...);


// 解析一个布尔类型的配置项，并将其值设置到conf指向的内存中
// 函数的返回值是 char * 类型。它用于指示在解析配置文件时是否发生错误。
// 如果返回的指针为 NGX_CONF_OK，则表示解析成功；
// 如果返回的指针为其他值，则表示解析出错，返回的指针指向的字符串将被输出到日志中。
char *ngx_conf_set_flag_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 解析一个字符串类型的配置项，并将其值设置到conf指向的内存中
char *ngx_conf_set_str_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 解析一个字符串数组类型的配置项，并将其值设置到conf指向的内存中
char *ngx_conf_set_str_array_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
// 解析一个键值对类型的配置项，并将其值设置到conf指向的内存中
char *ngx_conf_set_keyval_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 解析一个整数类型的配置项，并将其值设置到conf指向的内存中
char *ngx_conf_set_num_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 这个函数用于处理一个大小类型的配置指令
char *ngx_conf_set_size_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 这个函数用于处理一个偏移量类型的配置指令
char *ngx_conf_set_off_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 这个函数用于处理一个毫秒类型的配置指令。
char *ngx_conf_set_msec_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 这个函数用于处理一个秒类型的配置指令。
char *ngx_conf_set_sec_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 这个函数用于处理一个缓冲区类型的配置指令
char *ngx_conf_set_bufs_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 这个函数用于处理一个枚举类型的配置指令
char *ngx_conf_set_enum_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
// 这个函数用于处理一个掩码类型的配置指令
char *ngx_conf_set_bitmask_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


#endif /* _NGX_CONF_FILE_H_INCLUDED_ */

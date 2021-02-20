#pragma once
#include <string>
#include "memory.h"

const int CHAR_HTTP_VAR_MAX = 256;

struct upload_param
{
    upload_param()
    {
        memset(urltxt, 0, CHAR_HTTP_VAR_MAX);
        memset(dbDir, 0, CHAR_HTTP_VAR_MAX);
        isbatch = 1;
    }
    char urltxt[CHAR_HTTP_VAR_MAX];
    char dbDir[CHAR_HTTP_VAR_MAX];
    int isbatch;
};

struct query_param
{
    query_param()
    {
        memset(inputType, 0, CHAR_HTTP_VAR_MAX);
        memset(input, 0, CHAR_HTTP_VAR_MAX);
        memset(topN, 0, CHAR_HTTP_VAR_MAX);
    }

    char inputType[CHAR_HTTP_VAR_MAX];
    char input[CHAR_HTTP_VAR_MAX];
    char topN[CHAR_HTTP_VAR_MAX];
};

//restful api 接口处理基类
class CHttpHandler
{
public:
    virtual ~CHttpHandler() {};
public:
    /*
   *功能：预处理解析接口参数并返回
   *输入：
    c  当前连接session信息
    ev 
    ev_data 当前请求数据
    fn_data 当前连接的额外参数(包括对应连接参数，服务器参数)
   *输出：void * 接口参数指针
   */
    virtual void* http_handler_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data) = 0;

    /*
   *功能：实际处理接口业务逻辑处理
   *输入：
    c  当前连接session信息
    ev
    ev_data 当前请求数据
    fn_data 当前连接的额外参数(包括对应连接参数，服务器参数)
   *输出：int 成功处理返回0 失败返回<0
   */
    virtual int http_handler(void* requestparam, void* fn_data, std::string& response) = 0;
};

class CHttpHandlerUpload : public CHttpHandler
{
public:
    virtual void* http_handler_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
    virtual int http_handler(void* requestparam, void* fn_data, std::string& response);
};

class CHttpHandlerQuery : public CHttpHandler
{
public:
    virtual void* http_handler_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
    virtual int http_handler(void* requestparam, void* fn_data, std::string& response);
};

typedef CHttpHandler* PHttpHandler;

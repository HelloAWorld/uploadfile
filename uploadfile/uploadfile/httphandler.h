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

//restful api �ӿڴ������
class CHttpHandler
{
public:
    virtual ~CHttpHandler() {};
public:
    /*
   *���ܣ�Ԥ��������ӿڲ���������
   *���룺
    c  ��ǰ����session��Ϣ
    ev 
    ev_data ��ǰ��������
    fn_data ��ǰ���ӵĶ������(������Ӧ���Ӳ���������������)
   *�����void * �ӿڲ���ָ��
   */
    virtual void* http_handler_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data) = 0;

    /*
   *���ܣ�ʵ�ʴ���ӿ�ҵ���߼�����
   *���룺
    c  ��ǰ����session��Ϣ
    ev
    ev_data ��ǰ��������
    fn_data ��ǰ���ӵĶ������(������Ӧ���Ӳ���������������)
   *�����int �ɹ�������0 ʧ�ܷ���<0
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

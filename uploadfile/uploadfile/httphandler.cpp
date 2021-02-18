#include "httphandler.h"
#include "stdio.h"
#include <iostream>
#include<vector>
#include<string>
#include "Module.h"
#include "mongoose.h"
#include <fstream>

/*
 * API方式的两个接口, callType区分两个链接：
 *(1)传入callType=1, urltxt文件,载入图库
 *(2)传入callType=2, ImageType,载入图库
*/



class CMessageHead
{
public:
    CMessageHead();
    ~CMessageHead();

    int GetMsgLength() const;
    int GetDataLen() const;
    const char* GetData() const;
    void Encode(int* iOutDataLen, char** pOutData);
    int GetEncodeDataLenght() const;

    void SetMsgLength(int iLength);
    void SetData(int iDataLen, const char* pData);
 
    int Decode(int iDataLen, const char* pData);
private:
    int m_msglength;
    int m_datalen;
    char* m_pData;
};


CMessageHead::CMessageHead()
{
    m_msglength = 0;
    m_datalen = 0;
    m_pData = nullptr;
}

CMessageHead::~CMessageHead()
{
    if (nullptr != m_pData)
    {
        //delete [] m_pData;
    }
}

int CMessageHead::GetMsgLength() const
{
    return m_msglength;
}

int CMessageHead::GetDataLen() const
{
    return m_datalen;
}

const char* CMessageHead::GetData() const
{
    return m_pData;
}

void CMessageHead::Encode(int* iOutDataLen, char** pOutData)
{
    int len = 0;
    int iEncodedLen = 0;
    *iOutDataLen = GetEncodeDataLenght();
    char* pTmpData = new char[*iOutDataLen];
   
    len = sizeof(m_msglength);
    memcpy(pTmpData + iEncodedLen, &m_msglength, len);
    iEncodedLen += len;

    len = sizeof(m_datalen);
    memcpy(pTmpData + iEncodedLen, &m_datalen, len);
    iEncodedLen += len;

    memcpy(pTmpData + iEncodedLen, m_pData, m_datalen);
    iEncodedLen += m_datalen;

    *pOutData = pTmpData;
}

int CMessageHead::GetEncodeDataLenght() const
{
    int iEncodeLen = 0;  
    iEncodeLen += sizeof(m_msglength);
    iEncodeLen += sizeof(m_datalen);
    iEncodeLen += m_datalen;
    return iEncodeLen;
}

void CMessageHead::SetMsgLength(int iLength)
{
    m_msglength = iLength;
}

void CMessageHead::SetData(int iDataLen, const char* pData)
{
    m_datalen = iDataLen;
    if (m_pData != nullptr)
    {
        delete[] m_pData;
        m_pData = nullptr;
    }
    m_pData = new char[m_datalen];
    memcpy(m_pData, pData, m_datalen);
}

int CMessageHead::Decode(int iDataLen, const char* pData)
{
    int len = 0;
    int iDecodedLen = 0;
    
    len = sizeof(m_msglength);
    if (iDataLen < iDecodedLen + len)
    {
        return -1;
    }
    memcpy(&m_msglength, pData + iDecodedLen, len);
    iDecodedLen += len;

    len = sizeof(m_datalen);
    if (iDataLen < iDecodedLen + len)
    {
        return -1;
    }
    memcpy(&m_datalen, pData + iDecodedLen, len);
    iDecodedLen += len;

    if (nullptr != m_pData)
    {
        delete[] m_pData;
    }
    len = m_datalen;
    if (iDataLen < iDecodedLen + len)
    {
        return -1;
    }
    m_pData = new char[len];
    memcpy(m_pData, pData + iDecodedLen, len);
    iDecodedLen += len;

    return iDecodedLen;
}


std::vector<std::string> getUrlsFromTxt(std::string txt);
std::string searchResult2str(std::vector<std::string> res);


void handler_accept(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
void http_handler_pair_request(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
void http_ahndler_pair_response(struct mg_connection* c, int ev, void* ev_data, void* fn_data);

void * handler_upload_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_datak);
void * handler_query_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data);

int handler_upload(void* requestparam, void* fn_data, std::string& response);
int handler_query(void* requestparam, void* fn_data, std::string& response);


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

typedef int (*p_handler_func)(void* requestparam, void* fn_data, std::string& response);

struct thread_data {
    struct mg_connection* c;
    int ev;
    void* ev_data;
    void* fn_data;
    int blocking;
    void* param;
    p_handler_func p_func;
};


#ifndef SLEEP_TIME
#define SLEEP_TIME 3  // Seconds to sleep to simulate calculation
#endif

static void start_thread(void (*f)(void*), void* p) {
#ifdef _WIN32
    _beginthread((void(__cdecl*)(void*)) f, 0, p);
#else
#define closesocket(x) close(x)
#include <pthread.h>
    pthread_t thread_id = (pthread_t)0;
    pthread_attr_t attr;
    (void)pthread_attr_init(&attr);
    (void)pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &attr, (void* (*) (void*)) f, p);
    pthread_attr_destroy(&attr);
#endif
}

static void thread_function(void* param) {
    struct thread_data* pthreaddata = (struct thread_data*)param;
    int sock = (long)pthreaddata->blocking;                     // Grab our blocking socket
   
    LOG(LL_INFO, ("got sock %d", sock));     
  
    std::string response;
    pthreaddata->p_func(pthreaddata->param, pthreaddata->fn_data, response);
   
    CMessageHead msg;
    msg.SetData(response.length(), response.c_str());

    char* pData = NULL;
    int iMsgLen = 0;
    msg.Encode(&iMsgLen, &pData);
    send(sock, pData, iMsgLen, 0);

    closesocket(sock);                           // Done, close socket, end thread
}

void http_server_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    if (ev == MG_EV_ACCEPT && mg_url_is_ssl(s_listen_on))
    {
        handler_accept(c, ev, ev_data, fn_data);
    }
    else if (ev == MG_EV_HTTP_MSG) {
        http_handler_pair_request(c, ev, ev_data, fn_data);
    }
    else if (ev == MG_EV_POLL && c->fn_data != NULL) {
        http_ahndler_pair_response(c, ev, ev_data, fn_data);
    }
}

void http_handler_pair_request(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    // Incoming request. Create socket pair.
    // Pass blocking socket to the thread, and keep the non-blocking socket.
    struct mg_http_message* hm = (struct mg_http_message*)ev_data;
    void* param = nullptr;
    p_handler_func p_func = nullptr;
    std::cout << "111.http_server_handler  uri:" << std::string(hm->uri.ptr, hm->uri.len) << " query:" << std::string(hm->query.ptr, hm->query.len) << std::endl;
    if (mg_http_match_uri(hm, "/CBIR/upload"))
    {
        //std::cout << "222.handler_upload  uri:" << std::string(hm->uri.ptr, hm->uri.len) << " query:" << std::string(hm->query.ptr, hm->uri.len) << std::endl;
        param = handler_upload_pre_parse_param(c, ev, ev_data, fn_data);
        p_func = handler_upload;
    }
    else if (mg_http_match_uri(hm, "/CBIR/query"))
    {
        //std::cout << "333.handler_query  uri:" << std::string(hm->uri.ptr, hm->uri.len) << " query:" << std::string(hm->query.ptr, hm->uri.len) << std::endl;
        param = handler_query_pre_parse_param(c, ev, ev_data, fn_data);
        p_func = handler_query;
    }
    else
    {
        std::string response = "{\"code\":-1,\"msg\":\"url not exist\"}";
        mg_http_reply(c, 200, "Content-Type:application/json; charset=utf-8\r\n", response.c_str());  // Serve REST
        return;
    }
    
    _fn_data* pfndata = (_fn_data*)fn_data;
    if (pfndata != nullptr && !pfndata->bmultithreading)
    {
        std::string resposne;
        p_func(param, fn_data, resposne);
        mg_http_reply(c, 200, "Content-Type:application/json; charset=utf-8\r\n", resposne.c_str());  // Serve REST
        return;
    }

    int blocking = -1, non_blocking = -1;

    struct thread_data* pthreadata = new thread_data();

    mg_socketpair(&blocking, &non_blocking);  // Create connected pair
    pthreadata->blocking = blocking;
    pthreadata->c = c;
    pthreadata->ev = ev;
    pthreadata->ev_data = ev_data;
    pthreadata->fn_data = fn_data;
    pthreadata->param = param;
    pthreadata->p_func = p_func;
    // Pass blocking socket to the thread_function.
    start_thread(thread_function, (void*)pthreadata);
    std::cout << "MG_EV_POLL 0000000000 blocking=" << blocking << " non_blocking=" << non_blocking << std::endl;

    // Non-blocking is ours.   Store it in the fn_data, in
    // order to use it in the subsequent invocations
    _fn_data* new_fn_data = new _fn_data();
    new_fn_data->blocking = non_blocking;
    new_fn_data->bmultithreading = ((_fn_data*)fn_data)->bmultithreading;
    new_fn_data->machine = ((_fn_data*)fn_data)->machine;
    new_fn_data->origin_fn_data = fn_data;
    c->fn_data = (void *)new_fn_data;
        
}

void handler_accept(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    // If s_listen_on URL is https://, tell listening connection to use TLS
    struct mg_tls_opts opts;

    //opts.ca = "ca.pem",         // Uncomment to enable two-way SSL
    opts.cert = "server.pem";     // Certificate PEM file
    opts.certkey = "server.pem";  // This pem conains both cert and key

    mg_tls_init(c, &opts);
}

void * handler_upload_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    struct mg_http_message* hm = (struct mg_http_message*)ev_data;
    upload_param* param = new upload_param();
    int ul = mg_http_get_var(&hm->query, "urltxt", param->urltxt, sizeof(param->urltxt));
    int pl = mg_http_get_var(&hm->query, "dbDir", param->dbDir, sizeof(param->dbDir));
    char batch[MG_MAX_HTTP_HEADERS] = { 0 };
    int bl = mg_http_get_var(&hm->query, "batch", batch, sizeof(batch));
    if (strlen(batch) > 0)
    {
        param->isbatch = atoi(batch);
    }
    else
    {
        param->isbatch = 1;
    }

    std::cout << "parse.handler_upload_pre_parse_param url:  urltxt:" << param->urltxt << " dbDir:" << param->dbDir << " isbatch:" << param->isbatch  << std::endl;

    return param;
}

void * handler_query_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    struct mg_http_message* hm = (struct mg_http_message*)ev_data;
    struct query_param* param = new query_param();
    int ul = mg_http_get_var(&hm->query, "inputType", param->inputType, sizeof(param->inputType));
    int pl = mg_http_get_var(&hm->query, "input", param->input, sizeof(param->input));
    int tl = mg_http_get_var(&hm->query, "topN", param->topN, sizeof(param->topN));

    std::cout << "parse.handler_query_pre_parse_param url:  inputType:" << param->inputType << " input:" << param->input << " topN:" << param->topN << std::endl;

    return param;
}

int handler_upload(void* requestparam, void* fn_data, std::string& response)
{
    struct upload_param* param = (struct upload_param*)requestparam;

    std::vector<std::string> urls = getUrlsFromTxt(param->urltxt);
   
    std::cout << "exec handler_upload url:  urltxt:" << param->urltxt << " dbDir:" << param->dbDir << std::endl;
    do {
        if (urls.size() < 1)
        {
            response =  "{\"code\":0,\"msg\":\"urls.size() < 1 !!\"}";
            break;
        }       

        //添加图像至图库
        try {
            IModule* machine = (IModule*)(((struct _fn_data*)fn_data)->machine);
            int ret = 0;
            
            if (param->isbatch)
            {
                ret = machine->addUrlToDB(urls, param->dbDir);
                if (ret >= 0) 
                {
                    response = "{\"code\":0,\"msg\":\"success add " + std::to_string(ret) + " url to db !!\"}";
                    break;
                }
                else 
                {
                    response = "{\"code\":-1,\"msg\":\"Failed in add url to db !! retry it !! retcode: " + std::to_string(ret) + "\"}";
                    break;
                }
            }
            else
            {
                for (int index = 0; index < urls.size(); index++)
                {

                    ret = machine->addAUrlToDB(urls[index], param->dbDir);
                    if (ret >= 0) 
                    {
                        response = "{\"code\":0,\"msg\":\"success add " + std::to_string(ret) + " url to db !!\"}";
                        break;
                    }
                    else
                    {
                        response = "{\"code\":-1,\"msg\":\"Failed in add url to db !! retry it !! retcode: " + std::to_string(ret) + "\"}";
                        break;
                    }
                }
            }
        }
        catch (std::exception e)
        {
            std::cout << "444.throw exception " << e.what() << std::endl;
            response = "{\"code\":-1,\"msg\":\"Failed in add url to db !! retry it !! throw exception ";
            response += e.what();
            response += "\"}";
        }
    } while (false);

    std::cout << "555.handler_upload url:  urltxt:" << param->urltxt << " dbDir:" << param->dbDir << " response:"<< response << std::endl;
}

int handler_query(void* requestparam, void * fn_data, std::string& response)
{
    struct query_param* param = (struct query_param*)requestparam;
    std::vector<std::string> searchResult;
    
    IModule* machine = (IModule*)(((struct _fn_data*)fn_data)->machine);

    std::cout << "exec.handler_query url:  inputType:" << param->inputType << " input:" << param->input << " topN:" << param->topN << std::endl;

    do {

        try {
            int ret = machine->search(param->input, atoi(param->inputType), searchResult, atoi(param->topN));
            if (ret >= 1) {
                response = "{\"code\":0,\"result:\n " + searchResult2str(searchResult) + "\"}";
                break;
            }
            else {
                response = "{\"code\":-1,\"msg\":\"Failed search !! retcode: " + std::to_string(ret) + "\"}";
                break;
            }
        }
        catch (std::exception e)
        {
            std::cout << "666.throw exception " << e.what() << std::endl;
            response = "{\"code\":-1,\"msg\":\"Failed search !! retry it !! throw exception ";
            response += e.what();
            response += "\"}";
        }

    } while (false);

    std::cout << "777.handler_query url:  inputType:" << param->inputType << " input:" << param->input << " topN:" << param->topN << " response:" << response << std::endl;
}


void http_ahndler_pair_response(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    // On each poll iteration, try to receive response data
    int sock = ((_fn_data*)(c->fn_data))->blocking;

    int iRet = 0;
    std::string strRecvData;

    do
    {
        char szBuf[1024] = { 0 };

        iRet = recv(sock, szBuf, 1024, 0);
        if (0 == iRet)
        {
            std::cout << "recv 0 break while" << std::endl;
            break;
        }
        if (iRet < 0)
        {
#ifdef _WIN32
            int err = WSAGetLastError();
            if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN)
            {
                continue;
            }
#else
            int err = errno;
            if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN)
            {
                continue;
            }
#endif    
            break;
        }
        strRecvData.append(szBuf, iRet);

        while (strRecvData.length() > 0) {
            CMessageHead msg;
            int ilen = msg.GetEncodeDataLenght();
            if (strRecvData.length() < ilen)
            {
                break;
            }
            int iDecoededLen = msg.Decode(strRecvData.length(), strRecvData.c_str());
            if (iDecoededLen < 0)
            {
                break;
            }
            strRecvData = strRecvData.substr(iDecoededLen, strRecvData.length() - iDecoededLen);
            std::cout << "MG_EV_POLL 1111111111 len=" << msg.GetDataLen() << std::endl;
            // Yeah! Got the response.
            mg_http_reply(c, 200, "Content-Type:application/json; charset=utf-8\r\n", msg.GetData());  // Serve REST
            std::cout << "MG_EV_POLL len:" << msg.GetDataLen() << " len:" << msg.GetDataLen() << " data" << msg.GetData() << std::endl;
            closesocket(sock);    // And close our end of the socket pair
            
            _fn_data* new_fn_data = ((_fn_data*)(c->fn_data));
            //if(new_fn_data != nullptr)
            c->fn_data = new_fn_data->origin_fn_data;
            delete new_fn_data;
            break;
        }
        if (iRet < 0)
        {
            break;
        }
    } while (iRet > 0);
}

/*
 * API方式的两个接口, callType区分两个链接：
 *(1)传入callType=1, urltxt文件,载入图库
 *(2)传入callType=2, ImageType,载入图库
*/

std::vector<std::string> getUrlsFromTxt(std::string txt)
{
    std::ifstream read_file;
    std::vector<std::string> vecUrls;

    read_file.open(txt, std::ios::binary);

    std::string line;
    while (std::getline(read_file, line))
    {
        std::cout << "line:" << line.c_str() << std::endl;
        vecUrls.push_back(line);
    }
    return vecUrls;
}

std::string searchResult2str(std::vector<std::string> res)
{
    std::string resStr = "|";
    for (int i = 0; i < res.size(); i++)
    {
        resStr += res[i] + "|";
    }
    return resStr;
}


#include <iostream>

#include "HttpServer.h"
#include "httphandler.h"
#include "MessageHead.h"
#include "ThreadPool.h"

struct thread_data {
    struct mg_connection* c;
    int ev;
    void* ev_data;
    void* fn_data;
    int blocking;
    void* param;
    PHttpHandler pHttpHandler;
};

CHttpServer::CHttpServer()
{
}

CHttpServer::~CHttpServer()
{
}

CHttpServer* CHttpServer::Instance()
{
    static CHttpServer server;
    return &server;
}

int CHttpServer::InitServer(_ServerParam* pfn_data)
{    
    mg_log_set("3");                              // Set to 3 to enable debug
    mg_mgr_init(&m_mgr);                            // Initialise event manager
    mg_http_listen(&m_mgr, s_listen_on, &CHttpServer::http_server_handler, pfn_data);  // Create HTTP listener

    return 0;
}


void CHttpServer::StartServerLoop()
{
    for (;;) mg_mgr_poll(&m_mgr, 10);             // Infinite event loop
}

void CHttpServer::StopServer()
{
    mg_mgr_free(&m_mgr);
}

void CHttpServer::RegisterHandler(const std::string& path, PHttpHandler pHttpHandler)
{
    m_mapRouter.insert(std::pair<std::string, PHttpHandler>(path, pHttpHandler));
}


void CHttpServer::sync_http_server_handler(void* param) {
    LOG(LL_INFO, ("thread_http_server_handler "));

    struct thread_data* pthreaddata = (struct thread_data*)param;
    int sock = (long)pthreaddata->blocking;                     // Grab our blocking socket

    LOG(LL_INFO, ("got sock %d", sock));

    std::string response;
    pthreaddata->pHttpHandler->http_handler(pthreaddata->param, pthreaddata->fn_data, response);

    CMessageHead msg;
    msg.SetData(response.length(), response.c_str());

    char* pData = NULL;
    int iMsgLen = 0;
    msg.Encode(&iMsgLen, &pData);
    send(sock, pData, iMsgLen, 0);

    closesocket(sock);                           // Done, close socket, end thread

    delete pthreaddata->param;

    delete pthreaddata;
}

void CHttpServer::http_server_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    if (ev == MG_EV_ACCEPT && mg_url_is_ssl(s_listen_on))
    {
        handler_accept(c, ev, ev_data, fn_data);
    }
    else if (ev == MG_EV_HTTP_MSG) {
        http_handler_pair_request(c, ev, ev_data, fn_data);
    }
    else if (ev == MG_EV_POLL && c->fn_data != NULL) {
        http_handler_pair_response(c, ev, ev_data, fn_data);
    }
}


void CHttpServer::http_handler_pair_request(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    // Incoming request. Create socket pair.
    // Pass blocking socket to the thread, and keep the non-blocking socket.
    struct mg_http_message* hm = (struct mg_http_message*)ev_data;
    void* param = nullptr;
 
    LOG(LL_INFO, ("111.http_server_handler  uri:%s query:%s",std::string(hm->uri.ptr, hm->uri.len).c_str() ,std::string(hm->query.ptr, hm->query.len).c_str()));

    PHttpHandler phttphandler;
    CHttpServer* pServer = CHttpServer::Instance();

    std::map<std::string, PHttpHandler>::iterator it;
    for (it = pServer->m_mapRouter.begin(); it != pServer->m_mapRouter.end(); it++)
    {
        if (mg_http_match_uri(hm, it->first.c_str()))
        {
            break;
        }
    }
    if (it != pServer->m_mapRouter.end())
    {
        phttphandler = it->second;
        param = phttphandler->http_handler_pre_parse_param(c, ev, ev_data, fn_data);
    }
    else
    {
        std::string response = "{\"code\":-1,\"msg\":\"url not exist\"}";
        mg_http_reply(c, 200, "Content-Type:application/json; charset=utf-8\r\n", response.c_str());  // Serve REST
        return;
    }

    _ServerParam* pfndata = (_ServerParam*)fn_data;
    if (pfndata != nullptr && !pfndata->bmultithreading)
    {
        std::string resposne;
        phttphandler->http_handler(param, fn_data, resposne);
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
    pthreadata->pHttpHandler = phttphandler;
    // Pass blocking socket to the thread_function.
    //start_thread(thread_function, (void*)pthreadata);
    CThreadPool::Instance()->AddTask(pthreadata);

    LOG(LL_INFO, ("MG_EV_POLL 0000000000 blocking=%d non_blocking%d", blocking, non_blocking));

    // Non-blocking is ours.   Store it in the fn_data, in
    // order to use it in the subsequent invocations
    _ServerParam* new_fn_data = new _ServerParam();
    new_fn_data->blocking = non_blocking;
    new_fn_data->bmultithreading = ((_ServerParam*)fn_data)->bmultithreading;
    new_fn_data->machine = ((_ServerParam*)fn_data)->machine;
    new_fn_data->origin_fn_data = fn_data;
    c->fn_data = (void*)new_fn_data;

}

void CHttpServer::handler_accept(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    // If s_listen_on URL is https://, tell listening connection to use TLS
    struct mg_tls_opts opts;

    //opts.ca = "ca.pem",         // Uncomment to enable two-way SSL
    opts.cert = "server.pem";     // Certificate PEM file
    opts.certkey = "server.pem";  // This pem conains both cert and key

    mg_tls_init(c, &opts);
}

void CHttpServer::http_handler_pair_response(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    // On each poll iteration, try to receive response data
    int sock = ((_ServerParam*)(c->fn_data))->blocking;

    int iRet = 0;
    std::string strRecvData;

    do
    {
        char szBuf[1024] = { 0 };

        iRet = recv(sock, szBuf, 1024, 0);
        if (0 == iRet)
        {
            LOG(LL_INFO, ("recv 0 break while"));
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
            LOG(LL_INFO, ("MG_EV_POLL 1111111111 len=%d", msg.GetDataLen()));
            // Yeah! Got the response.
            mg_http_reply(c, 200, "Content-Type:application/json; charset=utf-8\r\n", std::string(msg.GetData(), msg.GetDataLen()).c_str());  // Serve REST
            LOG(LL_INFO, ("MG_EV_POLL len:%d len:%d data:%s", msg.GetDataLen(), msg.GetDataLen(), std::string(msg.GetData(), msg.GetDataLen()).c_str()));
            closesocket(sock);    // And close our end of the socket pair

            _ServerParam* new_fn_data = ((_ServerParam*)(c->fn_data));
            //if(new_fn_data != nullptr)
            c->fn_data = new_fn_data->origin_fn_data;
            //delete new_fn_data;
            break;
        }
        if (iRet < 0)
        {
            break;
        }
    } while (iRet > 0);
}
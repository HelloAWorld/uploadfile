#include "Module.h"
#include "fstream"
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include "mongoose.h"
#include "httphandler.h"
#include "ThreadPool.h"
#include "HttpServer.h"

using namespace std;

IModule* machine;

typedef IModule* (*PInitialModule)(std::string featdbfile);

int main(int argc, char** argv)
{
    LOG(LL_INFO, ("start localhost....."));
    
    //初始化底层库
    machine = InitialModule(""); 

    //初始化http server
    _ServerParam* pfn_data = new _ServerParam();
    pfn_data->machine = machine;
    pfn_data->bmultithreading = true;   
    CHttpServer::Instance()->InitServer(pfn_data);

    //注册restful api接口处理器
    CHttpServer::Instance()->RegisterHandler("/CBIR/upload", new CHttpHandlerUpload());
    CHttpServer::Instance()->RegisterHandler("/CBIR/query", new CHttpHandlerQuery());

    //开启多线程线程池
    CThreadPool::Instance()->StartPool(10, CHttpServer::sync_http_server_handler);

    //开启httpserver 事务处理循环
    CHttpServer::Instance()->StartServerLoop();
    
    //停止线程池
    CThreadPool::Instance()->StopPool();

    //停止httpserver
    CHttpServer::Instance()->StopServer();

    return 0;
}
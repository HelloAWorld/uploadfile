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
    
    //��ʼ���ײ��
    machine = InitialModule(""); 

    //��ʼ��http server
    _ServerParam* pfn_data = new _ServerParam();
    pfn_data->machine = machine;
    pfn_data->bmultithreading = true;   
    CHttpServer::Instance()->InitServer(pfn_data);

    //ע��restful api�ӿڴ�����
    CHttpServer::Instance()->RegisterHandler("/CBIR/upload", new CHttpHandlerUpload());
    CHttpServer::Instance()->RegisterHandler("/CBIR/query", new CHttpHandlerQuery());

    //�������߳��̳߳�
    CThreadPool::Instance()->StartPool(10, CHttpServer::sync_http_server_handler);

    //����httpserver ������ѭ��
    CHttpServer::Instance()->StartServerLoop();
    
    //ֹͣ�̳߳�
    CThreadPool::Instance()->StopPool();

    //ֹͣhttpserver
    CHttpServer::Instance()->StopServer();

    return 0;
}
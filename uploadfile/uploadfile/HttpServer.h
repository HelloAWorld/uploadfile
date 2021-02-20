#pragma once
#include <map>
#include "mongoose.h"
#include "httphandler.h"


#ifdef _WIN32
#else
#define closesocket(x) close(x)
#include <pthread.h>
#endif

//http����������,����api�ӿڵ�ȫ�ֲ���
struct _ServerParam
{
	_ServerParam()
	{
		machine = nullptr;
		bmultithreading = true;
		blocking = 0;
		origin_fn_data = nullptr;
	}

	int blocking;           //�첽����sock
	void* machine;          //ҵ����Module�ӿ�
	bool bmultithreading;   //ʹ�ö��̻߳��ǵ��߳�
	void* origin_fn_data;   //���ڱ�����Ϣ��Ӧ����
};

static const char* s_listen_on = "http://0.0.0.0:8000";
static const char* s_web_directory = ".";


//http��������
class CHttpServer
{
private://����
	CHttpServer();

public:
	~CHttpServer();

	//��̬������
	static CHttpServer* Instance();

	int InitServer(_ServerParam* pfn_data);

	void StartServerLoop();

	void StopServer();

	void RegisterHandler(const std::string& path, PHttpHandler pHttpHandler);
public:
	static void http_server_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
	static void sync_http_server_handler(void* param);


	static void handler_accept(struct mg_connection* c, int ev, void* ev_data, void* fn_data);

	static void http_handler_pair_request(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
	static void http_handler_pair_response(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
private:
	struct mg_mgr m_mgr;                            // Event manager
	std::map<std::string, PHttpHandler> m_mapRouter;
};


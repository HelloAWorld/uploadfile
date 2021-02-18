#pragma once
//#include "Module.h"

struct _fn_data
{
    _fn_data()
    {
        machine = nullptr;
        bmultithreading = true;    
        blocking = 0;
        origin_fn_data = nullptr;
    }

    int blocking;
    void* machine;
    bool bmultithreading;
    void* origin_fn_data;
};

static const char* s_listen_on = "http://0.0.0.0:8000";
static const char* s_web_directory = ".";
static _fn_data s_fn_data;

void http_server_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
void thread_http_server_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data, int sock = 0);
//void thread_http_server_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data);

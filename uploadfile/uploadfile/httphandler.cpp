#include "httphandler.h"
#include "stdio.h"
#include <iostream>
#include<vector>
#include<string>
#include "Module.h"
#include "mongoose.h"
#include <fstream>
#include "MessageHead.h"
#include "ThreadPool.h"
#include "HttpServer.h"


std::vector<std::string> getUrlsFromTxt(const std::string & txt);
std::string searchResult2str(std::vector<std::string>  res);


void* CHttpHandlerUpload::http_handler_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
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

    LOG(LL_INFO, ("CHttpHandlerUpload::http_handler_pre_parse_param urltxt:%s dbDir:%s isbatc:%d", param->urltxt, param->dbDir, param->isbatch));

    return param;
}

void* CHttpHandlerQuery::http_handler_pre_parse_param(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    struct mg_http_message* hm = (struct mg_http_message*)ev_data;
    struct query_param* param = new query_param();
    int ul = mg_http_get_var(&hm->query, "inputType", param->inputType, sizeof(param->inputType));
    int pl = mg_http_get_var(&hm->query, "input", param->input, sizeof(param->input));
    int tl = mg_http_get_var(&hm->query, "topN", param->topN, sizeof(param->topN));

    LOG(LL_INFO, ("CHttpHandlerQuery::http_handler_pre_parse_param inputType:%d input:%s topN:%d", param->inputType, param->input, param->topN));

    return param;
}

int CHttpHandlerUpload::http_handler(void* requestparam, void* fn_data, std::string& response)
{
    int result = 0;
    struct upload_param* param = (struct upload_param*)requestparam;

    std::vector<std::string> urls = getUrlsFromTxt(param->urltxt);
   
    LOG(LL_INFO, ("CHttpHandlerUpload::http_handler urltxt:%s dbDir:%s urls-count:%d", param->urltxt, param->dbDir, urls.size()));
    do {
        if (urls.size() < 1)
        {
            response =  "{\"code\":0,\"msg\":\"urls.size() < 1 !!\"}";
            result = -1;
            break;
        }       

        //Ìí¼ÓÍ¼ÏñÖÁÍ¼¿â
        try {
            IModule* machine = (IModule*)(((struct _ServerParam*)fn_data)->machine);
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
                    result = -2;
                    response = "{\"code\":-1,\"msg\":\"Failed in add url to db !! retry it !! retcode: " + std::to_string(ret) + "\"}";
                    break;
                }
            }
            else
            {
                for (int index = 0; index < urls.size(); index++)
                {
                    //std::string url = urls[index];
                    //std::string key = param->dbDir + url.substr(url.find_last_of("/") + 1);
                    //std::cout << "url:" << urls[index] << " key:" << key << std::endl;
                    ret = machine->addAUrlToDB(urls[index], param->dbDir);
                    if (ret >= 0) 
                    {
                        response = "{\"code\":0,\"msg\":\"success add " + std::to_string(ret) + " url to db !!\"}";
                        break;
                    }
                    else
                    {
                        result = -2;
                        response = "{\"code\":-1,\"msg\":\"Failed in add url to db !! retry it !! retcode: " + std::to_string(ret) + "\"}";
                        break;
                    }
                }
            }
        }
        catch (std::exception e)
        {
            LOG(LL_INFO, ("CHttpHandlerUpload::http_handler throw exception %s", e.what()));
            response = "{\"code\":-1,\"msg\":\"Failed in add url to db !! retry it !! throw exception ";
            response += e.what();
            response += "\"}";
            result = -3;
        }
    } while (false);

    LOG(LL_INFO, ("CHttpHandlerUpload::http_handler urltxt:%s dbDir:%s response:%s", param->urltxt, param->dbDir, response.c_str()));
    return result;
}

int CHttpHandlerQuery::http_handler(void* requestparam, void * fn_data, std::string& response)
{
    int result = 0;
    struct query_param* param = (struct query_param*)requestparam;
    std::vector<std::string> searchResult;
    
    IModule* machine = (IModule*)(((struct _ServerParam*)fn_data)->machine);

    LOG(LL_INFO, ("CHttpHandlerQuery::http_handler inputType:%d input:%s topN:%d", param->inputType, param->input, param->topN));

    do {

        try {
            int ret = machine->search(param->input, atoi(param->inputType), searchResult, atoi(param->topN));
            if (ret >= 1) {
                if (searchResult.size() > 0)
                {
                    response = "{\"code\":0,\"result\":\"" + searchResult2str(searchResult) + "\"}";
                }
                else
                {
                    response = "{\"code\":0,\"result\":\"\"}";
                }
                break;
            }
            else {
                result = -1;
                response = "{\"code\":-1,\"msg\":\"Failed search !! retcode: " + std::to_string(ret) + "\"}";
                break;
            }
        }
        catch (std::exception e)
        {
            LOG(LL_INFO, ("CHttpHandlerQuery::http_handler throw exception %s", e.what()));;
            response = "{\"code\":-1,\"msg\":\"Failed search !! retry it !! throw exception ";
            response += e.what();
            response += "\"}";
            result = -2;
        }

    } while (false);

    LOG(LL_INFO, ("CHttpHandlerQuery::http_handler url:  inputType:%d input:%s topN:%d response:%s", param->inputType, param->input, param->topN, response.c_str()));
    return result;
}



std::vector<std::string> getUrlsFromTxt(const std::string & txt)
{
    std::ifstream read_file;
    std::vector<std::string> vecUrls;

    read_file.open(txt, std::ios::binary);

    std::string line;
    while (std::getline(read_file, line))
    {
        LOG(LL_INFO, ("getUrlsFromTxt line:%s", line.c_str()));
        vecUrls.push_back(line);
    }
    return vecUrls;
}

std::string searchResult2str(std::vector<std::string>  res)
{
    std::string resStr = "|";
    for (int i = 0; i < res.size(); i++)
    {
        resStr += res[i] + "|";
    }
    return resStr;
}


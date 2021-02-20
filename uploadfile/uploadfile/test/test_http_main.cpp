#include "Module.h"
#include "fstream"
#include "httplib.h"
#include "Easylog.h"

using namespace std;
using namespace httplib;

/*
 * API方式的两个接口, callType区分两个链接：
 *(1)传入callType=1, urltxt文件,载入图库
 *(2)传入callType=2, ImageType,载入图库
*/

vector<string> getUrlsFromTxt(string txt)
{
    return vector<string>(0);
}

string searchResult2str(vector<string> res)
{
    string resStr = "|";
    for(int i=0; i<res.size(); i++)
    {
        resStr += res[i]+"|";
    }
    return resStr;
}

IModule *machine;

int main(int argc, char** argv)
{
    //初始化
    machine = InitialModule("");
    ELOG_INFO("init network succeed !!\n");

    Server svr;
    svr.Get("/CBIR",[](const Request& req, Response& res){
    //localhost:1234/CBIR?callType=D:/ttt/new-Gk.cfg&imgPath=20210126101006.jpg&savePath=1.jpg&useNumRecognize=0

        string retCode = "";

        int callType = 0;
        if (req.has_param("callType")) {
            callType = atoi(req.get_param_value("callType").c_str());
            cout<<"callType = "<<callType<<endl;
        }
        ELOG_INFO("callType=%d\n" , callType);

        if(callType == 1){
            //(1)载入图像接口
            string urltxt = "";
            if (req.has_param("urltxt")) {
                urltxt = req.get_param_value("urltxt");
                cout<<"urltxt = "<<urltxt<<endl;
            }
            ELOG_INFO("urltxt=%s\n" , urltxt.c_str());

            vector<string> urls = getUrlsFromTxt(urltxt);
            if(urls.size()<1)
            {
                retCode = "urls.size() < 1 !!";
                res.set_content(retCode, "text/plain");
                return;
            }

            string dbDir = "";
            if (req.has_param("dbDir")) {
                dbDir = req.get_param_value("dbDir");
                cout<<"dbDir = "<<dbDir<<endl;
            }
            ELOG_INFO("dbDir=%s\n" , dbDir.c_str());

            //添加图像至图库
            int ret1 = machine->addUrlToDB(urls,dbDir);
            if(ret1>=0){
                retCode = "success add "+ to_string(ret1) +"url to db !!";
                res.set_content(retCode, "text/plain");
                return;
            }
            else{
                retCode = "Failed in add url to db !! retry it !!";
                res.set_content(retCode, "text/plain");
                return;
            }

        }else if(callType == 2){
            //(1)搜索
            int inputType = 0;
            if (req.has_param("inputType")) {
                inputType = atoi(req.get_param_value("urltxt").c_str());
                cout<<"inputType = "<<inputType<<endl;
            }
            ELOG_INFO("inputType=%d\n" , inputType);

            string input = "";
            if (req.has_param("input")) {
                input = req.get_param_value("input");
                cout<<"input = "<<input<<endl;
            }
            ELOG_INFO("input=%s\n" , input.c_str());

            int topN = 0;
            if (req.has_param("topN")) {
                topN = atoi(req.get_param_value("topN").c_str());
                cout<<"topN = "<<topN<<endl;
            }
            ELOG_INFO("topN=%d\n" , topN);

            vector<string> searchResult;
            int ret2 = machine->search(input, inputType, searchResult, topN);
            if(ret2 >= 1){
                string
                retCode = "result:\n "+ searchResult2str(searchResult);
                res.set_content(retCode, "text/plain");
                return;
            }
            else{
                retCode = "Failed search !! retcode: "+to_string(ret2);
                res.set_content(retCode, "text/plain");
                return;
            }

        }else{
            //不正确的输入
            retCode = "callType is not correct!!";
            res.set_content(retCode, "text/plain");
            return;
        }
    });

    cout<<"listenning localhost....."<<endl;
    svr.listen("127.0.0.1", 1234);

/*
    using namespace httplib;
    Server svr;
    svr.Get("/hi", [](const Request& req, Response& res) {
        res.set_content("Hello World!", "text/plain");
    });

    cout<<"listenning localhost....."<<endl;
    svr.listen("127.0.0.1", 1234);
*/
    return 0;
}
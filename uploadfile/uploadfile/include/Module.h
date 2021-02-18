#ifndef __MODULE__
#define __MODULE__
#include <string>
#include <vector>
#include "include/opencv2/opencv.hpp"

#ifdef __cplusplus
extern "C" {
#endif
   
#define ARCH_X64_LINUX
#ifdef ARCH_X64_LINUX
#define EXPORT __attribute__ ((visibility("default")))
#else
#define EXPORT __declspec(dllexport)
#endif

struct IModule {
public:
    /*
    *功能：以链接方式增加到图库,图库有默认路径
    *输入：一组urls
    *输出：int 成功的条数或失败码
    */
    virtual int addUrlToDB(std::vector<std::string> urls, std::string dbDir) = 0;
    virtual int addAUrlToDB(std::string url, std::string dbDir) = 0;
    virtual int addAPicToDB(std::string pic, std::string dbDir) = 0;

    /*
	*功能：搜索图像
	*输入：一个url或者本地路径、输入类型、输出结果地址数组、topN
	*输出：bool vector
	*/
    virtual int search(std::string urlOrPath, int type, std::vector<std::string> &urlsOrPath, int topN) = 0;

};

//init function
EXPORT IModule *InitialModule(std::string featdbfile);
//destory function
EXPORT void ExitModule(IModule *mObj);
#endif

#ifdef __cplusplus
}
#endif
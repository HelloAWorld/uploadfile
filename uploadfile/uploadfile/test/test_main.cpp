#include "Module.h"
#include "fstream"
#include <time.h>

using namespace std;



int main()
{
    //初始化
    IModule *m = InitialModule("");

    //double t0 = cv::getTickCount();

    vector<string> urls = {
        "i.pe168.cc/img/201806/16/160231977426407.png.thumb.png",
        "i.pe168.cc/img/201806/14/154601787426407.jpg.thumb.jpg",
        "i.pe168.cc/img/201805/24/145720477416451.jpg.thumb.jpg",
        "i.pe168.cc/img/201703/26/131304197366950.jpg.thumb.jpg",
        "i.pe168.cc/img/201705/16/094823247270592.jpg.thumb.jpg",
        "i.pe168.cc/img/201704/21/194022107257698.jpg.thumb.jpg",
        "i.pe168.cc/img/201703/13/220642427369655.jpg.thumb.jpg",
        "i.pe168.cc/img/201702/20/161215807365123.jpg.thumb.jpg",
        "https://i.pe168.com/img/201806/28/165114457428430.jpg.thumb.jpg",
        "https://i.pe168.com/img/201806/26/170745827428430.jpg.thumb.jpg",
        "https://i.pe168.com/img/201703/24/184742757336977.jpg.thumb.jpg",
        "https://i.pe168.com/img/201703/20/104501527335431.jpg.thumb.jpg",
        "https://i.pe168.com/img/201703/02/192850727364564.jpg.thumb.jpg",
        "https://i.pe168.com/img/201702/27/110147107302215.jpg.thumb.jpg",
        "https://i.pe168.com/img/201702/24/102829707260224.jpg.thumb.jpg",
        "https://i.pe168.com/img/201701/12/151941127356895.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/21/223802617319693.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/09/094346967345416.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/05/102451657320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/02/133515737320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/02/133331337320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/02/132947337320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/02/132730527320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/02/132506257320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/02/113849427320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/02/113326427320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201612/02/112932307320755.jpg.thumb.jpg",
        "https://i.pe168.com/img/201611/29/150424787298339.jpg.thumb.jpg",
        "https://i.pe168.com/img/201611/28/152044247335552.jpg.thumb.jpg",
        "https://i.pe168.com/img/201611/28/152044247335552.jpg.thumb.jpg",
        "https://i.pe168.com/img/201611/23/183520567319974.jpg.thumb.jpg",
        "https://i.pe168.com/img/201611/23/183520567319974.jpg.thumb.jpg",
    };

    std::cout << "addUrlToDB begin" << std::endl;

    //添加图像至图库
    m->addUrlToDB(urls,"/opt/urlpic/");

    std::cout << "addUrlToDB end" << std::endl;

   /* double t1 = cv::getTickCount();
    std::cout<<"addUrlToDB time0: "<<(t1-t0)*1000.0/cv::getTickFrequency()<<" ms"<<std::endl;*/

    //搜索
    vector<string> retVec;
    int topN = 5;
    m->search("https://i.pe168.com/img/201611/23/183520567319974.jpg.thumb.jpg", 0, retVec, topN);

  /*  double t2 = cv::getTickCount();
    std::cout<<"search time1: "<<(t2-t1)*1000.0/cv::getTickFrequency()<<" ms"<<std::endl;*/

    //查看结果缩略图
 /*   cv::Mat showMat = cv::Mat(cv::Size(1500,1200),CV_8UC3, cv::Scalar(0,0,0));
    if(topN> 20) topN = 20;
    if (retVec.size() < topN) topN = retVec.size();
    for (int i = 0; i < topN; i++)
    {
        cv::Mat im = cv::imread(retVec[i]);
        resize(im, im, cv::Size(300, 300));
        im.copyTo(showMat.colRange((i%5)*300, (i%5+1)*300).rowRange(i/5*300, (i/5+1) *300));
    }
    imwrite("MyPicture.jpg", showMat);*/
//    imshow("show",showMat);

    ExitModule(m);
    return 0;
}
#定义变量，使用变量:$(变量名)
CC=g++
#定义变量OBJS,表示将原文件中所有以.cpp结尾的文件替换成以.o结尾，即将.cpp源文件编译成.o文件
objects = main.o httphandler.o
objectsc = mongoose.o 
#包含目录
include_dirs = -Iinclude -I.
#cxxflags
CXXFLAGS = $(include_dirs) -m64 -g -DMG_ENABLE_SOCKETPAIR=1
EXEC=uploadfile

#依赖库
libs= -llibCBIR_linux64.so -llibcurl.so.4 -llibcurl.so.4.3.0 -llibHyCvUtils.so -lpthread
#依赖库目录
LDDIR+=-L/mnt/d/code/work/uploadfile/uploadfile/uploadfile/lib 
#执行所有
all: $(EXEC)

$(EXEC):$(objects) $(objectsc)
		$(CC) -o $@ $(objects)  $(objectsc) -Wall -Wfatal-errors $(CXXFLAGS) -ldl -lpthread
		cp uploadfile ./bin/uploadfile

#表示我的.o文件来自于.cpp文件
$(objects): %.o: %.cpp
		$(CC)   -Wall -Wfatal-errors  $(CXXFLAGS) $< -c -o $@ 

		
#表示我的.o文件来自于.c文件
$(objectsc): %.o: %.c
		$(CC)   -Wall -Wfatal-errors  $(CXXFLAGS) $< -c -o $@ 



		
#执行make clean指令
clean:
	#执行make clean指令时，需要执行的操作，比如下面的指令时指删除所有.o文件
		rm -rf $(objects)


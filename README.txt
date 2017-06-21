log4cpp的使用（windows&linux）

protobuf的使用（windows&linux）
protobuf地址下载地址:
https://github.com/google/protobuf.git

vc2015编译protobuf:
http://m.blog.csdn.net/u010008152/article/details/52088238

通过cmake生成sln文件来编译:
用cmd命令 cd 到3.3.0\cmake
mkdir build & cd build
mkdir solutionx86 & cd solutionx86
cmake -G "Visual Studio 12 2013" -DCMAKE_INSTALL_PREFIX=../../../../install ../.. -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=ON -DCMAKE_GENERATOR_TOOLSET=v120_xp
如果要编译64位的话
cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_INSTALL_PREFIX=../../../../install ../.. -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=ON -DCMAKE_GENERATOR_TOOLSET=v120_xp

注意中间不能有回车之类的，该命令行生成的是dll版本的（静态库的话要注意lib的运行库），其它版本请查考cmake目录下面的README.md


PHP制作后台网页，socket连接服务器

Gateway
.转发消息
.隔离GS与外网
.被攻击后，自动重启（需相关进程对其监控）
.多进程，多IP，多端口，防攻击

http://blog.csdn.net/ning521513/article/details/52288236
phpstorm破解网址：
http://idea.qinxi1992.cn/
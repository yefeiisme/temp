log4cplus��ʹ�ã�windows&linux��

protobuf��ַ���ص�ַ:
https://github.com/google/protobuf.git

vc2015����protobuf:
http://m.blog.csdn.net/u010008152/article/details/52088238

protobuf�ĺ����£����滹������php��ʹ��protobuf�ķ�ʽ��:
http://blog.csdn.net/hguisu/article/details/20721109

ͨ��cmake����sln�ļ�������:
��cmd���� cd ��3.3.0\cmake
mkdir build & cd build
mkdir solutionx86 & cd solutionx86
cmake -G "Visual Studio 12 2013" -DCMAKE_INSTALL_PREFIX=../../../../install ../.. -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_GENERATOR_TOOLSET=v120_xp
���Ҫ����64λ�Ļ�
cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_INSTALL_PREFIX=../../../../install ../.. -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_GENERATOR_TOOLSET=v120_xp

ע���м䲻���лس�֮��ģ������������ɵ���dll�汾�ģ���̬��Ļ�Ҫע��lib�����п⣩�������汾��鿼cmakeĿ¼�����README.md


PHP������̨��ҳ��socket���ӷ�����
PHP��protobuf��
https://github.com/google/protobuf/tree/master/php

Gateway
.ת����Ϣ
.����GS������
.���������Զ�����������ؽ��̶����أ�
.����̣���IP����˿ڣ�������

http://blog.csdn.net/ning521513/article/details/52288236
phpstorm�ƽ���ַ��
http://idea.qinxi1992.cn/


�����㣺
google��quic������upd�����紫�䣩
http://blog.csdn.net/wu5215080/article/details/56836196
tcp��
�����ڴ�

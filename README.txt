1.Ҫ�����wrapper��Ҫ�������»���������
1)����QTDIR������ export QTDIR=/usr/local/Trolltech/Qt-4.8.1
2)����WEBKIT_ROOT������ export WEBKIT_ROOT=/home/guanjianjun/code/Webkit
3)��QT��binĿ¼�ӵ�PATHǰ������ export PATH=$QTDIR/bin:$PATH:$HOME/bin
4)��QT��lib, /usr/local/lib, webkit��������ɵ�lib,���뵽LD_LIBRARY_PATH�У����磺
export LD_LIBRARY_PATH=$QTDIR/lib:/usr/local/lib:$WEBKIT_ROOT/WebKitBuild/Debug/lib:$WEBKIT_ROOT/WebKitBuild/Release/lib:$LD_LIBRARY_PATH

2.������Ŀ¼Core������Makefile�ļ�������
1)[Debug�汾] >qmake -o Makefile WrapperCore.pro
  [Release�汾] >qmake -o Makefile release_WrapperCore.pro
2)> make
**����ɹ������ServerWebkit/binĿ¼������webkit�ļ���

[GuanJianjun]
1.**���WebketServer�ǻ������ϰ汾�������˺ܶ�log��Ϣ������ģ�������ѧϰ������������.
2.webkit.ini���������1��webkitʵ��,����partner�������ļ���Ҳ������1������.


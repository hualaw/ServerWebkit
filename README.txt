1.要编译该wrapper需要配置如下环境变量：
1)定义QTDIR，例如 export QTDIR=/usr/local/Trolltech/Qt-4.8.1
2)定义WEBKIT_ROOT，例如 export WEBKIT_ROOT=/home/guanjianjun/code/Webkit
3)将QT的bin目录加到PATH前，例如 export PATH=$QTDIR/bin:$PATH:$HOME/bin
4)将QT的lib, /usr/local/lib, webkit编译后生成的lib,加入到LD_LIBRARY_PATH中，例如：
export LD_LIBRARY_PATH=$QTDIR/lib:/usr/local/lib:$WEBKIT_ROOT/WebKitBuild/Debug/lib:$WEBKIT_ROOT/WebKitBuild/Release/lib:$LD_LIBRARY_PATH

2.进入子目录Core下生成Makefile文件并编译
1)[Debug版本] >qmake -o Makefile WrapperCore.pro
  [Release版本] >qmake -o Makefile release_WrapperCore.pro
2)> make
**编译成功后会在ServerWebkit/bin目录下生成webkit文件。

[GuanJianjun]
1.**这个WebketServer是基于线上版本，加入了很多log信息后产生的，仅用于学习，而不是上线.
2.webkit.ini里仅配置了1个webkit实例,所以partner的配置文件里也仅配置1个窗口.


#include <iostream>
#include <QApplication>
#include "SWServer.h"
#include "SWGlobal.h"
#include "SendPacket.h"
#include "JobProcessor.h"

//global data structure
SWSettings settings;
WebkitWrapper **ww; //webkitwrapper object's array
metaData **md; //metaData array
JobProcessor *jp; 

SWLogger gLog;
Log4cplusWrapper *errLogWp = gLog.get_handle("ERROR");
Log4cplusWrapper *dbgLogWp = gLog.get_handle("DEBUG");
Log4cplusWrapper *runLogWp = gLog.get_handle("RUN");

class MyApplication : public QApplication
{
    public:
        MyApplication(int argc, char **argv) : QApplication(argc, argv) {}
        virtual ~MyApplication() {}
        virtual bool notify(QObject *rec, QEvent *ev)
        {
            try {
                return QApplication::notify(rec, ev);
            } catch(QString excpt) {
                qDebug() << "Unknown Exception: " << excpt;
                return false;
            }
        }
};

int main(int argc, char *argv[])
{
	if( argc < 3 )
	{
		std::cerr << "Usage: main.exe [port] [proxy]" << endl;
		return 1;
	}

    int ret_value = 0;

    MyApplication a(argc, argv);
    char *port = argv[1];
    QString proxy_str(argv[2]);

    SWServer server(port);
    QThread serverThread;
    QThread respThread;
    QNetworkProxy proxy = QNetworkProxy(QNetworkProxy::HttpProxy, proxy_str.section(":", 0, 0), proxy_str.section(":", 1, 1).toShort());

    jp = new JobProcessor();
    SendPacket sp;

    int wc = settings.getWindowCount();
    ww = new WebkitWrapper *[wc];
    md = new metaData *[wc];

    //construct worker window
    for(int i=0; i< wc; i++)
    {
        ww[i] = new WebkitWrapper(proxy);

        md[i] = new metaData();
        md[i]->state = NOT_BEGIN; 
        md[i]->socket = NULL;
        md[i]->resp = NULL;
    }

    gLog.log("RUN", QString("start SWServer successfully, start %1 windows").arg(wc));

    //make swserver run in it's own thread
    server.moveToThread(&serverThread);
    server.connect(&serverThread, SIGNAL(started()), SLOT(start()));
    a.connect(&serverThread, SIGNAL(finished()), SLOT(quit()));

    //create JobProcessor
    jp->connect(&server, SIGNAL(readyForProcess(int)), SLOT(start(int)));

    //send packets in response thread
    sp.moveToThread(&respThread);
    sp.connect(&respThread, SIGNAL(started()), SLOT(start()));
    a.connect(&respThread, SIGNAL(finished()), SLOT(quit()));

    //before application exit, cleanup global variables memory
    server.connect(&a, SIGNAL(aboutToQuit()), SLOT(cleanupGlobalParams()));

    //start run two thread
    serverThread.start();
    respThread.start();

    return a.exec();
}

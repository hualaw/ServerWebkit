# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.
message("You are running qmake on a generated .pro file. This may not work!")


HEADERS += ./BinaryStream.h \
    ./JobProcessor.h \
    ./ProtocolAnalyzer.h \
    ./SendPacket.h \
    ./RequestBuffer.h \
    ./SWClientSocket.h \
    ./SWServer.h \
    ./SWSettings.h \
    ../../include/SWUtil.h \
    ./SWWebPage.h \
    ./WebkitWrapper.h \
    ./SWLogger.h \
    ./SWQThread.h \
    $(WEBKIT_ROOT)/Tools/DumpRenderTree/qt/Log4cplusWrapper.h
SOURCES += ./main.cpp \
    ./JobProcessor.cpp \
    ./ProtocolAnalyzer.cpp \
    ./SendPacket.cpp \
    ./SWClientSocket.cpp \
    ./SWServer.cpp \
    ./SWSettings.cpp \
    ./SWWebPage.cpp \
    ./WebkitWrapper.cpp \
    ./SWLogger.cpp \
    ./SWQThread.cpp \
    $(WEBKIT_ROOT)/Tools/DumpRenderTree/qt/Log4cplusWrapper.cpp

TEMPLATE = lib
CONFIG += static no_keywords create_prl

QMAKE_CLEAN += libcore.prl

include(../configure.pri)

QT += network

QT_CONFIG -= opengl # hack to avoid linking with QtOpenGL
CONFIG += mobility
MOBILITY = multimedia

HEADERS += account.h \
    accountmanager.h \
    audio.h \
    chatsmanager.h \
    chatsession.h \
    contact.h \
    contactdata.h \
    contactgroup.h \
    contactinfo.h \
    contactlist.h \
    contactlistitem.h \
    contactlistmodel.h \
    contactmimedata.h \
    datetime.h \
    emoticonformat.h \
    emoticons.h \
    historylogger.h \
    historymanager.h \
    locations.h \
    message.h \
    mults.h \
    onlinestatus.h \
    onlinestatuses.h \
    plaintextexporter.h \
    plaintextparser.h \
    resourcemanager.h \
    rtfexporter.h \
    rtfparser.h \
    soundplayer.h \
    zlibbytearray.h \
    zodiac.h \
    mrim/proto.h \
    mrim/mrimmime.h \
    mrim/mrimdatastream.h \
    mrim/mrimclientprivate.h \
    mrim/mrimclient.h \
    mrim/filemessage.h \
    mrim/fileexistsdialog.h \
    mrim/tasks/tasksetvisibility.h \
    mrim/tasks/tasksendsms.h \
    mrim/tasks/tasksendmessage.h \
    mrim/tasks/tasksearchcontacts.h \
    mrim/tasks/taskrequestcontactinfo.h \
    mrim/tasks/taskrenamegroup.h \
    mrim/tasks/taskrenamecontact.h \
    mrim/tasks/taskremovegroup.h \
    mrim/tasks/taskremovecontact.h \
    mrim/tasks/tasknewconference.h \
    mrim/tasks/taskignorecontact.h \
    mrim/tasks/taskgetmpopsession.h \
    mrim/tasks/taskchangephones.h \
    mrim/tasks/taskchangegroup.h \
    mrim/tasks/taskbroadcastmessage.h \
    mrim/tasks/taskaddsmscontact.h \
    mrim/tasks/taskaddgroup.h \
    mrim/tasks/taskaddcontact.h \
    mrim/tasks/task.h \
    mrim/tasks/simpleblockingtask.h

SOURCES += account.cpp \
    accountmanager.cpp \
    audio.cpp \
    chatsession.cpp \
    contact.cpp \
    contactdata.cpp \
    contactgroup.cpp \
    contactinfo.cpp \
    contactlist.cpp \
    contactlistitem.cpp \
    contactlistmodel.cpp \
    contactmimedata.cpp \
    chatsmanager.cpp \
    historylogger.cpp \
    historymanager.cpp \
    datetime.cpp \
    locations.cpp \
    message.cpp \
    mults.cpp \
    onlinestatus.cpp \
    plaintextexporter.cpp \
    resourcemanager.cpp \
    rtfexporter.cpp \
    soundplayer.cpp \
    zlibbytearray.cpp \
    zodiac.cpp \
    mrim/mrimmime.cpp \
    mrim/mrimdatastream.cpp \
    mrim/mrimclientprivate.cpp \
    mrim/mrimclient.cpp \
    mrim/filemessage.cpp \
    mrim/fileexistsdialog.cpp \
    mrim/tasks/tasksetvisibility.cpp \
    mrim/tasks/tasksendsms.cpp \
    mrim/tasks/tasksendmessage.cpp \
    mrim/tasks/tasksearchcontacts.cpp \
    mrim/tasks/taskrequestcontactinfo.cpp \
    mrim/tasks/taskrenamegroup.cpp \
    mrim/tasks/taskrenamecontact.cpp \
    mrim/tasks/taskremovegroup.cpp \
    mrim/tasks/taskremovecontact.cpp \
    mrim/tasks/tasknewconference.cpp \
    mrim/tasks/taskignorecontact.cpp \
    mrim/tasks/taskgetmpopsession.cpp \
    mrim/tasks/taskchangephones.cpp \
    mrim/tasks/taskchangegroup.cpp \
    mrim/tasks/taskbroadcastmessage.cpp \
    mrim/tasks/taskaddsmscontact.cpp \
    mrim/tasks/taskaddgroup.cpp \
    mrim/tasks/taskaddcontact.cpp \
    mrim/tasks/task.cpp

LEXSOURCES += onlinestatuses.ll emoticons.ll rtf.ll plaintextparser.ll

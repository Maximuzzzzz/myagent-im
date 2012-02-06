TEMPLATE = app
TARGET = myagent-im
CONFIG += qt no_keywords warn_on debug
QT += network \
    phonon
INCLUDEPATH += /usr/include
TRANSLATIONS = locale/myagent-im_en.ts \
    locale/myagent-im_ru.ts \
    locale/myagent-im_uk.ts \
    locale/myagent-im_tr_TR.ts
LIBS += -laspell
win32 {
    QTPLUGIN += qjpeg \
        qgif
    LIBS += -llibapi \
        -llibbackend \
        -llibcommon \
        -llibexpand \
        -llibqueryparser \
        -llibflint \
        -lliblanguages \
        -llibunicode \
        -llibmulti \
        -llibmatcher \
        -llibremote \
        -llibnet \
        -llibinmemory \
        -llibquartz
}
unix {
    QMAKE_CXXFLAGS += $$system(xapian-config --cxxflags)
    LIBS += $$system(xapian-config --libs)
    CONFIG += link_pkgconfig
    PKGCONFIG += xscrnsaver zlib x11
}
unix {
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share/myagent-im
    HICOLOR = $$PREFIX/share/icons/hicolor
    DEFINES += DATADIR=\\\"$$DATADIR\\\"
    DEFINES += VERSION=\\\"0.5.0\\\"
    INSTALLS += target \
        smiles_animated \
        smiles_set03 \
        smiles_set04 \
        smiles_set05 \
        smiles_set06 \
        smiles_smiles \
        smiles_static_png \
        statuses_set01 \
        skins_en_US \
        skins_ru_RU \
        skins_uk_UA \
        data \
        desktop \
        appicon \
        sounds \
        hicolor16 \
        hicolor22 \
        hicolor24 \
        hicolor32 \
        hicolor48 \
        mults
    target.path = $$BINDIR
    data.path = $$DATADIR
    data.files += locale/*.qm
    skins_en_US.path = $$DATADIR/emoticons/skin/en_US
    skins_en_US.files = emoticons/skin/en_US/skin.txt
    skins_ru_RU.path = $$DATADIR/emoticons/skin/ru_RU
    skins_ru_RU.files = emoticons/skin/ru_RU/skin.txt
    skins_uk_UA.path = $$DATADIR/emoticons/skin/uk_UA
    skins_uk_UA.files = emoticons/skin/uk_UA/skin.txt
    STATUSESDIR = $$DATADIR/emoticons/status
    statuses_set01.path = $$STATUSESDIR/set01
    statuses_set01.files = emoticons/status/set01/*.png
    SMILESDIR = $$DATADIR/emoticons/smiles
    smiles_animated.path = $$SMILESDIR/animated
    smiles_animated.files = emoticons/smiles/animated/*.gif
    smiles_set03.path = $$SMILESDIR/set03
    smiles_set03.files = emoticons/smiles/set03/*.gif
    smiles_set03.files += emoticons/smiles/set03/*.png
    smiles_set04.path = $$SMILESDIR/set04
    smiles_set04.files = emoticons/smiles/set04/*.gif
    smiles_set04.files += emoticons/smiles/set04/*.png
    smiles_set05.path = $$SMILESDIR/set05
    smiles_set05.files = emoticons/smiles/set05/*.gif
    smiles_set05.files += emoticons/smiles/set05/*.png
    smiles_set06.path = $$SMILESDIR/set06
    smiles_set06.files = emoticons/smiles/set06/*.gif
    smiles_set06.files += emoticons/smiles/set06/*.png
    smiles_smiles.path = $$SMILESDIR/smiles
    smiles_smiles.files = emoticons/smiles/smiles/*.gif
    smiles_static_png.path = $$SMILESDIR/static_png
    smiles_static_png.files = emoticons/smiles/static_png/*.png
    hicolor16.path = $$HICOLOR/16x16/apps
    hicolor16.files = icons/hicolor/16x16/apps/*.png
    hicolor22.path = $$HICOLOR/22x22/apps
    hicolor22.files = icons/hicolor/22x22/apps/*.png
    hicolor24.path = $$HICOLOR/24x24/apps
    hicolor24.files = icons/hicolor/24x24/apps/*.png
    hicolor32.path = $$HICOLOR/32x32/apps
    hicolor32.files = icons/hicolor/32x32/apps/*.png
    hicolor48.path = $$HICOLOR/48x48/apps
    hicolor48.files = icons/hicolor/48x48/apps/*.png
    MULTSDIR = $$DATADIR/emoticons/flash
    mults.files = emoticons/flash/*.swf
    mults.files += emoticons/flash/*.png
    mults.path = $$MULTSDIR
    desktop.path = $$PREFIX/share/applications
    desktop.files += myagent-im.desktop
    sounds.path = $$DATADIR/sounds
    sounds.files = sounds/*
    appicon.path = $$PREFIX/share/pixmaps
    appicon.files = myagent-im.xpm
    appicon.files += myagent-im.png
}
RESOURCES += res.qrc
SOURCES += main.cpp \
    accountmanager.cpp \
    account.cpp \
    gui/logindialog.cpp \
    resourcemanager.cpp \
    gui/settingswindow.cpp \
    onlinestatus.cpp \
    audio.cpp \
    soundplayer.cpp \
    locations.cpp \
    gui/centerwindow.cpp \
    gui/contactlistwindow.cpp \
    idle/idle_x11.cpp \
    idle/idle_win.cpp \
    idle/idle_mac.cpp \
    idle/idle.cpp \
    contactlist.cpp \
    mrim/mrimmime.cpp \
    mrim/mrimdatastream.cpp \
    mrim/mrimclientprivate.cpp \
    mrim/mrimclient.cpp \
    mrim/filemessage.cpp \
    zlibbytearray.cpp \
    datetime.cpp \
    gui/onlinestatuseseditor.cpp \
    gui/onlinestatusselector.cpp \
    chatsmanager.cpp \
    historymanager.cpp \
    contactdata.cpp \
    contact.cpp \
    gui/emoticonwidget.cpp \
    gui/emoticonmovie.cpp \
    message.cpp \
    gui/chatwindowsmanager.cpp \
    gui/emoticonselector.cpp \
    chatsession.cpp \
    contactgroup.cpp \
    historylogger.cpp \
    gui/statuseditor.cpp \
    gui/statusbarwidget.cpp \
    gui/favouriteemoticonsdialog.cpp \
    flowlayout.cpp \
    gui/messageeditor.cpp \
    gui/toolbutton.cpp \
    spellchecker/spellhighlighter.cpp \
    spellchecker/spellchecker.cpp \
    spellchecker/aspellchecker.cpp \
    gui/chatwindow.cpp \
    rtfexporter.cpp \
    gui/newletterslabel.cpp \
    gui/linkbutton.cpp \
    gui/lineedit.cpp \
    gui/systemtrayicon.cpp \
    gui/popupwindowsstack.cpp \
    gui/popupwindow.cpp \
    gui/statusmenu.cpp \
    gui/models/conferencelistmodel.cpp \
    gui/models/contactlistbroadcastmodel.cpp \
    gui/contactlistitem.cpp \
    contactmimedata.cpp \
    gui/models/contactlistsortfilterproxymodel.cpp \
    gui/models/contactlistmodel.cpp \
    plaintextexporter.cpp \
    gui/avatarboxwithhandle.cpp \
    gui/avatarbox.cpp \
    gui/avatarwidget.cpp \
    gui/avatar.cpp \
    gui/filtercontactslineedit.cpp \
    gui/triggeroptionbutton.cpp \
    zodiac.cpp \
    contactinfo.cpp \
    gui/contactlisttreeview.cpp \
    gui/contactcontextmenu.cpp \
    gui/contactgroupcontextmenu.cpp \
    gui/centeredmessagebox.cpp \
    gui/inputlinedialog.cpp \
    gui/historyviewer.cpp \
    gui/historyviewtab.cpp \
    gui/historysearchtab.cpp \
    gui/statusbutton.cpp \
    gui/buttonwithmenu.cpp \
    gui/mainmenubutton.cpp \
    gui/authorizedialog.cpp \
    gui/newconferencedialog.cpp \
    gui/contactinfodialog.cpp \
    gui/conferencecontextmenu.cpp \
    gui/editphonesdialog.cpp \
    gui/removecontactdialog.cpp \
    gui/searchcontactsdialog.cpp \
    gui/radiogroupbox.cpp \
    gui/contactinfolistwindow.cpp \
    gui/contactinfolistwidget.cpp \
    gui/smallavatar.cpp \
    gui/addcontactdialog.cpp \
    gui/aboutdialog.cpp \
    gui/contactlistbroadcast.cpp \
    gui/contactlistconferencewithhandle.cpp \
    gui/contactlistconference.cpp \
    gui/animatedtextbrowser.cpp \
    gui/animation.cpp \
    gui/messageedit.cpp \
    gui/animatedtextedit.cpp \
    gui/animatedtextdocument.cpp \
    gui/smseditor.cpp \
    gui/smsedit.cpp \
    qgsmcodec.cpp \
    tasks/tasksetvisibility.cpp \
    tasks/tasksendsms.cpp \
    tasks/tasksendmessage.cpp \
    tasks/tasksearchcontacts.cpp \
    tasks/taskrequestcontactinfo.cpp \
    tasks/taskrenamegroup.cpp \
    tasks/taskrenamecontact.cpp \
    tasks/taskremovegroup.cpp \
    tasks/taskremovecontact.cpp \
    tasks/tasknewconference.cpp \
    tasks/taskignorecontact.cpp \
    tasks/taskgetmpopsession.cpp \
    tasks/taskchangephones.cpp \
    tasks/taskchangegroup.cpp \
    tasks/taskbroadcastmessage.cpp \
    tasks/taskaddsmscontact.cpp \
    tasks/taskaddgroup.cpp \
    tasks/taskaddcontact.cpp \
    tasks/task.cpp \
    gui/contactlistitemdelegate.cpp \
    mults.cpp \
    gui/multselector.cpp \
    gui/gnashplayer.cpp \
    mrim/fileexistsdialog.cpp

HEADERS += accountmanager.h \
    account.h \
    gui/logindialog.h \
    resourcemanager.h \
    gui/settingswindow.h \
    onlinestatus.h \
    mrim/proto.h \
    audio.h \
    soundplayer.h \
    emoticons.h \
    locations.h \
    gui/centerwindow.h \
    gui/contactlistwindow.h \
    idle/idle.h \
    contactlist.h \
    mrim/mrimmime.h \
    mrim/mrimdatastream.h \
    mrim/mrimclientprivate.h \
    mrim/mrimclient.h \
    mrim/filemessage.h \
    zlibbytearray.h \
    datetime.h \
    gui/onlinestatuseseditor.h \
    onlinestatuses.h \
    gui/onlinestatusselector.h \
    chatsmanager.h \
    historymanager.h \
    contactdata.h \
    contact.h \
    gui/emoticonwidget.h \
    gui/emoticonmovie.h \
    message.h \
    rtfparser.h \
    plaintextparser.h \
    gui/chatwindowsmanager.h \
    gui/emoticonselector.h \
    chatsession.h \
    contactgroup.h \
    historylogger.h \
    gui/statuseditor.h \
    gui/statusbarwidget.h \
    gui/favouriteemoticonsdialog.h \
    flowlayout.h \
    gui/messageeditor.h \
    gui/toolbutton.h \
    spellchecker/spellhighlighter.h \
    spellchecker/spellchecker.h \
    spellchecker/aspellchecker.h \
    gui/chatwindow.h \
    rtfexporter.h \
    emoticonformat.h \
    gui/igetanimation.h \
    gui/animationsupport.h \
    gui/newletterslabel.h \
    gui/linkbutton.h \
    gui/lineedit_p.h \
    gui/lineedit.h \
    gui/systemtrayicon.h \
    gui/popupwindowsstack.h \
    gui/popupwindow.h \
    gui/statusmenu.h \
    gui/models/conferencelistmodel.h \
    gui/models/contactlistbroadcastmodel.h \
    gui/contactlistitem.h \
    contactmimedata.h \
    gui/models/contactlistsortfilterproxymodel.h \
    gui/models/contactlistmodel.h \
    plaintextexporter.h \
    gui/avatarboxwithhandle.h \
    gui/avatarbox.h \
    gui/avatarwidget.h \
    gui/avatar.h \
    gui/filtercontactslineedit.h \
    gui/triggeroptionbutton.h \
    zodiac.h \
    contactinfo.h \
    gui/contactlisttreeview.h \
    gui/contactcontextmenu.h \
    gui/contactgroupcontextmenu.h \
    gui/centeredmessagebox.h \
    gui/inputlinedialog.h \
    gui/historyviewer.h \
    gui/historyviewtab.h \
    gui/historysearchtab.h \
    gui/statusbutton.h \
    gui/buttonwithmenu.h \
    gui/mainmenubutton.h \
    gui/authorizedialog.h \
    gui/newconferencedialog.h \
    gui/contactinfodialog.h \
    gui/conferencecontextmenu.h \
    gui/editphonesdialog.h \
    gui/removecontactdialog.h \
    gui/searchcontactsdialog.h \
    gui/radiogroupbox.h \
    gui/contactinfolistwindow.h \
    gui/contactinfolistwidget.h \
    gui/smallavatar.h \
    gui/addcontactdialog.h \
    gui/aboutdialog.h \
    gui/contactlistbroadcast.h \
    gui/contactlistconferencewithhandle.h \
    gui/contactlistconference.h \
    gui/animatedtextbrowser.h \
    gui/animationsupport.h \
    gui/animation.h \
    gui/messageedit.h \
    gui/animatedtextedit.h \
    gui/animatedtextdocument.h \
    gui/smseditor.h \
    gui/smsedit.h \
    qgsmcodec.h \
    tasks/tasksetvisibility.h \
    tasks/tasksendsms.h \
    tasks/tasksendmessage.h \
    tasks/tasksearchcontacts.h \
    tasks/taskrequestcontactinfo.h \
    tasks/taskrenamegroup.h \
    tasks/taskrenamecontact.h \
    tasks/taskremovegroup.h \
    tasks/taskremovecontact.h \
    tasks/tasknewconference.h \
    tasks/taskignorecontact.h \
    tasks/taskgetmpopsession.h \
    tasks/taskchangephones.h \
    tasks/taskchangegroup.h \
    tasks/taskbroadcastmessage.h \
    tasks/taskaddsmscontact.h \
    tasks/taskaddgroup.h \
    tasks/taskaddcontact.h \
    tasks/task.h \
    tasks/simpleblockingtask.h \
    gui/contactlistitemdelegate.h \
    mults.h \
    gui/multselector.h \
    gui/iflashplayer.h \
    gui/gnashplayer.h \
    mrim/fileexistsdialog.h

FORMS += gui/logindialog.ui \
    gui/authorize.ui \
    gui/newconference.ui \
    gui/askauthorization.ui \
    gui/editphones.ui \
    gui/searchcontacts.ui \
    gui/addcontact.ui \
    gui/aboutdialog.ui
LEXSOURCES += onlinestatuses.ll \
    emoticons.ll \
    rtf.ll \
    plaintextparser.ll

DESTDIR = ../bin
OBJECTS_DIR = ../build/obj
MOC_DIR = ../build/moc
UI_DIR = ../build/ui

TEMPLATE = app
TARGET = myagent-im
CONFIG += qt # warn_on debug
QT += network \
    phonon
INCLUDEPATH += /usr/include
TRANSLATIONS = locale/myagent-im_ru.ts \
    locale/myagent-im_uk.ts
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
    LIBS += -lxapian
    CONFIG += link_pkgconfig
    PKGCONFIG += xscrnsaver
}
unix { 
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share/myagent-im
	HICOLOR = $$PREFIX/share/icons/hicolor
	DEFINES += DATADIR=\\\"$$DATADIR\\\"
    DEFINES += VERSION=\\\"0.4.6\\\"
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
		hicolor48
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
	smiles_set04.files = emoticons/smiles/set04/*.png
	smiles_set05.path = $$SMILESDIR/set05
	smiles_set05.files = emoticons/smiles/set05/*.gif
	smiles_set05.files = emoticons/smiles/set05/*.png
	smiles_set06.path = $$SMILESDIR/set06
	smiles_set06.files = emoticons/smiles/set06/*.gif
	smiles_set06.files = emoticons/smiles/set06/*.png
	smiles_smiles.path = $$SMILESDIR/smiles
	smiles_smiles.files = emoticons/smiles/smiles/*.gif
    smiles_static_png.path = $$SMILESDIR/static_png
	smiles_static_png.files = emoticons/smiles/static_png/*.png
	hicolor16.path = $$HICOLOR/16x16/apps
	hicolor16.files =icons/hicolor/16x16/apps/*.png
	hicolor22.path = $$HICOLOR/22x22/apps
	hicolor22.files =icons/hicolor/22x22/apps/*.png
	hicolor24.path = $$HICOLOR/24x24/apps
	hicolor24.files =icons/hicolor/24x24/apps/*.png
	hicolor32.path = $$HICOLOR/32x32/apps
	hicolor32.files =icons/hicolor/32x32/apps/*.png
	hicolor48.path = $$HICOLOR/48x48/apps
	hicolor48.files =icons/hicolor/48x48/apps/*.png
	desktop.path = $$PREFIX/share/applications
    desktop.files += myagent-im.desktop
    sounds.path = $$DATADIR/sounds
    sounds.files = sounds/*
    appicon.path = $$PREFIX/share/pixmaps
	appicon.files += myagent-im.xpm
	appicon.files += myagent-im.png
}
RESOURCES += res.qrc
SOURCES += main.cpp \
    soundplayer.cpp \
    audio.cpp \
    mrimclient.cpp \
    account.cpp \
    contactgroup.cpp \
    contact.cpp \
    contactlist.cpp \
    accountmanager.cpp \
    contactlistmodel.cpp \
    logindialog.cpp \
    contactlistwindow.cpp \
    contactlistitem.cpp \
    zlibbytearray.cpp \
    contactlisttreeview.cpp \
    message.cpp \
    filemessage.cpp \
    contactmimedata.cpp \
    taskchangegroup.cpp \
    task.cpp \
    tasksendmessage.cpp \
    resourcemanager.cpp \
    taskrequestcontactinfo.cpp \
    statusbutton.cpp \
    buttonwithmenu.cpp \
    animatedtextdocument.cpp \
    animatedtextedit.cpp \
    animation.cpp \
    rtfexporter.cpp \
    emoticonmovie.cpp \
    emoticonwidget.cpp \
    emoticonselector.cpp \
    plaintextexporter.cpp \
    systemtrayicon.cpp \
    chatwindow.cpp \
    chatwindowsmanager.cpp \
    chatsession.cpp \
    chatsmanager.cpp \
    avatar.cpp \
    avatarwidget.cpp \
    mainmenubutton.cpp \
    contactcontextmenu.cpp \
    radiogroupbox.cpp \
    zodiac.cpp \
    locations.cpp \
    tasksearchcontacts.cpp \
    contactinfo.cpp \
    contactinfolistwidget.cpp \
    smallavatar.cpp \
    contactinfolistwindow.cpp \
    contactinfodialog.cpp \
    searchcontactsdialog.cpp \
    addcontactdialog.cpp \
    taskaddcontact.cpp \
    taskremovecontact.cpp \
    tasksetvisibility.cpp \
    linkbutton.cpp \
    mrimdatastream.cpp \
    toolbutton.cpp \
    datetime.cpp \
    historymanager.cpp \
    animatedtextbrowser.cpp \
    historylogger.cpp \
    historyviewer.cpp \
    authorizedialog.cpp \
    centerwindow.cpp \
    messageeditor.cpp \
    taskgetmpopsession.cpp \
    smseditor.cpp \
    newletterslabel.cpp \
    tasksendsms.cpp \
    smsedit.cpp \
    qgsmcodec.cpp \
    contactdata.cpp \
    editphonesdialog.cpp \
    taskchangephones.cpp \
    taskaddsmscontact.cpp \
    taskrenamecontact.cpp \
    taskaddgroup.cpp \
    inputlinedialog.cpp \
    taskremovegroup.cpp \
    contactgroupcontextmenu.cpp \
    taskrenamegroup.cpp \
    historyviewtab.cpp \
    avatarbox.cpp \
    avatarboxwithhandle.cpp \
    historysearchtab.cpp \
    idle/idle.cpp \
    idle/idle_mac.cpp \
    idle/idle_win.cpp \
    idle/idle_x11.cpp \
    mrimclientprivate.cpp \
    settingswindow.cpp \
    spellchecker/aspellchecker.cpp \
    spellchecker/spellchecker.cpp \
    spellchecker/spellhighlighter.cpp \
    centeredmessagebox.cpp \
    messageedit.cpp \
    favouriteemoticonsdialog.cpp \
    flowlayout.cpp \
    statusbarwidget.cpp \
    statuseditor.cpp \
    contactlistsortfilterproxymodel.cpp \
    lineedit.cpp \
    filtercontactslineedit.cpp \
    triggeroptionbutton.cpp \
    newconferencedialog.cpp \
    tasknewconference.cpp \
    mrimmime.cpp \
    conferencecontextmenu.cpp \
    onlinestatus.cpp \
    onlinestatuseseditor.cpp \
    onlinestatusselector.cpp \
    statusmenu.cpp \
    aboutdialog.cpp \
    taskignorecontact.cpp \
    contactlistbroadcast.cpp \
    contactlistconferencewithhandle.cpp \
    contactlistconference.cpp \
    removecontactdialog.cpp \
    contactlistbroadcastmodel.cpp \
    taskbroadcastmessage.cpp \
    conferencelistmodel.cpp \
    popupwindow.cpp \
	popupwindowsstack.cpp
HEADERS += mrimclient.h \
    soundplayer.h \
    audio.h \
    proto.h \
    account.h \
    contactgroup.h \
    contact.h \
    contactlist.h \
    accountmanager.h \
    contactlistmodel.h \
    logindialog.h \
    contactlistwindow.h \
    contactlistitem.h \
    zlibbytearray.h \
    contactlisttreeview.h \
    message.h \
    filemessage.h \
    contactmimedata.h \
    taskchangegroup.h \
    task.h \
    tasksendmessage.h \
    resourcemanager.h \
    taskrequestcontactinfo.h \
    statusbutton.h \
    buttonwithmenu.h \
    emoticons.h \
    rtfparser.h \
    animatedtextedit.h \
    animatedtextdocument.h \
    animation.h \
    rtfexporter.h \
    emoticonmovie.h \
    emoticonwidget.h \
    emoticonformat.h \
    emoticonselector.h \
    plaintextexporter.h \
    systemtrayicon.h \
    chatwindow.h \
    chatwindowsmanager.h \
    chatsession.h \
    chatsmanager.h \
    avatar.h \
    avatarwidget.h \
    mainmenubutton.h \
    onlinestatus.h \
    onlinestatuses.h \
    contactcontextmenu.h \
    radiogroupbox.h \
    zodiac.h \
    locations.h \
    tasksearchcontacts.h \
    contactinfo.h \
    contactinfolistwidget.h \
    smallavatar.h \
    contactinfolistwindow.h \
    contactinfodialog.h \
    searchcontactsdialog.h \
    addcontactdialog.h \
    taskaddcontact.h \
    taskremovecontact.h \
    tasksetvisibility.h \
    linkbutton.h \
    mrimdatastream.h \
    toolbutton.h \
    datetime.h \
    historymanager.h \
    animationsupport.h \
    animatedtextbrowser.h \
    igetanimation.h \
    historylogger.h \
    historyviewer.h \
    authorizedialog.h \
    plaintextparser.h \
    contactdata.h \
    centerwindow.h \
    messageeditor.h \
    taskgetmpopsession.h \
    smseditor.h \
    newletterslabel.h \
    tasksendsms.h \
    smsedit.h \
    qgsmcodec.h \
    editphonesdialog.h \
    taskchangephones.h \
    taskaddsmscontact.h \
    simpleblockingtask.h \
    taskrenamecontact.h \
    taskaddgroup.h \
    inputlinedialog.h \
    taskremovegroup.h \
    contactgroupcontextmenu.h \
    taskrenamegroup.h \
    historyviewtab.h \
    avatarbox.h \
    avatarboxwithhandle.h \
    historysearchtab.h \
    idle/idle.h \
    mrimclientprivate.h \
    settingswindow.h \
    spellchecker/aspellchecker.h \
    spellchecker/spellchecker.h \
    spellchecker/spellhighlighter.h \
    centeredmessagebox.h \
    messageedit.h \
    favouriteemoticonsdialog.h \
    flowlayout.h \
    statusbarwidget.h \
    statuseditor.h \
    contactlistsortfilterproxymodel.h \
    lineedit_p.h \
    lineedit.h \
    filtercontactslineedit.h \
    triggeroptionbutton.h \
    newconferencedialog.h \
    tasknewconference.h \
    mrimmime.h \
    conferencecontextmenu.h \
    onlinestatuseseditor.h \
    onlinestatusselector.h \
    statusmenu.h \
    aboutdialog.h \
    taskignorecontact.h \
    contactlistbroadcast.h \
    contactlistconferencewithhandle.h \
    contactlistconference.h \
    removecontactdialog.h \
    contactlistbroadcastmodel.h \
    taskbroadcastmessage.h \
    conferencelistmodel.h \
    popupwindow.h \
	popupwindowsstack.h
FORMS += logindialog.ui \
    searchcontacts.ui \
    addcontact.ui \
    askauthorization.ui \
    authorize.ui \
    editphones.ui \
    newconference.ui \
    aboutdialog.ui \
    aboutdialog.ui
LEXSOURCES += emoticons.ll \
    rtf.ll \
    plaintextparser.ll \
    onlinestatuses.ll

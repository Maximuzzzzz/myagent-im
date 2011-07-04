clebsCheck(builddll) {
    CLEBS_DEPENDENCIES *= builddll
}

clebsDependency(builddll) {
    unix:CONFIG += hide_symbols
    unix:release:CONFIG += separate_debug_info
#    CONFIG += dll
#    TEMPLATE = lib
#    target.path = $$LIBDIR
#    INSTALLS *= target
}

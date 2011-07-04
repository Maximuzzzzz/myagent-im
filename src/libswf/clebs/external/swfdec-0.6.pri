clebsCheck(swfdec-0.6) {
    unix {
	system (pkg-config swfdec-0.6):CLEBS_DEPENDENCIES += swfdec-0.6
    }
}

clebsDependency(swfdec-0.6) {
    win32 {
    }

    unix {
        PKGCONFIG *= swfdec-0.6
    }
}

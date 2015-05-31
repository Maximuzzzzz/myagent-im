**Получение rpm из deb**


Для этого необходимы пакеты alien и rpmbuild и сам пакет
http://myagent-im.googlecode.com/files/myagent-im_0.2.1-0karmic1_i386.deb


в консоли из под root
ввести команду


**#alien --to-rpm /путь/до/файла**


нажать _Enter_


deb пакет преобразуется в **myagent-im-0.2.1-1.i386.rpm**


**Для x86\_64**

Добавление репозитория
в консоли из под root
ввести команду:

#urpmi.addmedia edutesting\_free [ftp://ftp.linuxcenter.ru/pub/Mandriva/EduMandriva/educontrib/2010.0/x86_64/free/release/](ftp://ftp.linuxcenter.ru/pub/Mandriva/EduMandriva/educontrib/2010.0/x86_64/free/release/)
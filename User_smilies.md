# Пользовательские смайлы #

Добавление своих смайлов доступно с версии 0.4.5

1. Создаём или находим каталог _/usr/share/myagent-im/emoticon/skin/{локаль}/user_<br>
2. В нём создаём файл с любым именем. Если Вы собираетесь редактировать существующий набор, открываете нужный файл и идём к пункту №4<br>
3. Создаём теги <code>&lt;</code>ROOT<code>&gt;</code> и <code>&lt;</code>SMILES<code>&gt;</code> (с закрывающими, естественно). В теге <code>&lt;</code>SMILES<code>&gt;</code> создаём строки:<br>
path = "[путь<code>_</code>к<code>_</code>смайлам]"<br>
ColumnsCount = "8"<br>
где path - путь к смайлам, начиная с каталога <i>/usr/share/myagent-im/emoticon</i>. По умолчанию и рекомендуемо ставить значение "/smiles".<br>
4. Далее начинается структура, описывающая набор ваших смайлов. Размещается после тега <code>&lt;</code>SMILES<code>&gt;</code><br>
<code>&lt;</code>MYSET[номер]<code>&gt;</code><br>
title = "[наименование<code>_</code>набора]"<br>
path = "/[имя<code>_</code>папки<code>_</code>в<code>_</code>которой<code>_</code>лежат<code>_</code>смайлы]"<br>
logo = <br>"/[наименование<code>_</code>смайла<code>_</code>который<code>_</code>отобразится<code>_</code>на<code>_</code>вкладке<code>_</code>в<code>_</code>окне<code>_</code>выбора<code>_</code>смайликов]"<br>
<code>&lt;</code>ITEMS<code>&gt;</code><br>
<code>&lt;</code>OBJ[номер<code>_</code>объекта<code>_</code>по<code>_</code>порядку]<code>&gt;</code><br>
id = "[номер<code>_</code>смайла]"<br>
alt = "[передаваемое<code>_</code>имя<code>_</code>смайла]"<br>
tip = "[отображаемое<code>_</code>имя<code>_</code>смайла]"<br>
src_width = "[ширина<code>_</code>смайла]"<br>
src_height = "[высота<code>_</code>смайла]"<br>
<code>&lt;ICON</code>><<code>DEFAULT&gt;</code>src="/[имя<code>_</code>файла<code>_</code>смайла]"<code>&lt;</code>/DEFAULT<code>&gt;&lt;</code>/ICON>`<br>
<code>&lt;</code>/OBJ[тот<code>_</code>же<code>_</code>номер<code>_</code>объекта]<code>&gt;</code><br>
<code>&lt;</code>OBJ[следующий<code>_</code>номер<code>_</code>объекта<code>_</code>по<code>_</code>порядку]<code>&gt;</code><br>
...<br>
<code>&lt;</code>/OBJ[тот<code>_</code>же<code>_</code>номер<code>_</code>объекта]<code>&gt;</code><br>
...<br>
<code>&lt;</code>/ITEMS<code>&gt;</code><br>
<code>&lt;</code>/MYSET[номер]<code>&gt;</code><br>

Примечания:<br>
Первый тег (<b><code>&lt;</code>MYSET[номер]<code>&gt;</code></b>) не обязательно должен быть <i>MYSET</i> - главное, чтобы он не повторялся с уже имеющимися <i>SET06</i>, <i>SET05</i>, <i>ANIMATED</i> ...<br>
<b>[наименование_набора]</b> - необходимый, хотя и не отображается в программе<br>
<b>[имя<code>_</code>папки<code>_</code>в<code>_</code>которой<code>_</code>лежат<code>_</code>смайлы]</b> - создаём данную папку в <i>/usr/share/myagent-im/emoticon/smiles</i><br>
<b>[наименование<code>_</code>смайла<code>_</code>который<code>_</code>отобразится<code>_</code>на<code>_</code>вкладке<code>_</code>в<code>_</code>окне<code>_</code>выбора<code>_</code>смайликов]</b> - в селекторе смайликов он будет на вкладке. <b>ВАЖНО!</b> Анимированные форматы (gif, ...) не опробованы! Желательно сделать этот смайл в формате png или jpg. Лучше png :)<br>
<b>[номер<code>_</code>объекта<code>_</code>по<code>_</code>порядку]</b> - 01, 02, 03 ... Главное, чтобы не было разрывов<br>
<b>[номер<code>_</code>смайла]</b> - самый главный аргумент смайла - именно он отвечает за то, какой смайл из всей кучи отобразить. <b>ВАЖНО</b>, чтобы этот номер для параметра id не повторялся нигде в данном файле! Вместо номера можно использовать строчку, типа <i>mysmile01</i><br>
<b>[передаваемое<code>_</code>имя<code>_</code>смайла]</b> - при копировании текста из окна чата именно этот текст будет выводиться вместо смайла<br>
<b>[отображаемое<code>_</code>имя<code>_</code>смайла]</b> - при наведении мыши на смайл будет отображаться этот текст<br>
<b>[ширина<code>_</code>смайла]</b>, <b>[высота<code>_</code>смайла]</b> - параметры картинки<br>
<b>[имя<code>_</code>файла<code>_</code>смайла]</b> - например <i>mysmile01.gif</i><br>

<b>ВАЖНО!</b> Сохранять файл skin.txt в <b>UTF-8</b>! Конец строки "UNIX/Linux" - другие не проверялись<br>
<b>ВАЖНО!</b> Не забывайте, что ваши смайлы увидят только те люди, у которых установлен тот же самый набор, что и у Вас.<br>
<b>ВАЖНО!</b> Если пользовательских наборов несколько, важно чтобы id смайлов не пересекались, иначе Вы или собеседник будете видеть не те смайлы, какие изначально были выбраны.<br>
Формат записи набора смайлов можно просмотреть в файле <i>/usr/share/myagent-im/emoticon/skin/{локаль}/skin.txt</i>
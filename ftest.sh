#!/bin/bash
if [ $# != 1 ];then
	echo "Формат командной строки :"
	echo "          ./ftest.sh  <имя_исполняемого_файла_программы>"
	echo "Правильный вывод после запуска :"
	echo "==============================="
	echo "Файл XXXXX, размер : YYYY"
	echo ". . . ."
	echo ". . . ."
	echo ". . . ."
	echo ". . . . . ."
	echo ". . . . . ."
	echo "Тест завершен..."
	echo "==============================="
	echo "Каждое отличие при выводе = одна ошибка"
	exit
fi
echo -n "Файл $1, размер : "
ls -o $1 | cut -d " " -f 4
	export OUT_FILE_NAME=""
	rm -f 1.txt 2.txt 3.txt 33.txt t.t
	echo -n "Тест " >1.txt
	echo "завершен..." >2.txt
	./$1 1.txt 2.txt >t.t
	if [ $? != 2 ]; then
		echo "Ошибка 2"
	fi
	cat t.t  | sed -e 's/Не/./; s/указан/./; s/выходной/./; s/файл!/./'
export OUT_FILE_NAME=3.txt
./$1 >t.t
if [ $? != 3 ]; then
	echo "Ошибка 3"
fi
cat t.t  | sed -e 's/Не/./; s/указаны/./; s/входные/./; s/файлы!/./'
	./$1 1.txt 2.txt
	if [ $? != 0 ]; then
		echo "Ошибка 0"
	fi
	mv 3.txt 33.txt
./$1 1.txt 4.txt >t.t
if [ $? != 4 ]; then
	echo "Ошибка 4"
fi
cat t.t  | sed -e 's/Файл/./; s/4.txt/./; s/не/./; s/найден!/./'
	chmod -r 1.txt
	./$1 1.txt 2.txt >t.t
	if [ $? != 5 ]; then
		echo "Ошибка 5"
	fi
	chmod +r 1.txt
	cat t.t  | sed -e 's/Ошибка/./; s/при/./; s/работе/./; s/с/./; s/файлом/./; s/1.txt/./'
chmod -w 3.txt
./$1 1.txt 2.txt >t.t
if [ $? != 6 ]; then
	echo "Ошибка 6"
fi
cat t.t  | sed -e 's/Ошибка/./; s/при/./; s/работе/./; s/с/./; s/файлом/./; s/3.txt/./'
chmod +w 3.txt
	cat 33.txt

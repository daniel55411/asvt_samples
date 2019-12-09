#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Тема:
#	Работа с файлами, командной строкой и переменными окружения
#
# Задание:
#	- написать аналогичную программу на ассемблере для АРМ
#	- использовать в программе те же тексты сообщений и коды возврата
#	- отладить все ветки
#	- за выполненное задание получить на следующей паре 2 балла
#	- авторы трех исполняемых файлов минимального размера получат 3 балла
#	- проверка задания будет выполняться автоматически (скриптом)
#
import os, sys
of = "OUT_FILE_NAME"
if of in os.environ and len(os.environ[of]): 
	ofile = os.environ[of]
else:
	#print("Не указан выходной файл!")
	sys.exit(2)
if len(sys.argv) == 1:
	#print("Не указаны входные файлы!")
	sys.exit(3)
try:
	f2 = open(ofile, 'wb')
	outfile = []
	for fle in sys.argv[1:]:
		try:
			if os.path.isfile(fle):
				f1 = open(fle, 'rb')
				outfile.append(f1.read())
				f1.close()
			else:
				#print("Файл " + fle + " не найден!")
				sys.exit(4)
		except Exception:
			#print("Ошибка при работе с файлом " + fle)
			f2.close()
			sys.exit(5)
	for f in outfile: 
		f2.write(f)
	f2.close()
except Exception:
	#print("Ошибка при работе с файлом " + ofile)
	sys.exit(6)

#include "convert.h"

Convert::Convert()
{
}

QString  Convert::convert_text_to_translit(QString work_text)
{
	QString text = work_text;
	int j = 0, len = text.length();

	QString trans_text = text;
	qDebug()<<"Begin main loop";
	for (int i = 0; i < len; i++) {
		qDebug()<<"If standart Ascii ";
		qDebug()<<text[i].toLatin1();
		if (isascii(text[i].toLatin1())) {
			//trans_text.append(text[i]);
			j++;
			continue;
		}
		qDebug()<<"Get translit";
		text[i].toLower();
		const char *new_symbol = get_translit((unsigned short)text[i].toLatin1());

		for(; i < len - 1; i++) {
			if (isascii(text[i + 1].toAscii()))
				break;
			//char tmp_char = text[i].toAscii();
			if (get_translit((unsigned short)text[i].toLatin1()) != NULL)
				break;
		}
		qDebug()<<"Replace symbol";
		qDebug()<<"Symbol"<<new_symbol;
		qDebug()<<"Compare";
		while (*new_symbol != NULL) {
			qDebug()<<"Do replasement";
			trans_text[j++] = *new_symbol;
			new_symbol++;
		}
	}
	qDebug()<<"Exit";
	//trans_text[j] = NULL;
	/*free(*work_text);
	*work_text = trans_text; */
	return QString(trans_text);
}

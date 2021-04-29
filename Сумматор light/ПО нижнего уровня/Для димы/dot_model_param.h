#ifndef __DOT_MODEL_PARAM_H__
#define __DOT_MODEL_PARAM_H__


// число опорных точек по которым строится кусочно линейная апроксимация
// число линий апроксимации = NUMBER_UROVNEV - 1
#define NUMBER_UROVNEV	2


// Нормированные частоты в зависимости от уровня заполнения датчика топливом
extern double normalizeInputFrequency[ NUMBER_UROVNEV ];

// Таблица значений OutTable полученных в MathCAD
// Данная таблица определяет выходную характеристику ДОТ в зависимости от
// типа прибора к которому подключается ДОТ

extern double OutTable[ NUMBER_UROVNEV ];

#endif

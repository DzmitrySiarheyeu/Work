#include "dot_model_param.h"

// Нормированные частоты в зависимости от уровня заполнения датчика топливом
double normalizeInputFrequency[ NUMBER_UROVNEV ] = {0.0, 1.0};

// Таблица значений OutTable полученных в MathCAD
// Данная таблица определяет выходную характеристику ДОТ в зависимости от
// типа прибора к которому подключается ДОТ

double OutTable[ NUMBER_UROVNEV ] = {
0,
100
};

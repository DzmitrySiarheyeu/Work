#ifndef COOKIE_H
#define COOKIE_H

#include "config.h"
#if(_HTTP_ == DEVICE_ON)

int CheckCookies(char *file, char *req);
void SetCookies(char *login, char *passw);
void AddCookies(char *req);

#endif //_HTTP_

#endif

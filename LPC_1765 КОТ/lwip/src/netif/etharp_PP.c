# 1 "D:/VSS_WORK/LPC_GSM/LPC_2138/../lwip/src/netif/etharp.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "D:/VSS_WORK/LPC_GSM/LPC_2138/../lwip/src/netif/etharp.c"
# 47 "D:/VSS_WORK/LPC_GSM/LPC_2138/../lwip/src/netif/etharp.c"
# 1 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/opt.h" 1
# 45 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/opt.h"
# 1 "d:/vss_work/LPC_GSM/Common/lwipopts.h" 1
# 1 "d:/vss_work/LPC_GSM/LPC_2138/Config.h" 1
# 2 "d:/vss_work/LPC_GSM/Common/lwipopts.h" 2
# 48 "d:/vss_work/LPC_GSM/Common/lwipopts.h"
# 1 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/debug.h" 1
# 35 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/debug.h"
# 1 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/arch.h" 1
# 43 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/arch.h"
# 1 "d:/vss_work/LPC_GSM/contrib/ports/FreeRTOS/str91x/arch/cc.h" 1
# 35 "d:/vss_work/LPC_GSM/contrib/ports/FreeRTOS/str91x/arch/cc.h"
# 1 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/stdio.h" 1 3 4
# 17 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/stdio.h" 3 4
# 1 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/__crossworks.h" 1 3 4
# 22 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/__crossworks.h" 3 4
typedef __builtin_va_list __va_list;
# 18 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/stdio.h" 2 3 4
# 33 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/stdio.h" 3 4
typedef unsigned long size_t;


int puts(const char *);
int putchar(int);
int getchar(void);
int sprintf(char *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int vsnprintf(char *, size_t, const char *, __va_list);

int printf(const char *, ...);
int vprintf(const char *, __va_list);
int vsprintf(char *, const char *, __va_list);

int scanf(const char *, ...);
int sscanf(const char *, const char *, ...);
int vscanf(const char *, __va_list);
int vsscanf(const char *, const char *, __va_list);
char *gets(char *);
# 65 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/stdio.h" 3 4
typedef void FILE;
typedef long fpos_t;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
void clearerr(FILE *);
int fclose(FILE *);
int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
int fgetc(FILE *);
int fgetpos(FILE *, fpos_t *);
char *fgets(char *, int, FILE *);
FILE *fopen(const char *, const char *);
int fprintf(FILE *, const char *, ...);
int fputc(int, FILE *);
int fputs(const char *, FILE *);
size_t fread(void *, size_t, size_t, FILE *);
FILE *freopen(const char *, const char *, FILE *);
int fscanf(FILE *, const char *, ...);
int fseek(FILE *, long, int);
int fsetpos(FILE *, const fpos_t *);
long ftell(FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int getc(FILE *);
void perror(const char *);
int putc(int, FILE *);
int remove(const char *);
int rename(const char *, const char *);
void rewind(FILE *);
void setbuf(FILE *, char *);
int setvbuf(FILE *, char *, int, size_t);
FILE *tmpfile(void);
char * tmpnam(char *);
int ungetc(int, FILE *);
int vfprintf(FILE *, const char *, __va_list);
int vfscanf(FILE *, const char *, __va_list);
# 36 "d:/vss_work/LPC_GSM/contrib/ports/FreeRTOS/str91x/arch/cc.h" 2
# 1 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/stdlib.h" 1 3 4
# 36 "c:/program files/rowley associates limited/crossworks for arm 1.7/include/stdlib.h" 3 4
typedef struct
{
  int quot;
  int rem;
} div_t;

typedef struct
{
  long quot;
  long rem;
} ldiv_t;

typedef struct
{
  long long quot;
  long rem;
} lldiv_t;

int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);

div_t div(int numer, int denom);
ldiv_t ldiv(long int numer, long int denom);
lldiv_t lldiv(long long int numer, long long int denom);

void *calloc(size_t nobj, size_t size);

void *malloc(size_t size);

void *realloc(void *p, size_t size);

void free(void *p);

double atof(const char *nptr);
double strtod(const char *nptr, char **endptr);
float strtof(const char *nptr, char **endptr);
int atoi(const char *nptr);
long int atol(const char *nptr);
long long int atoll(const char *nptr);

long int strtol(const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
unsigned long long int strtoull(const char *nptr, char **endptr, int base);

int rand(void);
void srand(unsigned int seed);

void *bsearch( const void *key, const void *buf, size_t num, size_t size, int (*compare)(const void *, const void *) );
void qsort( void *buf, size_t num, size_t size, int (*compare)(const void *, const void *) );

void abort(void);
void exit( int exit_code );
int atexit( void (*func)(void) );

char *getenv( const char *name );
int system( const char *command );


char *itoa(int val, char *buf, int radix);
char *utoa(unsigned val, char *buf, int radix);
char *ltoa(long val, char *buf, int radix);
char *ultoa(unsigned long val, char *buf, int radix);
char *lltoa(long long val, char *buf, int radix);
char *ulltoa(unsigned long long val, char *buf, int radix);
# 37 "d:/vss_work/LPC_GSM/contrib/ports/FreeRTOS/str91x/arch/cc.h" 2


# 1 "d:/vss_work/LPC_GSM/contrib/ports/FreeRTOS/str91x/arch/cpu.h" 1
# 40 "d:/vss_work/LPC_GSM/contrib/ports/FreeRTOS/str91x/arch/cc.h" 2

typedef unsigned char u8_t;
typedef signed char s8_t;
typedef unsigned short u16_t;
typedef signed short s16_t;
typedef unsigned long u32_t;
typedef signed long s32_t;
typedef u32_t mem_ptr_t;
typedef int sys_prot_t;
# 63 "d:/vss_work/LPC_GSM/contrib/ports/FreeRTOS/str91x/arch/cc.h"
void debug_printf_RTOS(const char *format,...);
# 44 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/arch.h" 2
# 219 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/arch.h"
extern int errno;
# 36 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/debug.h" 2
# 49 "d:/vss_work/LPC_GSM/Common/lwipopts.h" 2
# 46 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/opt.h" 2
# 1 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/debug.h" 1
# 47 "d:/vss_work/LPC_GSM/lwip/src/include/lwip/opt.h" 2
# 48 "D:/VSS_WORK/LPC_GSM/LPC_2138/../lwip/src/netif/etharp.c" 2

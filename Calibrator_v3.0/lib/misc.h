#ifndef MISC_H
#define MISC_H

#define for_each(__item__, __array__) \
    for(int __keep__ = 1, \
            __count__ = 0,\
            __size__ = sizeof (__array__) / sizeof *(__array__); \
        __keep__ && __count__ != __size__; \
        __keep__ = !__keep__, __count__++) \
      for(__item__ = (__array__) + __count__; __keep__; __keep__ = !__keep__)


#define _debug_on() {DDRD |= (1 << PD6);}
#define _OO {PORTD |= (1<<PD6);}
#define _PP {PORTD &= ~(1<<PD6);}
#define _OP {PIND |= (1 << PD6);}

#endif /* MISC_H */


#include <stdint.h>

const uint8_t bar[] = {
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0     /*  ????....  */
};

const uint8_t clock_big_digits[10][88] = {
/*uint8_t clock_0[] =*/ {
0x00,    /*  ........  */
0x07,    /*  .....???  */
0x1F,    /*  ...?????  */
0x3F,    /*  ..??????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFC,    /*  ??????..  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xFC,    /*  ??????..  */
0xFF,    /*  ????????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x3F,    /*  ..??????  */
0x1F,    /*  ...?????  */
0x07,    /*  .....???  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x00,    /*  ........  */

0xC0,    /*  ??......  */
0xF8,    /*  ?????...  */
0xFE,    /*  ???????.  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x3F,    /*  ..??????  */
0x0F,    /*  ....????  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x0F,    /*  ....????  */
0x3F,    /*  ..??????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFE,    /*  ???????.  */
0xF8,    /*  ?????...  */
0xC0,    /*  ??......  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00     /*  ........  */

},

/*uint8_t clock_1[] = */{
0x00,    /*  ........  */
0x00,    /*  ........  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x1F,    /*  ...?????  */
0x1F,    /*  ...?????  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x00,    /*  ........  */
0x00     /*  ........  */
},

/*uint8_t clock_2[] = */{
0x00,    /*  ........  */
0x00,    /*  ........  */
0x7E,    /*  .??????.  */
0x7C,    /*  .?????..  */
0x7C,    /*  .?????..  */
0x78,    /*  .????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xFC,    /*  ??????..  */
0xFE,    /*  ???????.  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x3F,    /*  ..??????  */
0x1F,    /*  ...?????  */
0x07,    /*  .....???  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x01,    /*  .......?  */
0x03,    /*  ......??  */
0x07,    /*  .....???  */
0x1F,    /*  ...?????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFE,    /*  ???????.  */
0xFC,    /*  ??????..  */
0xF8,    /*  ?????...  */
0xE0,    /*  ???.....  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x07,    /*  .....???  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x1F,    /*  ...?????  */
0x3F,    /*  ..??????  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xF7,    /*  ????.???  */
0xE7,    /*  ???..???  */
0xC7,    /*  ??...???  */
0x87,    /*  ?....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x00,    /*  ........  */
0x00     /*  ........  */
},


/*uint8_t clock_3[] = */{
0x00,    /*  ........  */
0x00,    /*  ........  */
0x7E,    /*  .??????.  */
0x7C,    /*  .?????..  */
0x7C,    /*  .?????..  */
0x7C,    /*  .?????..  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xFC,    /*  ??????..  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x3F,    /*  ..??????  */
0x3F,    /*  ..??????  */
0x0F,    /*  ....????  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x3E,    /*  ..?????.  */
0x3E,    /*  ..?????.  */
0x3E,    /*  ..?????.  */
0x3E,    /*  ..?????.  */
0x7E,    /*  .??????.  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xE7,    /*  ???..???  */
0xC7,    /*  ??...???  */
0x83,    /*  ?.....??  */
0x01,    /*  .......?  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x1F,    /*  ...?????  */
0x1F,    /*  ...?????  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFE,    /*  ???????.  */
0xF8,    /*  ?????...  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00     /*  ........  */
},


/*uint8_t clock_4[] =*/ {
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x01,    /*  .......?  */
0x03,    /*  ......??  */
0x07,    /*  .....???  */
0x1F,    /*  ...?????  */
0x3F,    /*  ..??????  */
0xFE,    /*  ???????.  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x03,    /*  ......??  */
0x07,    /*  .....???  */
0x1F,    /*  ...?????  */
0x3F,    /*  ..??????  */
0x7F,    /*  .???????  */
0xFD,    /*  ??????.?  */
0xF1,    /*  ????...?  */
0xE1,    /*  ???....?  */
0xC1,    /*  ??.....?  */
0x01,    /*  .......?  */
0x01,    /*  .......?  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x01,    /*  .......?  */
0x01,    /*  .......?  */
0x01,    /*  .......?  */
0x01,    /*  .......?  */

0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00     /*  ........  */
},


/*uint8_t clock_5[] =*/ {
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0xFE,    /*  ???????.  */
0xFE,    /*  ???????.  */
0xFC,    /*  ??????..  */
0xFC,    /*  ??????..  */
0xFC,    /*  ??????..  */
0xFC,    /*  ??????..  */
0x7C,    /*  .?????..  */
0x7C,    /*  .?????..  */
0x7C,    /*  .?????..  */
0x7E,    /*  .??????.  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x3F,    /*  ..??????  */
0x3F,    /*  ..??????  */
0x3F,    /*  ..??????  */
0x1F,    /*  ...?????  */
0x0F,    /*  ....????  */
0x03,    /*  ......??  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x1F,    /*  ...?????  */
0x1F,    /*  ...?????  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x0F,    /*  ....????  */
0x1F,    /*  ...?????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFE,    /*  ???????.  */
0xFC,    /*  ??????..  */
0xF0,    /*  ????....  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00     /*  ........  */
},


{
0x00,    /*  ........  */
0x00,    /*  ........  */
0x01,    /*  .......?  */
0x07,    /*  .....???  */
0x0F,    /*  ....????  */
0x1F,    /*  ...?????  */
0x3F,    /*  ..??????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x7E,    /*  .??????.  */
0xFC,    /*  ??????..  */
0xFC,    /*  ??????..  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xFC,    /*  ??????..  */
0xFC,    /*  ??????..  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xBC,    /*  ?.????..  */
0x3C,    /*  ..????..  */
0x7C,    /*  .?????..  */
0x7C,    /*  .?????..  */
0x7C,    /*  .?????..  */
0x7E,    /*  .??????.  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x3F,    /*  ..??????  */
0x3F,    /*  ..??????  */
0x1F,    /*  ...?????  */
0x0F,    /*  ....????  */
0x03,    /*  ......??  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0xE0,    /*  ???.....  */
0xF8,    /*  ?????...  */
0xFE,    /*  ???????.  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x0F,    /*  ....????  */
0x07,    /*  .....???  */
0x03,    /*  ......??  */
0x03,    /*  ......??  */
0x03,    /*  ......??  */
0x0F,    /*  ....????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFE,    /*  ???????.  */
0xFC,    /*  ??????..  */
0xF0,    /*  ????....  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00     /*  ........  */
},

{
0x00,    /*  ........  */
0x00,    /*  ........  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF8,    /*  ?????...  */
0xF9,    /*  ?????..?  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFC,    /*  ??????..  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x01,    /*  .......?  */
0x07,    /*  .....???  */
0x1F,    /*  ...?????  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFC,    /*  ??????..  */
0xF0,    /*  ????....  */
0xC0,    /*  ??......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x01,    /*  .......?  */
0x07,    /*  .....???  */
0x1F,    /*  ...?????  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFE,    /*  ???????.  */
0xF8,    /*  ?????...  */
0xC0,    /*  ??......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x40,    /*  .?......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x80,    /*  ?.......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00     /*  ........  */
},


/*uint8_t clock_8[] =*/ {
0x00,    /*  ........  */
0x0F,    /*  ....????  */
0x1F,    /*  ...?????  */
0x3F,    /*  ..??????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xF8,    /*  ?????...  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF8,    /*  ?????...  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x3F,    /*  ..??????  */
0x1F,    /*  ...?????  */
0x0F,    /*  ....????  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x81,    /*  ?......?  */
0xE3,    /*  ???...??  */
0xF7,    /*  ????.???  */
0xF7,    /*  ????.???  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFC,    /*  ??????..  */
0x7E,    /*  .??????.  */
0x3E,    /*  ..?????.  */
0x3E,    /*  ..?????.  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xE7,    /*  ???..???  */
0xC7,    /*  ??...???  */
0x83,    /*  ?.....??  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0xF8,    /*  ?????...  */
0xFE,    /*  ???????.  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x0F,    /*  ....????  */
0x07,    /*  .....???  */
0x03,    /*  ......??  */
0x03,    /*  ......??  */
0x03,    /*  ......??  */
0x03,    /*  ......??  */
0x87,    /*  ?....???  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFE,    /*  ???????.  */
0xF8,    /*  ?????...  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00     /*  ........  */
},

/*uint8_t clock_9[] =*/ {
0x00,    /*  ........  */
0x03,    /*  ......??  */
0x0F,    /*  ....????  */
0x1F,    /*  ...?????  */
0x3F,    /*  ..??????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFC,    /*  ??????..  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF0,    /*  ????....  */
0xF8,    /*  ?????...  */
0xFC,    /*  ??????..  */
0xFF,    /*  ????????  */
0x7F,    /*  .???????  */
0x7F,    /*  .???????  */
0x3F,    /*  ..??????  */
0x1F,    /*  ...?????  */
0x0F,    /*  ....????  */
0x01,    /*  .......?  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0xF0,    /*  ????....  */
0xFC,    /*  ??????..  */
0xFE,    /*  ???????.  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x1F,    /*  ...?????  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0x0F,    /*  ....????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x0F,    /*  ....????  */
0x07,    /*  .....???  */
0x07,    /*  .....???  */
0x87,    /*  ?....???  */
0x87,    /*  ?....???  */
0x87,    /*  ?....???  */
0x87,    /*  ?....???  */
0x8F,    /*  ?...????  */
0x0F,    /*  ....????  */
0x1F,    /*  ...?????  */
0x7F,    /*  .???????  */
0xFF,    /*  ????????  */
0xFF,    /*  ????????  */
0xFE,    /*  ???????.  */
0xFC,    /*  ??????..  */
0xF8,    /*  ?????...  */
0xE0,    /*  ???.....  */
0x00,    /*  ........  */
0x00,    /*  ........  */

0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0xC0,    /*  ??......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x80,    /*  ?.......  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00,    /*  ........  */
0x00     /*  ........  */
}};


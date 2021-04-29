using System;
using System.Collections.Generic;
using System.Text;

namespace DUTConfig_V3.Forms
{
  public  class Termo_correction
    {
      public ushort Adress_polinom_termo_correct = 0x002d;
      public ushort Adress_polinom_t = 0x0037;

      public int low = -40;
      public int high = 70;
      public int step = 5;


      public bool norm = true;
      public bool stock = true;
      public bool itog = true;
      public bool itog_calc = true;

      public float[] polinom_termo_correct;
      public float[] polinom_t;

      public Termo_correction()
      {
          polinom_termo_correct = new float[] { 1, 0, 0, 0, 0 };
          polinom_t = new float[] { -3.6389969978f, 27.6522496853f, -84.819636594f, 102.9668647154f };
      }
    }
}

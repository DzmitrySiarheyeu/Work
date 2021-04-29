using System.Collections.Generic;

namespace DUTConfig_V3.Forms.etc
{
    public class Point
    {
        private double Number;
        public double number
        {
            get { return Number; }
            set { Number = value; }
        }


        private double X;
        public double x
        {
            get { return X; }
            set { X = value; }
        }



        private double Y;
        public double y
        {
            get { return Y; }
            set { Y = value; }
        }
    }

    public class Point_Current
    {
        private string X;
        public string x
        {
            get { return X; }
            set { X = value; }
        }

        private double Y;
        public double y
        {
            get { return Y; }
            set { Y = value; }
        }
    }

    public class Points
    {
        public List<Point> Table_baze;
        public List<Point> Table_read_from_device;
        public List<Point> Table_restore;
        public List<Point> Table_yitog;   
        public List<Point_Current> Table_current;

        public Points()
        {          
            Table_restore = new List<Point>();
            Table_yitog = new List<Point>();           
            Table_current = new List<Point_Current>();
        }

        private double[] koeff;
        public double[] KOEFF
        {
            get
            {
                return koeff;
            }
            set
            {
                koeff = value;
            }
        }

        private int Step_Polinom;
        public int STEP_POLINOM
        {
            get { return Step_Polinom; }
            set
            {
                if (Step_Polinom > 5) Step_Polinom = 5;
                Step_Polinom = value;
            }
        }

        private int Step_Polinom_Current;
        public int STEP_POLINOM_CURRENT
        {
            get { return Step_Polinom_Current; }
            set
            {
                if (Step_Polinom_Current > 5) Step_Polinom_Current = 5;
                Step_Polinom_Current = value;
            }
        }

        private int FMIN = 0;
        public int Fmin
        {
            get { return FMIN; }
            set { FMIN = value; }
        }

        private int FMAX = 0;
        public int Fmax
        {
            get { return FMAX; }
            set { FMAX = value; }
        }

        private int DIAP = 100;
        public int Diap
        {
            get { return DIAP; }
            set { DIAP = value; }
        }

        public bool F = true;
        public bool f
        {
            get { return F; }
            set
            {
                F = value;
            }
        }

    }
}

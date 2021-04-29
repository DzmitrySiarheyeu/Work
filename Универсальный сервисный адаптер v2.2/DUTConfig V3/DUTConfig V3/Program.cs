﻿using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace DUTConfig_V3
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            try
            {

                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new Forms.MainForm());
            }
            catch/*(Exception ex)*/
            {
                /*MessageBox.Show(ex.ToString());*/
            }
        }
    }
}

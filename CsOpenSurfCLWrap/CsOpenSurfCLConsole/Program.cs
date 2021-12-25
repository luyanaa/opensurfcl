using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace CsOpenSurfCLConsole
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine(CSurfCL.GetVersion());
            CSurfCL.PrintCLDevices();
            CSurfCL surfCL = new CSurfCL();
            Bitmap bitmap = (Bitmap)Bitmap.FromFile("../../../../data/pic_carsten.bmp");

           

            surfCL.ScanImage(bitmap, 0.0002f, 5, 2, false);
            surfCL.CalculateSurfDescriptor();
            SIpoint[] pa = surfCL.GetIPointsLink();
            foreach (SIpoint p in pa)
            {
                Console.WriteLine(p.x + "   " + p.y);
                Console.WriteLine("\tlaplacian:" + p.laplacian);
                Console.WriteLine("\torientation:" + p.orientation);
            }
        }
    }
}

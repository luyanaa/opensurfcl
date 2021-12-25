using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;



namespace OpenSURFDemo
{
    /// <summary>
    /// This class is based on the orginal OpenSurf Demo by Chris Evans!
    /// http://code.google.com/p/opensurf1/
    /// </summary>
    public partial class DemoSURF : Form
    {
        CSurfCL surf;
        public DemoSURF()
        {
            InitializeComponent();
            
            //Create the CSurfCL with the first device found
            surf = new CSurfCL();
        }

    

        private void btnRunSurf_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.ShowDialog();
            string pathToFile = openFileDialog.FileName;
            if (pathToFile == "")
                return;

            Stopwatch watch = new Stopwatch();
            watch.Start();
            SIpoint[] ipts;

            //Load the Image
            Bitmap img = new Bitmap(pathToFile);
            pbMainPicture.Image = img;

            //Scan for points
            surf.ScanImage(img, 0.0002f, 5, 2, false);
            ipts = surf.GetIPointsLink();
        
            //Calculate the descriptors and orientation
            //only the orientation is visible in this demo
            surf.CalculateSurfDescriptor();

            PaintSURF(img, ipts);

            watch.Stop();

            this.Text = "DemoSURFCL - Elapsed time: " + watch.Elapsed + 
                        " for " + ipts.Length + "points";
        }

        private void PaintSURF(Bitmap img, SIpoint[] ipts)
        {
            Graphics g = Graphics.FromImage(img);
      
            Pen redPen = new Pen(Color.Red);
            Pen bluePen = new Pen(Color.Blue);
            Pen myPen;
      
            foreach (SIpoint ip in ipts)
            {
                int S = 2 * Convert.ToInt32(2.5f * ip.scale);
                int R = Convert.ToInt32(S / 2f);

                Point pt = new Point(Convert.ToInt32(ip.x), Convert.ToInt32(ip.y));
                Point ptR = new Point(Convert.ToInt32(R * Math.Cos(ip.orientation)),
                                Convert.ToInt32(R * Math.Sin(ip.orientation)));

                myPen = (ip.laplacian > 0 ? bluePen : redPen);
        
                g.DrawEllipse(myPen, pt.X - R, pt.Y - R, S, S);
                g.DrawLine(new Pen(Color.FromArgb(0, 255, 0)), new Point(pt.X, pt.Y), new Point(pt.X + ptR.X, pt.Y + ptR.Y));
            }
        }
    }
}

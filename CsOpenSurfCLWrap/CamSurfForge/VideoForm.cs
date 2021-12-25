using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using AForge.Video.DirectShow;
using AForge.Video;

namespace CamSurfForge
{
    /// <summary>
    /// The VideoForm uses Aforge to get images from a selected webcam.
    /// </summary>
    public partial class VideoForm : Form
    {
        FilterInfoCollection filterInfoCollection;
        VideoCaptureDevice curDevice = null;

        CSurfCL surf;
        public VideoForm()
        {
            InitializeComponent();
            refreshList();

            surf = new CSurfCL();
            this.comboBoxVideoDev.SelectedIndexChanged += comboBoxVideoDev_SelectedIndexChanged;
            this.Resize += VideoForm_Resize;
            this.Disposed += this_OnDispose;
        }

        public void refreshList()
        {
            filterInfoCollection = new FilterInfoCollection(FilterCategory.VideoInputDevice);

            foreach (FilterInfo fi in filterInfoCollection)
            {
                this.comboBoxVideoDev.Items.Add(fi.Name);
            }
        }

        public void comboBoxVideoDev_SelectedIndexChanged(object sender, EventArgs args)
        {
            if (curDevice != null)
            {
                //close old device
                curDevice.Stop();
            }

            FilterInfo nextFI = filterInfoCollection[comboBoxVideoDev.SelectedIndex];
            curDevice = new VideoCaptureDevice(nextFI.MonikerString);
            curDevice.Start();
            curDevice.NewFrame += curDevice_NewFrameEventHandler;

            started = false;
        }

        bool working = false;
        double calcTime = 0;
        uint frames = 0;

        DateTime startTime;
        bool started = false;
        public void curDevice_NewFrameEventHandler(object sender, NewFrameEventArgs eventArgs)
        {
            if (started == false)
            {
                started = true;

                startTime = DateTime.Now;
            }
            //this.pictureBox1.Height = eventArgs.Frame.Height;
            //this.pictureBox1.Width = eventArgs.Frame.Width;
            //Bitmap newImage = new Bitmap( eventArgs.Frame,new Size(120,80));
            //IntegralImage iimg = IntegralImage.FromImage(newImage);

            //List<IPoint> points = FastHessian.getIpoints(0.0002f, 5, 2, iimg);
            //SurfDescriptor.DecribeInterestPoints(points, false, false, iimg);
            if (working == false)
            {
                working = true;

                Bitmap copy = new Bitmap(eventArgs.Frame);

                DateTime start = System.DateTime.Now;
                surf.ScanImage(eventArgs.Frame, 0.0008f, 5, 2, false);
                surf.CalculateSurfDescriptor();
                
                SIpoint[] points = surf.GetIPointsLink();
                calcTime = (System.DateTime.Now - start).TotalSeconds;

                DrawIPoints(points, copy);
                this.pictureBox1.Image = copy;
                this.pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;

                try
                {
                    this.Invoke(new Action(deleg));
                }
                catch (ObjectDisposedException e)
                {

                    curDevice.Stop();
                }
                catch (InvalidOperationException e)
                {

                    curDevice.Stop();
                }


                frames++;
                working = false;
            }
        }

        void deleg()
        {
            this.Text = "" + calcTime + "    fps: " + frames / (DateTime.Now - startTime).TotalSeconds;
        }
               
        public void VideoForm_Resize(object sender, EventArgs args)
        {
            this.pictureBox1.Width = this.Width - 20;
            this.pictureBox1.Height = this.Height - 60;
        }

        void this_OnDispose(object sender, EventArgs args)
        {

            curDevice.Stop();
            System.Environment.Exit(0);
        }

        /// <summary>
        /// Draws a List of IPoints
        /// </summary>
        /// <param name="points">the List of IPoints</param>
        /// <param name="target">target Image</param>
        public void DrawIPoints(SIpoint[] points, Bitmap target)
        {
            Graphics g = Graphics.FromImage(target);
            foreach (SIpoint curPoint in points)
            {
                if (curPoint.laplacian > 0)
                {
                    DrawPoint(curPoint, g, Pens.Blue);
                }
                else
                {
                    DrawPoint(curPoint, g, Pens.Red);
                }
            }
        }

        /// <summary>
        /// draws a Point
        /// </summary>
        /// <param name="point">IPoint to draw</param>
        /// <param name="g">Graphics </param>
        /// <param name="p">Pen to use</param>
        public void DrawPoint(SIpoint point, Graphics g, Pen p)
        {
            float halfScale = point.scale * 0.5f;
            g.DrawEllipse(p, point.x - halfScale, point.y - halfScale, point.scale, point.scale);


            
            //ignore orientation 
            float vx = (float)(Math.Cos(point.orientation) * halfScale);
            float vy = (float)(Math.Sin(point.orientation) * halfScale);

            vx = vx * 2;
            vy = vy * 2;

            g.DrawLine(Pens.Green, new PointF(point.x, point.y), new PointF(point.x + vx, point.y + vy));



            //g.DrawString("" + point.orientation, SystemFonts.CaptionFont, SystemBrushes.ActiveCaption, new PointF(point.x + vx, point.y + vy));
        }
    }
}

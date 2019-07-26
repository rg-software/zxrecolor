using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text;

using Coords = System.Drawing.Point;

namespace ZXColor
{
    class ZXColor
    {
        const int FarAwayCoord = 5000;

        class PicDescriptor
        {
            public Coords P1 = new Coords(FarAwayCoord, FarAwayCoord);
            public Coords P2 = new Coords(FarAwayCoord, FarAwayCoord);
            public Coords Corner = new Coords(FarAwayCoord, FarAwayCoord);
            public override string ToString()
            {
                return string.Format("{0} {1} {2} {3} {4} {5}", P1.X, P1.Y, P2.X, P2.Y, Corner.X, Corner.Y);
            }

            static public PicDescriptor FromString(string s)
            {
                string[] spl = s.Split(' '); 
                PicDescriptor pd = new PicDescriptor();
                pd.P1.X = Convert.ToInt32(spl[0]); pd.P1.Y = Convert.ToInt32(spl[1]);
                pd.P2.X = Convert.ToInt32(spl[2]); pd.P2.Y = Convert.ToInt32(spl[3]);
                pd.Corner.X = Convert.ToInt32(spl[4]); pd.Corner.Y = Convert.ToInt32(spl[5]);

                return pd;
            }
        }

        public static int CellSize = 16;
        static Color MyBackColor = Color.White;
        static Color GridColor = Color.LightGray;
        static Color SelColor = Color.Yellow;
        static Color CornerColor = Color.Red;
        static int CornerOffset = 10;
        static Dictionary<string, PicDescriptor> AllObjects = new Dictionary<string, PicDescriptor>();
        //static string ZxSubdir = "OutputZX";
        //static string PcSubdir = "OutputPC";
        static string OutSubdir = "Output";
        static string[] NamePrefix = new string[] { "", "pc_", "zx_" };
        static Bitmap mOriginalImage;
        static Bitmap mGrayBitmap;

        static public void ClearObjects()
        {
            AllObjects = new Dictionary<string, PicDescriptor>();
        }

        static public void CreateObject(string name)
        {
            AllObjects.Add(name, new PicDescriptor());
        }

        static public void DeleteObject(string name)
        {
            AllObjects.Remove(name);
        }

        static public Bitmap LoadAndRescaleImage(string fileName, int scaleFactor)
        {
            Bitmap b = new Bitmap(fileName);
            mOriginalImage = (Bitmap)b.Clone();
            Bitmap newB = new Bitmap(b.Width * scaleFactor, b.Height * scaleFactor);

            using (Graphics g = Graphics.FromImage((Image)newB))
            {
                g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
                g.DrawImage(b, 0, 0, newB.Width, newB.Height);
            }

            MakeGrayCells(newB);
            mGrayBitmap = (Bitmap)newB.Clone();

            return newB;
        }

        static void MakeGrayCells(Bitmap b)
        {
            BitmapData bmpData = b.LockBits(new Rectangle(0, 0, b.Width, b.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            int[] pixelData = new int[bmpData.Height * bmpData.Width];
            Marshal.Copy(bmpData.Scan0, pixelData, 0, pixelData.Length);
            b.UnlockBits(bmpData);

            for (int x = 0; x < b.Width / CellSize; ++x)
                for (int y = 0; y < b.Height / CellSize; ++y)
                    if (x % 2 == y % 2)
                        MakeOneGrayCell(pixelData, x * CellSize, y * CellSize, b);

            bmpData = b.LockBits(new Rectangle(0, 0, b.Width, b.Height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            Marshal.Copy(pixelData, 0, bmpData.Scan0, pixelData.Length);
            b.UnlockBits(bmpData);
        }

        static void MakeOneGrayCell(int[] pixelData, int i, int j, Bitmap b)
        {
            for (int x = 0; x < CellSize; ++x)
                for (int y = 0; y < CellSize; ++y)
                {
                    int idx = (j + y) * b.Width + (x + i);
                    if (pixelData[idx] == MyBackColor.ToArgb())
                        pixelData[idx] = GridColor.ToArgb();
                }
        }

        static public void ToggleCell(int i, int j, bool isLeftButton, bool isMiddleButton, string objName)
        {
            PicDescriptor pd = AllObjects[objName];

            if (!isMiddleButton)
            {
                if (isLeftButton) pd.P1 = new Coords(i, j); else pd.P2 = new Coords(i, j);
                if (pd.P1.X == FarAwayCoord) pd.P1 = pd.P2;
                if (pd.P2.X == FarAwayCoord) pd.P2 = pd.P1;
            }
            else
            {
                if(pd.Corner.X == FarAwayCoord)
                    pd.Corner = new Coords(i, j);
                else
                    pd.Corner = new Coords(FarAwayCoord, FarAwayCoord);
            }
        }

        static public Bitmap CreateUpdatedScreen(string objName)
        {
            Bitmap b = (Bitmap)mGrayBitmap.Clone();
            PicDescriptor pd = AllObjects[objName];

/*            for (int i = Math.Min(pd.P1.X, pd.P2.X); i <= Math.Max(pd.P1.X, pd.P2.X); ++i)
                for (int j = Math.Min(pd.P1.Y, pd.P2.Y); j <= Math.Max(pd.P1.Y, pd.P2.Y); ++j)
                {
                    if (i == FarAwayCoord || j == FarAwayCoord)
                        continue;
                    
                    // make yellow background
                    for (int x = 0; x < CellSize; ++x)
                        for (int y = 0; y < CellSize; ++y)
                        {
                            int c = b.GetPixel(i * CellSize + x, j * CellSize + y).ToArgb();
                            if (c == MyBackColor.ToArgb() || c == GridColor.ToArgb())
                                b.SetPixel(i * CellSize + x, j * CellSize + y, SelColor);
                        }
                }
 */

            int minI = Math.Min(pd.P1.X, pd.P2.X), maxI = Math.Max(pd.P1.X, pd.P2.X);
            int minJ = Math.Min(pd.P1.Y, pd.P2.Y), maxJ = Math.Max(pd.P1.Y, pd.P2.Y);

            // make a yellow frame
            Graphics.FromImage(b).DrawRectangle(new Pen(SelColor,2), minI * CellSize, minJ * CellSize, (maxI - minI+1)* CellSize, (maxJ - minJ+1) * CellSize);
           

            if(pd.Corner.X != FarAwayCoord)
            {
                // make small green triangle
                int px = pd.Corner.X * CellSize, py = pd.Corner.Y * CellSize;
                for (int i = 0; i <= CornerOffset; ++i)
                    for (int j = 0; j <= CornerOffset - i; ++j)
                        b.SetPixel(px + i, py + j, CornerColor);
            }

            return b;
        }

        static public void SaveObjects(string fileName)
        {
            System.IO.StreamWriter sw = new System.IO.StreamWriter(fileName);

            foreach (KeyValuePair<string, PicDescriptor> kvp in AllObjects)
                    sw.WriteLine(kvp.Key + " " + kvp.Value.ToString());

            sw.Close();
        }

        static public string[] LoadObjects(string fileName)
        {
            System.IO.StreamReader sr = new System.IO.StreamReader(fileName);
            ClearObjects();

            string line;
            List<string> result = new List<string>();
            while ((line = sr.ReadLine()) != null)
            { 
                string name = line.Substring(0, line.IndexOf(' '));
                string tail = line.Substring(line.IndexOf(' ') + 1);

                AllObjects[name] = PicDescriptor.FromString(tail);
                result.Add(name);
            }
            
            sr.Close();
            
            return result.ToArray();
        }

        static public void PerformSplit(int scaleCoeff)
        {
            int size = CellSize / scaleCoeff;
            System.IO.Directory.CreateDirectory(OutSubdir);
            
            foreach (KeyValuePair<string, PicDescriptor> kvp in AllObjects)
            {
                string name = OutSubdir + "\\" + NamePrefix[scaleCoeff] + kvp.Key;
                PicDescriptor pd = kvp.Value;

                int minX = Math.Min(pd.P1.X, pd.P2.X), maxX = Math.Max(pd.P1.X, pd.P2.X);
                int minY = Math.Min(pd.P1.Y, pd.P2.Y), maxY = Math.Max(pd.P1.Y, pd.P2.Y);
                int cX = pd.Corner.X != FarAwayCoord ? pd.Corner.X : minX;
                int cY = pd.Corner.Y != FarAwayCoord ? pd.Corner.Y : minY;

                // switch off .info file writing for now
              /*  System.IO.StreamWriter sw = new System.IO.StreamWriter(name + ".info");
                sw.WriteLine((cX - minX).ToString() + " " + (cY - minY).ToString());
                sw.Close();
                */

                Bitmap b = new Bitmap((maxX - minX + 1) * size, (maxY - minY + 1) * size, PixelFormat.Format24bppRgb);
                Graphics.FromImage(b).DrawImage(mOriginalImage,
                    new Rectangle(0, 0, b.Width, b.Height),
                    new Rectangle(minX * size, minY * size, b.Width, b.Height),
                    GraphicsUnit.Pixel);
                b.Save(name + ".bmp", ImageFormat.Bmp);
            }
        }
    }
}


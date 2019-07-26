using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ZXColor
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            cbScale.SelectedIndex = 0;
        }

        int getScaleFactor()
        {
            return cbScale.SelectedIndex + 1; // PC = 1, ZX = 2
        }

        private void btnLoadImage_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                pictureBox.Image = ZXColor.LoadAndRescaleImage(openFileDialog.FileName, getScaleFactor());
            }
        }

        private void btnAddObject_Click(object sender, EventArgs e)
        {
            if (!lbObjects.Items.Contains(tbEditObject.Text))
            {
                lbObjects.Items.Add(tbEditObject.Text);
                ZXColor.CreateObject(tbEditObject.Text);
            }
        }

        private void btnDelObject_Click(object sender, EventArgs e)
        {
            if (lbObjects.SelectedIndex >= 0)
            {
                ZXColor.DeleteObject((string)lbObjects.Items[lbObjects.SelectedIndex]);
                lbObjects.Items.RemoveAt(lbObjects.SelectedIndex);
            }
        }

        private void pictureBox_MouseClick(object sender, MouseEventArgs e)
        {
            if (lbObjects.SelectedIndex >= 0)
            {
                ZXColor.ToggleCell(e.X / ZXColor.CellSize, e.Y / ZXColor.CellSize, e.Button == MouseButtons.Left, e.Button == MouseButtons.Middle, (string)lbObjects.Items[lbObjects.SelectedIndex]);
                UpdateScreen();
            }
        }

        private void UpdateScreen()
        {
            if(lbObjects.SelectedIndex >= 0)
            {
                pictureBox.Image = ZXColor.CreateUpdatedScreen((string)lbObjects.Items[lbObjects.SelectedIndex]);
            }
        }
        
        private void lbObjects_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateScreen();
        }

        private void btnNew_Click(object sender, EventArgs e)
        {
            lbObjects.Items.Clear();
            ZXColor.ClearObjects();
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            SaveFileDialog openFileDialog = new SaveFileDialog();

            if (openFileDialog.ShowDialog() == DialogResult.OK)
                ZXColor.SaveObjects(openFileDialog.FileName);
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string[] items = ZXColor.LoadObjects(openFileDialog.FileName);
                lbObjects.Items.AddRange(items);
            }
        }

        private void btnZxSplit_Click(object sender, EventArgs e)
        {
            Text = "ZXColor - working...";
            Application.DoEvents();
            ZXColor.PerformSplit(getScaleFactor());
            Text = "ZXColor";
        }
    }
}
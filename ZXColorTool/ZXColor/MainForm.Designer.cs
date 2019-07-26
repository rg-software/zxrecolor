namespace ZXColor
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.btnZxSplit = new System.Windows.Forms.Button();
            this.cbScale = new System.Windows.Forms.ComboBox();
            this.btnLoad = new System.Windows.Forms.Button();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnNew = new System.Windows.Forms.Button();
            this.btnDelObject = new System.Windows.Forms.Button();
            this.btnAddObject = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.tbEditObject = new System.Windows.Forms.TextBox();
            this.lbObjects = new System.Windows.Forms.ListBox();
            this.btnLoadImage = new System.Windows.Forms.Button();
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.btnZxSplit);
            this.splitContainer1.Panel1.Controls.Add(this.cbScale);
            this.splitContainer1.Panel1.Controls.Add(this.btnLoad);
            this.splitContainer1.Panel1.Controls.Add(this.btnSave);
            this.splitContainer1.Panel1.Controls.Add(this.btnNew);
            this.splitContainer1.Panel1.Controls.Add(this.btnDelObject);
            this.splitContainer1.Panel1.Controls.Add(this.btnAddObject);
            this.splitContainer1.Panel1.Controls.Add(this.label1);
            this.splitContainer1.Panel1.Controls.Add(this.tbEditObject);
            this.splitContainer1.Panel1.Controls.Add(this.lbObjects);
            this.splitContainer1.Panel1.Controls.Add(this.btnLoadImage);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.pictureBox);
            this.splitContainer1.Size = new System.Drawing.Size(896, 518);
            this.splitContainer1.SplitterDistance = 201;
            this.splitContainer1.TabIndex = 0;
            // 
            // btnZxSplit
            // 
            this.btnZxSplit.Location = new System.Drawing.Point(3, 352);
            this.btnZxSplit.Name = "btnZxSplit";
            this.btnZxSplit.Size = new System.Drawing.Size(75, 23);
            this.btnZxSplit.TabIndex = 11;
            this.btnZxSplit.Text = "Split";
            this.btnZxSplit.UseVisualStyleBackColor = true;
            this.btnZxSplit.Click += new System.EventHandler(this.btnZxSplit_Click);
            // 
            // cbScale
            // 
            this.cbScale.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbScale.FormattingEnabled = true;
            this.cbScale.Items.AddRange(new object[] {
            "PC",
            "ZX"});
            this.cbScale.Location = new System.Drawing.Point(92, 3);
            this.cbScale.Name = "cbScale";
            this.cbScale.Size = new System.Drawing.Size(41, 21);
            this.cbScale.TabIndex = 9;
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(113, 323);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(61, 23);
            this.btnLoad.TabIndex = 8;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(46, 323);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(61, 23);
            this.btnSave.TabIndex = 7;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnNew
            // 
            this.btnNew.Location = new System.Drawing.Point(3, 323);
            this.btnNew.Name = "btnNew";
            this.btnNew.Size = new System.Drawing.Size(37, 23);
            this.btnNew.TabIndex = 6;
            this.btnNew.Text = "New";
            this.btnNew.UseVisualStyleBackColor = true;
            this.btnNew.Click += new System.EventHandler(this.btnNew_Click);
            // 
            // btnDelObject
            // 
            this.btnDelObject.Location = new System.Drawing.Point(3, 294);
            this.btnDelObject.Name = "btnDelObject";
            this.btnDelObject.Size = new System.Drawing.Size(75, 23);
            this.btnDelObject.TabIndex = 5;
            this.btnDelObject.Text = "Del";
            this.btnDelObject.UseVisualStyleBackColor = true;
            this.btnDelObject.Click += new System.EventHandler(this.btnDelObject_Click);
            // 
            // btnAddObject
            // 
            this.btnAddObject.Location = new System.Drawing.Point(177, 37);
            this.btnAddObject.Name = "btnAddObject";
            this.btnAddObject.Size = new System.Drawing.Size(17, 20);
            this.btnAddObject.TabIndex = 4;
            this.btnAddObject.Text = ">";
            this.btnAddObject.UseVisualStyleBackColor = true;
            this.btnAddObject.Click += new System.EventHandler(this.btnAddObject_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(-3, 60);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(43, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Objects";
            // 
            // tbEditObject
            // 
            this.tbEditObject.Location = new System.Drawing.Point(0, 37);
            this.tbEditObject.Name = "tbEditObject";
            this.tbEditObject.Size = new System.Drawing.Size(174, 20);
            this.tbEditObject.TabIndex = 2;
            // 
            // lbObjects
            // 
            this.lbObjects.FormattingEnabled = true;
            this.lbObjects.Location = new System.Drawing.Point(0, 76);
            this.lbObjects.Name = "lbObjects";
            this.lbObjects.Size = new System.Drawing.Size(195, 212);
            this.lbObjects.Sorted = true;
            this.lbObjects.TabIndex = 1;
            this.lbObjects.SelectedIndexChanged += new System.EventHandler(this.lbObjects_SelectedIndexChanged);
            // 
            // btnLoadImage
            // 
            this.btnLoadImage.Location = new System.Drawing.Point(0, 0);
            this.btnLoadImage.Name = "btnLoadImage";
            this.btnLoadImage.Size = new System.Drawing.Size(76, 31);
            this.btnLoadImage.TabIndex = 0;
            this.btnLoadImage.Text = "Load Image";
            this.btnLoadImage.UseVisualStyleBackColor = true;
            this.btnLoadImage.Click += new System.EventHandler(this.btnLoadImage_Click);
            // 
            // pictureBox
            // 
            this.pictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox.Location = new System.Drawing.Point(0, 0);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(691, 518);
            this.pictureBox.TabIndex = 0;
            this.pictureBox.TabStop = false;
            this.pictureBox.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pictureBox_MouseClick);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(896, 518);
            this.Controls.Add(this.splitContainer1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MainForm";
            this.Text = "ZX Color Tool";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button btnLoadImage;
        private System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.ListBox lbObjects;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbEditObject;
        private System.Windows.Forms.Button btnAddObject;
        private System.Windows.Forms.Button btnDelObject;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnNew;
        private System.Windows.Forms.ComboBox cbScale;
        private System.Windows.Forms.Button btnZxSplit;
    }
}


namespace ResourceTrans
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
            this.createRadio = new System.Windows.Forms.RadioButton();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.origRcFileTextBox = new System.Windows.Forms.TextBox();
            this.origRcFileButton = new System.Windows.Forms.Button();
            this.transRcFileTextBox = new System.Windows.Forms.TextBox();
            this.transRcFileButton = new System.Windows.Forms.Button();
            this.dstTransFileButton = new System.Windows.Forms.Button();
            this.dstTransFileTextBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.translateRadio = new System.Windows.Forms.RadioButton();
            this.srcTransFileButton = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.srcTransFileTextBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.dstRcFileButton = new System.Windows.Forms.Button();
            this.dstRcFileTextBox = new System.Windows.Forms.TextBox();
            this.goButton = new System.Windows.Forms.Button();
            this.srcRcFileTextBox = new System.Windows.Forms.TextBox();
            this.srcRcFileButton = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // createRadio
            // 
            this.createRadio.AutoSize = true;
            this.createRadio.Location = new System.Drawing.Point(12, 12);
            this.createRadio.Name = "createRadio";
            this.createRadio.Size = new System.Drawing.Size(155, 17);
            this.createRadio.TabIndex = 0;
            this.createRadio.TabStop = true;
            this.createRadio.Text = "Create translation dictionary";
            this.createRadio.UseVisualStyleBackColor = true;
            this.createRadio.CheckedChanged += new System.EventHandler(this.createRadio_CheckedChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(27, 37);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(82, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Original RC File:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 66);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(97, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Translated RC File:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // origRcFileTextBox
            // 
            this.origRcFileTextBox.Location = new System.Drawing.Point(115, 34);
            this.origRcFileTextBox.Name = "origRcFileTextBox";
            this.origRcFileTextBox.Size = new System.Drawing.Size(262, 20);
            this.origRcFileTextBox.TabIndex = 3;
            this.origRcFileTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
            // 
            // origRcFileButton
            // 
            this.origRcFileButton.Location = new System.Drawing.Point(383, 32);
            this.origRcFileButton.Name = "origRcFileButton";
            this.origRcFileButton.Size = new System.Drawing.Size(75, 23);
            this.origRcFileButton.TabIndex = 4;
            this.origRcFileButton.Text = "Browse...";
            this.origRcFileButton.UseVisualStyleBackColor = true;
            this.origRcFileButton.Click += new System.EventHandler(this.origRcFileButton_Click);
            // 
            // transRcFileTextBox
            // 
            this.transRcFileTextBox.Location = new System.Drawing.Point(115, 63);
            this.transRcFileTextBox.Name = "transRcFileTextBox";
            this.transRcFileTextBox.Size = new System.Drawing.Size(262, 20);
            this.transRcFileTextBox.TabIndex = 5;
            this.transRcFileTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
            // 
            // transRcFileButton
            // 
            this.transRcFileButton.Location = new System.Drawing.Point(383, 61);
            this.transRcFileButton.Name = "transRcFileButton";
            this.transRcFileButton.Size = new System.Drawing.Size(75, 23);
            this.transRcFileButton.TabIndex = 6;
            this.transRcFileButton.Text = "Browse...";
            this.transRcFileButton.UseVisualStyleBackColor = true;
            this.transRcFileButton.Click += new System.EventHandler(this.transRcFileButton_Click);
            // 
            // dstTransFileButton
            // 
            this.dstTransFileButton.Location = new System.Drawing.Point(383, 90);
            this.dstTransFileButton.Name = "dstTransFileButton";
            this.dstTransFileButton.Size = new System.Drawing.Size(75, 23);
            this.dstTransFileButton.TabIndex = 7;
            this.dstTransFileButton.Text = "Browse...";
            this.dstTransFileButton.UseVisualStyleBackColor = true;
            this.dstTransFileButton.Click += new System.EventHandler(this.dstTransFileButton_Click);
            // 
            // dstTransFileTextBox
            // 
            this.dstTransFileTextBox.Location = new System.Drawing.Point(115, 92);
            this.dstTransFileTextBox.Name = "dstTransFileTextBox";
            this.dstTransFileTextBox.Size = new System.Drawing.Size(262, 20);
            this.dstTransFileTextBox.TabIndex = 8;
            this.dstTransFileTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(27, 95);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(81, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Translation File:";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // translateRadio
            // 
            this.translateRadio.AutoSize = true;
            this.translateRadio.Location = new System.Drawing.Point(12, 125);
            this.translateRadio.Name = "translateRadio";
            this.translateRadio.Size = new System.Drawing.Size(106, 17);
            this.translateRadio.TabIndex = 10;
            this.translateRadio.TabStop = true;
            this.translateRadio.Text = "Translate RC File";
            this.translateRadio.UseVisualStyleBackColor = true;
            // 
            // srcTransFileButton
            // 
            this.srcTransFileButton.Location = new System.Drawing.Point(383, 145);
            this.srcTransFileButton.Name = "srcTransFileButton";
            this.srcTransFileButton.Size = new System.Drawing.Size(75, 23);
            this.srcTransFileButton.TabIndex = 11;
            this.srcTransFileButton.Text = "Browse...";
            this.srcTransFileButton.UseVisualStyleBackColor = true;
            this.srcTransFileButton.Click += new System.EventHandler(this.srcTransFileButton_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(28, 150);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(81, 13);
            this.label4.TabIndex = 12;
            this.label4.Text = "Translation File:";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // srcTransFileTextBox
            // 
            this.srcTransFileTextBox.Location = new System.Drawing.Point(115, 147);
            this.srcTransFileTextBox.Name = "srcTransFileTextBox";
            this.srcTransFileTextBox.Size = new System.Drawing.Size(262, 20);
            this.srcTransFileTextBox.TabIndex = 13;
            this.srcTransFileTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(40, 207);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(69, 13);
            this.label5.TabIndex = 14;
            this.label5.Text = "Dest RC File:";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // dstRcFileButton
            // 
            this.dstRcFileButton.Location = new System.Drawing.Point(383, 202);
            this.dstRcFileButton.Name = "dstRcFileButton";
            this.dstRcFileButton.Size = new System.Drawing.Size(75, 23);
            this.dstRcFileButton.TabIndex = 15;
            this.dstRcFileButton.Text = "Browse...";
            this.dstRcFileButton.UseVisualStyleBackColor = true;
            this.dstRcFileButton.Click += new System.EventHandler(this.dstRcFileButton_Click);
            // 
            // dstRcFileTextBox
            // 
            this.dstRcFileTextBox.Location = new System.Drawing.Point(115, 204);
            this.dstRcFileTextBox.Name = "dstRcFileTextBox";
            this.dstRcFileTextBox.Size = new System.Drawing.Size(262, 20);
            this.dstRcFileTextBox.TabIndex = 16;
            this.dstRcFileTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
            // 
            // goButton
            // 
            this.goButton.Location = new System.Drawing.Point(383, 235);
            this.goButton.Name = "goButton";
            this.goButton.Size = new System.Drawing.Size(75, 23);
            this.goButton.TabIndex = 17;
            this.goButton.Text = "Go";
            this.goButton.UseVisualStyleBackColor = true;
            this.goButton.Click += new System.EventHandler(this.goButton_Click);
            // 
            // srcRcFileTextBox
            // 
            this.srcRcFileTextBox.Location = new System.Drawing.Point(115, 175);
            this.srcRcFileTextBox.Name = "srcRcFileTextBox";
            this.srcRcFileTextBox.Size = new System.Drawing.Size(262, 20);
            this.srcRcFileTextBox.TabIndex = 20;
            this.srcRcFileTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
            // 
            // srcRcFileButton
            // 
            this.srcRcFileButton.Location = new System.Drawing.Point(383, 173);
            this.srcRcFileButton.Name = "srcRcFileButton";
            this.srcRcFileButton.Size = new System.Drawing.Size(75, 23);
            this.srcRcFileButton.TabIndex = 19;
            this.srcRcFileButton.Text = "Browse...";
            this.srcRcFileButton.UseVisualStyleBackColor = true;
            this.srcRcFileButton.Click += new System.EventHandler(this.srcRcFileButton_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(27, 178);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(81, 13);
            this.label6.TabIndex = 18;
            this.label6.Text = "Source RC File:";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(470, 270);
            this.Controls.Add(this.srcRcFileTextBox);
            this.Controls.Add(this.srcRcFileButton);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.goButton);
            this.Controls.Add(this.dstRcFileTextBox);
            this.Controls.Add(this.dstRcFileButton);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.srcTransFileTextBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.srcTransFileButton);
            this.Controls.Add(this.translateRadio);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.dstTransFileTextBox);
            this.Controls.Add(this.dstTransFileButton);
            this.Controls.Add(this.transRcFileButton);
            this.Controls.Add(this.transRcFileTextBox);
            this.Controls.Add(this.origRcFileButton);
            this.Controls.Add(this.origRcFileTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.createRadio);
            this.Name = "MainForm";
            this.Text = "Resource Translator";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RadioButton createRadio;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox origRcFileTextBox;
        private System.Windows.Forms.Button origRcFileButton;
        private System.Windows.Forms.TextBox transRcFileTextBox;
        private System.Windows.Forms.Button transRcFileButton;
        private System.Windows.Forms.Button dstTransFileButton;
        private System.Windows.Forms.TextBox dstTransFileTextBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.RadioButton translateRadio;
        private System.Windows.Forms.Button srcTransFileButton;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox srcTransFileTextBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button dstRcFileButton;
        private System.Windows.Forms.TextBox dstRcFileTextBox;
        private System.Windows.Forms.Button goButton;
        private System.Windows.Forms.TextBox srcRcFileTextBox;
        private System.Windows.Forms.Button srcRcFileButton;
        private System.Windows.Forms.Label label6;
    }
}


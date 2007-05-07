namespace ResourceTrans
{
    partial class MismatchForm
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
			this.origListBox = new System.Windows.Forms.CheckedListBox();
			this.transListBox = new System.Windows.Forms.CheckedListBox();
			this.okButton = new System.Windows.Forms.Button();
			this.cancelButton = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// origListBox
			// 
			this.origListBox.CheckOnClick = true;
			this.origListBox.FormattingEnabled = true;
			this.origListBox.Location = new System.Drawing.Point(12, 25);
			this.origListBox.Name = "origListBox";
			this.origListBox.Size = new System.Drawing.Size(266, 244);
			this.origListBox.TabIndex = 0;
			this.origListBox.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.listBox_ItemCheck);
			// 
			// transListBox
			// 
			this.transListBox.CheckOnClick = true;
			this.transListBox.FormattingEnabled = true;
			this.transListBox.Location = new System.Drawing.Point(284, 25);
			this.transListBox.Name = "transListBox";
			this.transListBox.Size = new System.Drawing.Size(266, 244);
			this.transListBox.TabIndex = 1;
			this.transListBox.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.listBox_ItemCheck);
			// 
			// okButton
			// 
			this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.okButton.Location = new System.Drawing.Point(475, 275);
			this.okButton.Name = "okButton";
			this.okButton.Size = new System.Drawing.Size(75, 23);
			this.okButton.TabIndex = 2;
			this.okButton.Text = "OK";
			this.okButton.UseVisualStyleBackColor = true;
			this.okButton.Click += new System.EventHandler(this.okButton_Click);
			// 
			// cancelButton
			// 
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(394, 275);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(75, 23);
			this.cancelButton.TabIndex = 3;
			this.cancelButton.Text = "Cancel";
			this.cancelButton.UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(471, 13);
			this.label1.TabIndex = 4;
			this.label1.Text = "Please uncheck the items on either side that don\'t have a corresponding item on t" +
				"he opposite side.";
			// 
			// MismatchForm
			// 
			this.AcceptButton = this.okButton;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.cancelButton;
			this.ClientSize = new System.Drawing.Size(562, 310);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.cancelButton);
			this.Controls.Add(this.okButton);
			this.Controls.Add(this.transListBox);
			this.Controls.Add(this.origListBox);
			this.Name = "MismatchForm";
			this.Text = "MismatchForm";
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckedListBox origListBox;
        private System.Windows.Forms.CheckedListBox transListBox;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
		private System.Windows.Forms.Label label1;
    }
}
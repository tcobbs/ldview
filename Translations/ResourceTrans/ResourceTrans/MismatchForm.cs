using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections.Specialized;

namespace ResourceTrans
{
    public partial class MismatchForm : Form
    {
        private int origCheckedCount;
        private int transCheckedCount;
        private bool doneInitializing = false;
        private StringCollection newStrings = null;
        private StringCollection newTransStrings = null;

        private void updateEnabled()
        {
            bool enabled = false;

            if (origCheckedCount == transCheckedCount)
            {
                enabled = true;
            }
            okButton.Enabled = enabled;
        }

        public MismatchForm(StringCollection origStrings, StringCollection transStrings)
        {
            InitializeComponent();
            for (int i = 0; i < origStrings.Count; i++)
            {
                origListBox.Items.Add(origStrings[i], true);
            }
            for (int i = 0; i < transStrings.Count; i++)
            {
                transListBox.Items.Add(transStrings[i], true);
            }
            origCheckedCount = origStrings.Count;
            transCheckedCount = transStrings.Count;
            doneInitializing = true;
            updateEnabled();
        }

        public bool getStrings(out StringCollection orig, out StringCollection trans)
        {
            if (newStrings != null && newTransStrings != null)
            {
                orig = newStrings;
                trans = newTransStrings;
                return true;
            }
            else
            {
                orig = null;
                trans = null;
                return false;
            }
        }

        private void getStrings(CheckedListBox listBox, ref StringCollection strings)
        {
            int i;

            for (i = 0; i < listBox.Items.Count; i++)
            {
                if (listBox.GetItemChecked(i))
                {
					String itemText = listBox.Items[i].ToString();
					strings.Add(itemText);
                }
            }
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            newStrings = new StringCollection();
            newTransStrings = new StringCollection();
            getStrings(origListBox, ref newStrings);
            getStrings(transListBox, ref newTransStrings);
        }

        private void listBox_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (doneInitializing)
            {
                int delta = -1;

                if (e.NewValue == CheckState.Checked)
                {
                    delta = 1;
                }
                if (sender == origListBox)
                {
                    origCheckedCount += delta;
                }
                else
                {
                    transCheckedCount += delta;
                }
                updateEnabled();
            }
        }
    }
}
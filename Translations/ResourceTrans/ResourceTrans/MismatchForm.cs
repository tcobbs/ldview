using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections.Specialized;

using StringList = System.Collections.Generic.List<string>;

namespace ResourceTrans
{
    public partial class MismatchForm : Form
    {
        private int origCheckedCount;
        private int transCheckedCount;
        private bool doneInitializing = false;
        private StringList newStrings = null;
        private StringList newTransStrings = null;

        private void updateEnabled()
        {
            bool enabled = false;

            if (origCheckedCount == transCheckedCount)
            {
                enabled = true;
            }
            okButton.Enabled = enabled;
        }

        public MismatchForm(StringList origStrings, StringList transStrings)
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

        public bool getStrings(out StringList orig, out StringList trans)
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

        private void getStrings(CheckedListBox listBox, ref StringList strings)
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
            newStrings = new StringList();
            newTransStrings = new StringList();
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
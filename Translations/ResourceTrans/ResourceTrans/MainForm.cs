using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Collections.Specialized;

using StringDict = System.Collections.Generic.Dictionary<System.String, System.String>;
using DlgDict = System.Collections.Generic.Dictionary<System.String, System.Collections.Generic.Dictionary<System.String, System.String>>;

namespace ResourceTrans
{
	public partial class MainForm : Form
	{
		public MainForm()
		{
			InitializeComponent();
		}

		private void enableCreateFields(bool enabled)
		{
			origRcFileTextBox.Enabled = enabled;
			origRcFileButton.Enabled = enabled;
			transRcFileTextBox.Enabled = enabled;
			transRcFileButton.Enabled = enabled;
			dstTransFileTextBox.Enabled = enabled;
			dstTransFileButton.Enabled = enabled;
		}

		private void enableTranslateFields(bool enabled)
		{
			srcTransFileTextBox.Enabled = enabled;
			srcTransFileButton.Enabled = enabled;
			srcRcFileTextBox.Enabled = enabled;
			srcRcFileButton.Enabled = enabled;
			dstRcFileTextBox.Enabled = enabled;
			dstRcFileButton.Enabled = enabled;
		}

		private void updateEnabled()
		{
			bool goEnabled = false;

			if (createRadio.Checked)
			{
				enableCreateFields(true);
				enableTranslateFields(false);
				if (origRcFileTextBox.Text.Length > 0 &&
					transRcFileTextBox.Text.Length > 0 &&
					dstTransFileTextBox.Text.Length > 0)
				{
					goEnabled = true;
				}
			}
			else
			{
				enableCreateFields(false);
				enableTranslateFields(true);
				if (srcTransFileTextBox.Text.Length > 0 &&
					srcRcFileTextBox.Text.Length > 0 &&
					dstRcFileTextBox.Text.Length > 0)
				{
					goEnabled = true;
				}
			}
			goButton.Enabled = goEnabled;
		}

		private void browseInput(ref TextBox textBox, String filter)
		{
			OpenFileDialog ofd = new OpenFileDialog();

			ofd.Filter = filter;
			ofd.FilterIndex = 1;
			if (ofd.ShowDialog() == DialogResult.OK)
			{
				textBox.Text = ofd.FileName;
			}
		}

		private void browseOutput(ref TextBox textBox, String filter)
		{
			SaveFileDialog sfd = new SaveFileDialog();

			sfd.Filter = filter;
			sfd.FilterIndex = 1;
			if (sfd.ShowDialog() == DialogResult.OK)
			{
				textBox.Text = sfd.FileName;
			}
		}

		private String getDialogName(String line)
		{
			int loc;

			if ((loc = line.IndexOf(" DIALOG ")) != -1 ||
				(loc = line.IndexOf(" DIALOGEX ")) != -1)
			{
				return line.Substring(0, loc);
			}
			else
			{
				return null;
			}
		}

		private String findDialog(StreamReader reader)
		{
			String line;

			while ((line = reader.ReadLine()) != null)
			{
				String name = getDialogName(line);
				if (name != null)
				{
					return name;
				}
			}
			return null;
		}

		private String findString(String line)
		{
			int openQuoteLoc = line.IndexOf('"');

			if (openQuoteLoc != -1)
			{
				int closeQuoteLoc = -1;
				int startLoc = openQuoteLoc + 1;

				while (closeQuoteLoc == -1)
				{
					closeQuoteLoc = line.IndexOf('"', startLoc);

					if (closeQuoteLoc == -1)
					{
						break;
					}
					else if (line[closeQuoteLoc - 1] == '\\')
					{
						closeQuoteLoc = -1;
					}
				}
				if (closeQuoteLoc != -1)
				{
					return line.Substring(openQuoteLoc + 1, closeQuoteLoc - openQuoteLoc - 1);
				}
			}
			return null;
		}

		private void scanStrings(StreamReader reader, String prefix, ref StringCollection strings)
		{
			String line;

			if (strings == null)
			{
				strings = new StringCollection();
			}
			while ((line = reader.ReadLine()) != null)
			{
				String foundString;

				if (line == "END")
				{
					return;
				}
				else if ((foundString = findString(line)) != null)
				{
					if (foundString != "Button")
					{
						strings.Add(prefix + ":" + foundString);
					}
				}
			}
		}

		private void scanRc(StreamReader reader, ref StringCollection strings)
		{
			String dialogName;

			while ((dialogName = findDialog(reader)) != null)
			{
				scanStrings(reader, dialogName, ref strings);
			}
		}

		private void writeTransFile(StreamWriter writer, StringCollection origStrings, StringCollection transStrings, int dstEncoding)
		{
			int origIndex = 0;
			int transIndex = 0;
			StringCollection skippedDialogs = new StringCollection();

			writer.WriteLine(dstEncoding);
			while (origIndex < origStrings.Count && transIndex < transStrings.Count)
			{
				String origString = origStrings[origIndex];
				String dialogName = origString.Substring(0, origString.IndexOf(':') + 1);
				int origDialogCount = 0;
				int transDialogCount = 0;

				while (origIndex + origDialogCount < origStrings.Count &&
					origStrings[origIndex + origDialogCount].StartsWith(dialogName))
				{
					origDialogCount++;
				}
				while (transIndex + transDialogCount < transStrings.Count &&
					transStrings[transIndex + transDialogCount].StartsWith(dialogName))
				{
					transDialogCount++;
				}
				if (origDialogCount == transDialogCount)
				{
					int i;

					for (i = 0; i < origDialogCount; i++)
					{
						writer.WriteLine("O:" + origStrings[origIndex + i]);
						writer.WriteLine("T:" + transStrings[transIndex + i]);
					}
				}
				else
				{
					StringCollection origDialogStrings = new StringCollection();
					StringCollection transDialogStrings = new StringCollection();
					int i;

					for (i = 0; i < origDialogCount; i++)
					{
						origDialogStrings.Add(origStrings[origIndex + i]);
					}
					for (i = 0; i < transDialogCount; i++)
					{
						transDialogStrings.Add(transStrings[transIndex + i]);
					}
					MismatchForm mismatchForm = new MismatchForm(origDialogStrings, transDialogStrings);
					bool skippedDialog = true;
					if (mismatchForm.ShowDialog() == DialogResult.OK)
					{
						StringCollection origDlgStrings;
						StringCollection transDlgStrings;

						if (mismatchForm.getStrings(out origDlgStrings, out transDlgStrings))
						{
							for (i = 0; i < origDlgStrings.Count; i++)
							{
								writer.WriteLine("O:" + origDlgStrings[i]);
								writer.WriteLine("T:" + transDlgStrings[i]);
							}
							skippedDialog = false;
						}
					}
					if (skippedDialog)
					{
						skippedDialogs.Add(dialogName);
					}
				}
				origIndex += origDialogCount;
				transIndex += transDialogCount;
			}
			if (skippedDialogs.Count > 0)
			{
				String message = "The following dialog";

				if (skippedDialogs.Count > 1)
				{
					message += "s";
				}
				message += "won't be translated due to an unhandled mismatch in the string count:\n";
				for (int i = 0; i < skippedDialogs.Count; i++)
				{
					message += "\n" + skippedDialogs[i];
				}
				MessageBox.Show(message, "Resource Translator", MessageBoxButtons.OK, MessageBoxIcon.Warning);
			}
		}

		private int fixEncoding(ref StreamReader reader, String filename)
		{
			String line;
			int retValue = -1;

			while ((line = reader.ReadLine()) != null && retValue == -1)
			{
				if (line.StartsWith("#pragma code_page("))
				{
					int codePage;
					String codePageString = line.Substring(18);

					if (codePageString.IndexOf(')') != -1)
					{
						codePageString = codePageString.Substring(0, codePageString.IndexOf(')'));
					}
					if (int.TryParse(codePageString, out codePage))
					{
						retValue = codePage;
					}
				}
			}
			if (retValue == -1)
			{
				reader = new StreamReader(filename);
			}
			else
			{
				reader = new StreamReader(filename, System.Text.Encoding.GetEncoding(retValue));
			}
			return retValue;
		}

		private void createTranslation()
		{
			StreamReader origRcReader = null;
			StreamReader transRcReader = null;
			StreamWriter transWriter = null;
			String action = "reading";
			String filename = "";
			StringCollection origStrings = null;
			StringCollection transStrings = null;

			try
			{
				filename = origRcFileTextBox.Text;
				origRcReader = new StreamReader(filename);
				filename = transRcFileTextBox.Text;
				transRcReader = new StreamReader(filename);
				action = "writing to";
				filename = dstTransFileTextBox.Text;
				transWriter = new StreamWriter(filename, false, System.Text.Encoding.Unicode);
				action = "parsing";
				filename = origRcFileTextBox.Text;
				fixEncoding(ref origRcReader, filename);
				scanRc(origRcReader, ref origStrings);
				filename = transRcFileTextBox.Text;
				int dstEncoding = fixEncoding(ref transRcReader, filename);
				scanRc(transRcReader, ref transStrings);
				action = "creating translation";
				filename = dstTransFileTextBox.Text;
				writeTransFile(transWriter, origStrings, transStrings, dstEncoding);
			}
			catch
			{
				MessageBox.Show(this, "Error " + action + " file " + filename + ".",
					"Resource Translator", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
			}
			if (transWriter != null)
			{
				transWriter.Close();
			}
		}

		private String[] splitTransLine(String line)
		{
			int colonIndex = line.IndexOf(':', 2);

			if (colonIndex > 1)
			{
				String[] retValue = new String[3];

				retValue[0] = line.Substring(0, 1);
				retValue[1] = line.Substring(2, colonIndex - 2);
				retValue[2] = line.Substring(colonIndex + 1);
				return retValue;
			}
			else
			{
				return null;
			}
		}

		private DlgDict readTrans(StreamReader transReader)
		{
			String lineO;
			String lineT;
			DlgDict retValue = new DlgDict();

			while ((lineO = transReader.ReadLine()) != null)
			{
				lineT = transReader.ReadLine();
				if (lineT != null)
				{
					String[] partsO = splitTransLine(lineO);
					String[] partsT = splitTransLine(lineT);
					if (partsO != null && partsT != null)
					{
						if (!retValue.ContainsKey(partsO[1]))
						{
							retValue.Add(partsO[1], new StringDict());
						}
						StringDict stringDict = retValue[partsO[1]];
						if (stringDict.ContainsKey(partsO[2]))
						{
							stringDict[partsO[2]] = partsT[2];
						}
						else
						{
							stringDict.Add(partsO[2], partsT[2]);
						}
					}
				}
			}
			return retValue;
		}

		private void translateRc(StreamReader transReader, StreamReader srcRcReader, StreamWriter dstRcWriter, int codePage)
		{
			DlgDict dlgDict = readTrans(transReader);
			String srcLine;
			String currentDlg = null;
			StringDict dlgStrings = null;

			while ((srcLine = srcRcReader.ReadLine()) != null)
			{
				String dlgName = getDialogName(srcLine);
				bool wrote = false;

				if (dlgName != null)
				{
					currentDlg = dlgName;
					if (!dlgDict.TryGetValue(dlgName, out dlgStrings))
					{
						dlgStrings = null;
					}
				}
				else
				{
					if (srcLine == "END")
					{
						currentDlg = null;
						dlgStrings = null;
					}
					else if (srcLine.StartsWith("#pragma code_page("))
					{
						if (codePage != -1)
						{
							dstRcWriter.WriteLine("#pragma code_page(" + codePage.ToString() + ")");
							wrote = true;
						}
					}
					else
					{
						if (currentDlg != null && dlgStrings != null)
						{
							String foundString = findString(srcLine);
							String transString = null;

							if (foundString != null && foundString.Length > 0 &&
								dlgStrings.TryGetValue(foundString, out transString))
							{
								dstRcWriter.WriteLine(srcLine.Replace(foundString, transString));
								wrote = true;
							}
						}
					}
				}
				if (!wrote)
				{
					dstRcWriter.WriteLine(srcLine);
				}
			}
		}

		private void translateRc()
		{
			StreamReader srcRcReader = null;
			StreamReader transReader = null;
			StreamWriter dstRcWriter = null;
			String action = "reading";
			String filename = "";

			try
			{
				String line;
				Encoding dstEncoding = Encoding.Default;
				int codePage = -1;

				filename = srcRcFileTextBox.Text;
				srcRcReader = new StreamReader(filename, dstEncoding);
				filename = srcTransFileTextBox.Text;
				transReader = new StreamReader(filename);
				action = "parsing";
				if ((line = transReader.ReadLine()) != null)
				{
					if (int.TryParse(line, out codePage))
					{
						dstEncoding = Encoding.GetEncoding(codePage);
					}
					else
					{
						codePage = -1;
					}
				}
				action = "writing to";
				filename = dstRcFileTextBox.Text;
				dstRcWriter = new StreamWriter(filename, false, dstEncoding);
				translateRc(transReader, srcRcReader, dstRcWriter, codePage);
			}
			catch
			{
				MessageBox.Show(this, "Error " + action + " file " + filename + ".",
					"Resource Translator", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
			}
			if (dstRcWriter != null)
			{
				dstRcWriter.Close();
			}
		}

		private void createRadio_CheckedChanged(object sender, EventArgs e)
		{
			updateEnabled();
		}

		private void textBox_TextChanged(object sender, EventArgs e)
		{
			updateEnabled();
		}

		private void origRcFileButton_Click(object sender, EventArgs e)
		{
			browseInput(ref origRcFileTextBox, "RC Files (*.rc)|*.rc|All Files (*.*)|*.*");
			updateEnabled();
		}

		private void transRcFileButton_Click(object sender, EventArgs e)
		{
			browseInput(ref transRcFileTextBox, "RC Files (*.rc)|*.rc|All Files (*.*)|*.*");
			updateEnabled();
		}

		private void dstTransFileButton_Click(object sender, EventArgs e)
		{
			browseOutput(ref dstTransFileTextBox, "Translation Files (*.trans)|*.trans|All Files (*.*)|*.*");
			updateEnabled();
		}

		private void srcTransFileButton_Click(object sender, EventArgs e)
		{
			browseInput(ref srcTransFileTextBox, "Translation Files (*.trans)|*.trans|All Files (*.*)|*.*");
			updateEnabled();
		}

		private void srcRcFileButton_Click(object sender, EventArgs e)
		{
			browseInput(ref srcRcFileTextBox, "RC Files (*.rc)|*.rc|All Files (*.*)|*.*");
			updateEnabled();
		}

		private void dstRcFileButton_Click(object sender, EventArgs e)
		{
			browseOutput(ref dstRcFileTextBox, "RC Files (*.rc)|*.rc|All Files (*.*)|*.*");
			updateEnabled();
		}

		private void goButton_Click(object sender, EventArgs e)
		{
			if (createRadio.Checked)
			{
				createTranslation();
			}
			else if (translateRadio.Checked)
			{
				translateRc();
			}
		}
	}
}
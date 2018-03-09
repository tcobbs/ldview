using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
//using System.Collections.Specialized;

using StringDict = System.Collections.Generic.Dictionary<System.String, System.String>;
using DlgDict = System.Collections.Generic.Dictionary<System.String, System.Collections.Generic.Dictionary<System.String, System.String>>;
using AccelDict = System.Collections.Generic.Dictionary<string, System.Collections.Generic.Dictionary<string, string[]>>;
using StringList = System.Collections.Generic.List<string>;

namespace ResourceTrans
{
	public partial class MainForm : Form
	{
		enum SectionType
		{
			Unknown,
			Dialog,
			Menu,
			Accelerators
		}

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

		private String getSectionName(String line, out SectionType sectionType)
		{
			int loc;

			if ((loc = line.IndexOf(" DIALOG ")) != -1 ||
				(loc = line.IndexOf(" DIALOGEX ")) != -1)
			{
				sectionType = SectionType.Dialog;
			}
			else if ((loc = line.IndexOf(" MENU ")) != -1)
			{
				sectionType = SectionType.Menu;
			}
			else if ((loc = line.IndexOf(" ACCELERATORS ")) != -1)
			{
				sectionType = SectionType.Accelerators;
			}
			else
			{
				sectionType = SectionType.Unknown;
				return null;
			}
			return line.Substring(0, loc);
		}

		private String findSection(StreamReader reader, out SectionType sectionType)
		{
			String line;

			while ((line = reader.ReadLine()) != null)
			{
				String name = getSectionName(line, out sectionType);
				if (name != null)
				{
					return name;
				}
			}
			sectionType = SectionType.Unknown;
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

		private string findId(String input, ref int loc)
		{
			while (loc < input.Length && (input[loc] == ' ' || input[loc] == '\t'))
			{
				loc++;
			}
			if (loc < input.Length)
			{
				int startIndex = loc;

				while (loc < input.Length && input[loc] != ' ' && input[loc] != '\t' && input[loc] != ',')
				{
					loc++;
				}
				return input.Substring(startIndex, loc - startIndex);
			}
			return null;
		}

		private void scanStrings(StreamReader reader, String prefix, ref StringList strings, SectionType sectionType)
		{
			String line;

			if (strings == null)
			{
				strings = new StringList();
			}
			while ((line = reader.ReadLine()) != null)
			{
				String foundString;

				if (line == "END")
				{
					return;
				}
				else if (sectionType == SectionType.Accelerators)
				{
					String accelChar;
					int loc = 0;

					if ((accelChar = findId(line, ref loc)) != null && loc < line.Length && line[loc] == ',')
					{
						int idLoc = loc + 1;
						String accelId = findId(line, ref idLoc);

						if (accelId != null)
						{
							int modsLoc = idLoc + 1;
							String accelMods = line.Substring(modsLoc).Trim();

							while (accelMods[accelMods.Length - 1] == ',')
							{
								accelMods += " ";
								line = reader.ReadLine();
								if (line == null)
								{
									return;
								}
								line = line.Trim();
								accelMods += line;
							}
							strings.Add(prefix + accelId + ":" + accelMods + ":" + accelChar);
						}
					}
				}
				else if ((foundString = findString(line)) != null)
				{
					if (foundString != "Button")
					{
						strings.Add(prefix + foundString);
					}
				}
			}
		}

		private String SectionTypeChar(SectionType sectionType)
		{
			switch (sectionType)
			{
				case SectionType.Dialog:
					return "D";
				case SectionType.Menu:
					return "M";
				case SectionType.Accelerators:
					return "A";
				default:
					return "U";
			}
		}

		private void scanRc(StreamReader reader, ref StringList strings, String sourceChar)
		{
			String sectionName;
			SectionType sectionType;

			while ((sectionName = findSection(reader, out sectionType)) != null)
			{
				scanStrings(reader, SectionTypeChar(sectionType) + sourceChar + ":" + sectionName + ":", ref strings, sectionType);
			}
		}

		private void AddAccel(ref AccelDict accelDict, String[] parts)
		{
			int offset = 0;

			if (parts.Length == 5)
			{
				offset = 1;
			}
			if (!accelDict.ContainsKey(parts[offset]))
			{
				accelDict.Add(parts[offset], new Dictionary<string, string[]>());
			}
			if (!accelDict[parts[offset]].ContainsKey(parts[1]))
			{
				accelDict[parts[offset]].Add(parts[offset + 1], parts);
			}
		}

		private AccelDict buildAccelDict(StringList strings)
		{
			AccelDict retValue = new AccelDict();

			for (int i = 0; i < strings.Count; i++)
			{
				String accel = strings[i];

				if (accel.StartsWith("A"))
				{
					int lastColon = 2;
					bool failed = false;
					String[] parts = new String[4];

					for (int j = 0; j < 3 && !failed; j++)
					{
						int colon = accel.IndexOf(':', lastColon + 1);

						if (colon == -1)
						{
							failed = true;
						}
						else
						{
							parts[j] = accel.Substring(lastColon + 1, colon - lastColon - 1);
							lastColon = colon;
						}
					}
					if (!failed)
					{
						parts[3] = accel.Substring(lastColon + 1);
						AddAccel(ref retValue, parts);
					}
				}
			}
			return retValue;
		}

		private void writeAccel(StreamWriter writer, AccelDict origAccelDict, AccelDict transAccelDict)
		{
			foreach (KeyValuePair<string, Dictionary<string, string[]>> pair1 in origAccelDict)
			{
				if (transAccelDict.ContainsKey(pair1.Key))
				{
					foreach (KeyValuePair<string, string[]> pair2 in pair1.Value)
					{
						if (transAccelDict[pair1.Key].ContainsKey(pair2.Key))
						{
							writer.WriteLine("AO:" + String.Join(":", pair2.Value));
							writer.WriteLine("AT:" + String.Join(":", transAccelDict[pair1.Key][pair2.Key]));
						}
					}
				}
			}
		}

        private int findDialog(string dialogName, StringList strings)
        {
            int i;

            for (i = 0; i < strings.Count; i++)
            {
                string line = strings[i];
                if (!line.StartsWith("A"))
                {
                    String lineDialogName = line.Substring(3, line.IndexOf(':', 3) - 3);
                    if (lineDialogName == dialogName)
                    {
                        return i;
                    }
                }
            }
            return -1;
        }

        private void writeTransFile(StreamWriter writer, StringList origStrings, StringList transStrings, int dstEncoding)
		{
			int origIndex = 0;
			int transIndex = 0;
			StringList skippedDialogs = new StringList();
			AccelDict origAccelDict = buildAccelDict(origStrings);
			AccelDict transAccelDict = buildAccelDict(transStrings);

			writer.WriteLine(dstEncoding);
			writeAccel(writer, origAccelDict, transAccelDict);
			while (origIndex < origStrings.Count && transIndex < transStrings.Count)
			{
				while (origIndex < origStrings.Count && origStrings[origIndex].StartsWith("A"))
				{
					origIndex++;
				}
				if (origIndex >= origStrings.Count)
				{
					break;
				}
				while (transIndex < transStrings.Count && transStrings[transIndex].StartsWith("A"))
				{
					transIndex++;
				}
				if (transIndex >= transStrings.Count)
				{
					break;
				}
				String origString = origStrings[origIndex];
				String dialogName = origString.Substring(3, origString.IndexOf(':', 3) - 3);
				String startCheck = origString.Substring(0, 3) + dialogName;
				int origDialogCount = 0;
				int transDialogCount = 0;
                int newTransIndex = findDialog(dialogName, transStrings);

				while (origIndex + origDialogCount < origStrings.Count &&
					origStrings[origIndex + origDialogCount].StartsWith(startCheck))
				{
					origDialogCount++;
				}
                if (newTransIndex >= 0)
                {
                    startCheck = transStrings[newTransIndex].Substring(0, 3) + dialogName;
                    while (newTransIndex + transDialogCount < transStrings.Count &&
                        transStrings[newTransIndex + transDialogCount].StartsWith(startCheck))
                    {
                        transDialogCount++;
                    }
                }
				if (origDialogCount == transDialogCount)
				{
					int i;

					for (i = 0; i < origDialogCount; i++)
					{
						writer.WriteLine(origStrings[origIndex + i]);
						writer.WriteLine(transStrings[newTransIndex + i]);
					}
				}
				else
				{
					StringList origDialogStrings = new StringList();
					StringList transDialogStrings = new StringList();
					int i;

					for (i = 0; i < origDialogCount; i++)
					{
						origDialogStrings.Add(origStrings[origIndex + i]);
					}
					for (i = 0; i < transDialogCount; i++)
					{
						transDialogStrings.Add(transStrings[newTransIndex + i]);
					}
					MismatchForm mismatchForm = new MismatchForm(origDialogStrings, transDialogStrings);
					bool skippedDialog = true;
					if (mismatchForm.ShowDialog() == DialogResult.OK)
					{
						StringList origDlgStrings;
						StringList transDlgStrings;

						if (mismatchForm.getStrings(out origDlgStrings, out transDlgStrings))
						{
							for (i = 0; i < origDlgStrings.Count; i++)
							{
								writer.WriteLine(origDlgStrings[i]);
								writer.WriteLine(transDlgStrings[i]);
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
				MessageBox.Show(this, message, "Resource Translator", MessageBoxButtons.OK, MessageBoxIcon.Warning);
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
			StringList origStrings = null;
			StringList transStrings = null;

			try
			{
				filename = origRcFileTextBox.Text;
				origRcReader = new StreamReader(filename);
				filename = transRcFileTextBox.Text;
				transRcReader = new StreamReader(filename);
				action = "writing to";
				filename = dstTransFileTextBox.Text;
				transWriter = new StreamWriter(filename, false, System.Text.Encoding.UTF8);
				action = "parsing";
				filename = origRcFileTextBox.Text;
				fixEncoding(ref origRcReader, filename);
				scanRc(origRcReader, ref origStrings, "O");
				filename = transRcFileTextBox.Text;
				int dstEncoding = fixEncoding(ref transRcReader, filename);
				scanRc(transRcReader, ref transStrings, "T");
				action = "creating translation";
				filename = dstTransFileTextBox.Text;
				writeTransFile(transWriter, origStrings, transStrings, dstEncoding);
				MessageBox.Show(this, "Translation file created successfully.");
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
			int colonIndex = line.IndexOf(':', 3);
			bool isAccel = false;

			if (line.StartsWith("A"))
			{
				isAccel = true;
			}
			if (colonIndex > 1)
			{
				String[] retValue = new String[isAccel ? 5 : 3];

				retValue[0] = line.Substring(0, 2);
				retValue[1] = line.Substring(3, colonIndex - 3);
				if (isAccel)
				{
					int colonIndex2 = line.IndexOf(':', colonIndex + 1);
					retValue[2] = line.Substring(colonIndex + 1, colonIndex2 - colonIndex - 1);
					colonIndex = colonIndex2;
					colonIndex2 = line.IndexOf(':', colonIndex + 1);
					retValue[3] = line.Substring(colonIndex + 1, colonIndex2 - colonIndex - 1);
					retValue[4] = line.Substring(colonIndex2 + 1);
				}
				else
				{
					retValue[2] = line.Substring(colonIndex + 1);
				}
				return retValue;
			}
			else
			{
				return null;
			}
		}

		private void readTrans(StreamReader transReader, out DlgDict dlgDict, out StringDict globalDict, out StringDict ambigDict, out AccelDict accelDict)
		{
			String lineO;
			String lineT;

			dlgDict = new DlgDict();
			globalDict = new StringDict();
			ambigDict = new StringDict();
			accelDict = new AccelDict();
			while ((lineO = transReader.ReadLine()) != null)
			{
				lineT = transReader.ReadLine();
				if (lineT != null)
				{
					String[] partsO = splitTransLine(lineO);
					String[] partsT = splitTransLine(lineT);

					if (partsO != null && partsT != null)
					{
						if (partsO[0].StartsWith("A"))
						{
							AddAccel(ref accelDict, partsT);
						}
						else
						{
							if (!dlgDict.ContainsKey(partsO[1]))
							{
								dlgDict.Add(partsO[1], new StringDict());
							}
							StringDict stringDict = dlgDict[partsO[1]];
							if (stringDict.ContainsKey(partsO[2]))
							{
								stringDict[partsO[2]] = partsT[2];
							}
							else
							{
								stringDict.Add(partsO[2], partsT[2]);
							}
							if (globalDict.ContainsKey(partsO[2]))
							{
								if (globalDict[partsO[2]] != partsT[2])
								{
									if (!ambigDict.ContainsKey(partsO[2]))
									{
										ambigDict.Add(partsO[2], globalDict[partsO[2]]);
									}
								}
							}
							else
							{
								globalDict.Add(partsO[2], partsT[2]);
							}
						}
					}
				}
			}
		}

		private void translateRc(StreamReader transReader, StreamReader srcRcReader, StreamWriter dstRcWriter, int codePage)
		{
			DlgDict dlgDict;
			StringDict globalDict;
			StringDict ambigDict;
			AccelDict accelDict;
			String srcLine;
			String currentSection = null;
			SectionType currentSectionType = SectionType.Unknown;
			StringDict dlgStrings = null;
			Dictionary<string, string[]> currentAccelDict = null;

			readTrans(transReader, out dlgDict, out globalDict, out ambigDict, out accelDict);
			while ((srcLine = srcRcReader.ReadLine()) != null)
			{
				SectionType sectionType;
				String sectionName = getSectionName(srcLine, out sectionType);
				bool wrote = false;

				if (sectionName != null)
				{
					currentSection = sectionName;
					currentSectionType = sectionType;
					if (currentSectionType == SectionType.Accelerators)
					{
						if (!accelDict.TryGetValue(sectionName, out currentAccelDict))
						{
							currentAccelDict = null;
						}
					}
					else
					{
						if (!dlgDict.TryGetValue(sectionName, out dlgStrings))
						{
							dlgStrings = null;
						}
					}
				}
				else
				{
					if (srcLine == "END")
					{
						currentSection = null;
						dlgStrings = null;
						currentAccelDict = null;
					}
					else if (srcLine.StartsWith("#pragma code_page("))
					{
						if (codePage != -1)
						{
							dstRcWriter.WriteLine("#pragma code_page(" + codePage.ToString() + ")");
							wrote = true;
						}
					}
					else if (currentSection != null)
					{
						if (currentSectionType == SectionType.Accelerators)
						{
							String accelChar;
							int loc = 0;

							if ((accelChar = findId(srcLine, ref loc)) != null && loc < srcLine.Length && srcLine[loc] == ',')
							{
								int idLoc = loc + 1;
								String accelId = findId(srcLine, ref idLoc);

								if (accelId != null)
								{
									string[] value;

									if (currentAccelDict.TryGetValue(accelId, out value))
									{
										String newLine = srcLine.Replace(accelChar, value[4]);
										int modsLoc = idLoc + 1;
										String accelMods = srcLine.Substring(modsLoc).Trim();

										newLine = newLine.Replace(accelMods, value[3]);
										dstRcWriter.WriteLine(newLine);
										wrote = true;
										while (accelMods[accelMods.Length - 1] == ',')
										{
											accelMods += " ";
											srcLine = srcRcReader.ReadLine();
											if (srcLine == null)
											{
												return;
											}
											srcLine = srcLine.Trim();
											accelMods += srcLine;
										}
									}
								}
							}
						}
						else
						{
							String foundString = findString(srcLine);
							String transString = null;

							if (foundString != null && foundString.Length > 0)
							{
								if (dlgStrings != null)
								{
									dlgStrings.TryGetValue(foundString, out transString);
								}
								else
								{
									globalDict.TryGetValue(foundString, out transString);
									if (transString != null && ambigDict.ContainsKey(foundString))
									{
										MessageBox.Show(this, "Multiple translations for " + foundString + ".  " +
											"Using " + transString + " in " + currentSection + ".");
									}

								}
								if (transString != null)
								{
									dstRcWriter.WriteLine(srcLine.Replace(foundString, transString));
									wrote = true;
								}
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
				MessageBox.Show(this, "RC file translated successfully.");
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
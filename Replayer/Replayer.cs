/************************************************************************************\
 * Replayer - An Andrew Shurney Production                                          *
\************************************************************************************/

/*! \file		Replayer.cs
 *  \author		Andrew Shurney
 *  \brief		Main Replayer application
 */

using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Windows.Threading;

namespace Replayer
{
    public partial class Replayer : Form
    {
        RecordServer recordServer = null;
        Dispatcher uiDispatcher = Dispatcher.CurrentDispatcher;

        public Replayer(String[] args)
        {
            if (args.Length == 0)
                InitializeComponent();
            else
            {
                String exe = null;
                String workingDir = "";
                String arguments = "";
                String file = null;
                Boolean record = true;

                foreach (String arg in args)
                {
                    String lArg = arg.ToLower();
                    if (lArg == "-h" || lArg == "--help" || lArg == "-help")
                        PrintHelpAndClose();
                    else if (lArg == "-replay")
                        record = false;
                    else if (lArg == "-record")
                        record = true;
                    else if (String.Compare(lArg, 0, "-e:", 0, 3) == 0)
                    {
                        exe = arg.Substring(3);
                        Int32 lastSlash = exe.LastIndexOf('\\');
                        if (lastSlash < 0)
                            lastSlash = exe.LastIndexOf('/');

                        if (lastSlash >= 0)
                            workingDir = exe.Substring(0, lastSlash);
                    }
                    else if (String.Compare(lArg, 0, "-w:", 0, 3) == 0)
                        workingDir = arg.Substring(3);
                    else if (String.Compare(lArg, 0, "-a:", 0, 3) == 0)
                        arguments = arg.Substring(3);
                    else if (String.Compare(lArg, 0, "-f:", 0, 3) == 0)
                        file = arg.Substring(3);
                }

                if (exe == null)
                    PrintHelpAndClose();

                if (record)
                {
                    if (file == null)
                    {
                        String exeBaseName = exe;

                        Int32 lastSlash = exeBaseName.LastIndexOf('\\');
                        if (lastSlash < 0)
                            lastSlash = exeBaseName.LastIndexOf('/');

                        if (lastSlash >= 0)
                            exeBaseName = exeBaseName.Substring(lastSlash);

                        Int32 ext = exeBaseName.IndexOf('.');
                        if (ext > 0)
                            exeBaseName = exeBaseName.Substring(0, ext);

                        file = String.Format("{0}-{1}.rep", exeBaseName, DateTime.Now.ToString("yyyy.MM.dd.HH.mm.ss"));
                    }

                    Record(exe, workingDir, arguments, new FileStream(file, FileMode.Create, FileAccess.Write));
                }
                else
                {
                    if (file == null)
                        PrintHelpAndClose();

                    Replay(exe, workingDir, arguments, file);
                }
            }
        }

        private void PrintHelpAndClose()
        {
            Console.WriteLine("Replayer - by Andrew Shurney");
            Console.WriteLine("Usage:");
            Console.WriteLine("  replayer [-replay] [-record] -e:<exe> [-f:<file>] [-w:<workingDir>] [-a:<arguments>] [-help]");
            Console.WriteLine("Arguments:");
            Console.WriteLine("  -replay - Runs the program in replay mode. Requires a file name to be passed in.");
            Console.WriteLine("  -record - Runs the program in record mode. (Default)");
            Console.WriteLine("  -e      - Specifies the exe to run.");
            Console.WriteLine("  -f      - Specifies the filename to load or save.");
            Console.WriteLine("  -w      - Specifies the working directory of the exe. (Defaults to the exe's directory)");
            Console.WriteLine("  -a      - Arguments to be passed to the exe.");
            Console.WriteLine("  -help   - Displays this text.");
            Application.Exit();
        }

        private void btnBrowseExecutable_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Filter = "exe files (*.exe)|*.exe|All files (*.*)|*.*";
            openFile.CheckFileExists = true;

            if(openFile.ShowDialog() == DialogResult.OK)
            {
                txtExecutable.Text = openFile.FileName;

                int lastSlash = openFile.FileName.LastIndexOf('\\');
                if(lastSlash < 0)
                    lastSlash = openFile.FileName.LastIndexOf('/');

                if (lastSlash > 0)
                    txtWorkingDirectory.Text = openFile.FileName.Substring(0, lastSlash);
            }
        }

        private void btnBrowseWorkingDirectory_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog openFolder = new FolderBrowserDialog();
            
            if(openFolder.ShowDialog() == DialogResult.OK)
                txtWorkingDirectory.Text = openFolder.SelectedPath;
        }

        private void SetupProcClosed(ReplayerBackend backend)
        {
            Process proc = backend.GetProcess();
            proc.EnableRaisingEvents = true;
            proc.Exited += new EventHandler(proc_Exited);
        }

        void proc_Exited(object sender, EventArgs e)
        {
            if (Dispatcher.CurrentDispatcher != uiDispatcher)
            {
                uiDispatcher.Invoke((Action)delegate()
                {
                    proc_Exited(sender, e);
                });
            }
            else
            {
                if (recordServer != null)
                {
                    recordServer.Close();
                    recordServer = null;
                }

                if (btnRecord == null)
                {
                    Application.Exit();
                    return;
                }

                btnRecord.Enabled = true;
                btnReplay.Enabled = true;
            }
        }

        private void Record(String executable, String workingDirectory, String arguments, Stream file)
        {
            String pipeName = "ReplayRecordPipe";

            recordServer = new RecordServer(pipeName, file);

            ReplayerBackend backend = new ReplayerBackend(executable, workingDirectory, arguments);
            if (!backend.StartRecording(pipeName))
            {
                MessageBox.Show(String.Format("Failed to record {0}.", executable), "Failure", MessageBoxButtons.OK, MessageBoxIcon.Error);

                if (btnRecord == null)
                    Application.Exit();

                backend.Destroy();
            }
            else
                SetupProcClosed(backend);
        }

        private void btnRecord_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFile = new SaveFileDialog();
            saveFile.Filter = "Replay files (*.rep)|*.rep|All files (*.*)|*.*";
            saveFile.OverwritePrompt = true;

            if(saveFile.ShowDialog() == DialogResult.OK)
            {
                btnRecord.Enabled = false;
                btnReplay.Enabled = false;

                Record(txtExecutable.Text, txtWorkingDirectory.Text, txtArguments.Text, saveFile.OpenFile());
            }
        }

        private void Replay(String executable, String workingDirectory, String arguments, String fileName)
        {
            ReplayerBackend backend = new ReplayerBackend(executable, workingDirectory, arguments);
            if (!backend.StartReplaying(fileName))
            {
                MessageBox.Show(String.Format("Failed to replay {0} with {1}.", executable, fileName), "Failure", MessageBoxButtons.OK, MessageBoxIcon.Error);

                if (btnRecord == null)
                    Application.Exit();

                backend.Destroy();
            }
            else
                SetupProcClosed(backend);
        }

        private void btnReplay_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Filter = "Replay files (*.rep)|*.rep|All files (*.*)|*.*";
            openFile.CheckFileExists = true;

            if (openFile.ShowDialog() == DialogResult.OK)
            {
                btnRecord.Enabled = false;
                btnReplay.Enabled = false;

                Replay(txtExecutable.Text, txtWorkingDirectory.Text, txtArguments.Text, openFile.FileName);
            }
        }
    }
}

/************************************************************************************\
 * Replayer - An Andrew Shurney Production                                          *
\************************************************************************************/

/*! \file		RecordServer.cs
 *  \author		Andrew Shurney
 *  \brief		Stream pipe output to a gziped file while recording
 */

using System;
using System.ComponentModel;
using System.IO;
using System.IO.Compression;
using System.IO.Pipes;

namespace Replayer
{

    public class RecordServer
    {
        GZipStream file = null;
        NamedPipeServerStream pipe = null;
        BackgroundWorker bgThread;

        public RecordServer(String pipeName, Stream fileStream)
        {
            file = new GZipStream(fileStream, CompressionMode.Compress);


            pipe = new NamedPipeServerStream(pipeName, PipeDirection.In, 1, PipeTransmissionMode.Byte);

            bgThread = new BackgroundWorker();
            bgThread.WorkerSupportsCancellation = true;

            bgThread.DoWork += new DoWorkEventHandler(bgThread_DoWork);
            bgThread.RunWorkerCompleted += new RunWorkerCompletedEventHandler(bgThread_RunWorkerCompleted);
            bgThread.RunWorkerAsync();

        }

        void bgThread_DoWork(object sender, DoWorkEventArgs e)
        {
            pipe.WaitForConnection();
            pipe.CopyTo(file);
        }

        public void Close()
        {
            bgThread.CancelAsync();
        }

        void bgThread_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            pipe.Close();
            file.Close();
        }
    }
}

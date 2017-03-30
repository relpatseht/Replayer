/************************************************************************************\
 * Replayer - An Andrew Shurney Production                                          *
\************************************************************************************/

/*! \file		Program.cs
 *  \author		Andrew Shurney
 *  \brief		Entry point for Replayer
 */

using System;
using System.Windows.Forms;

namespace Replayer
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(String[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Replayer(args));
        }
    }
}

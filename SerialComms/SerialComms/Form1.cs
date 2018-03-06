using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace SerialComms
{
    public partial class SerialComms : Form
    {

        //make sure the COM ports are set correctly (COM number + Baud rates) on the serial port objects in the designer or none of this will work.

        public delegate void AddDataDelegate(String myString);
        public AddDataDelegate myDelegate;

        public SerialComms()
        {
            InitializeComponent();
        }


        private void SerialComms_Load(object sender, EventArgs e)
        {
            OneSheeld.Open();
            Zumo.Open();
            this.myDelegate = new AddDataDelegate(AddDataMethod);
        }

        private void OneSheeld_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string s = sp.ReadExisting();
            lstSerial.Invoke(this.myDelegate, new Object[] { s });
        }

        public void AddDataMethod(String s)
        {
            lstSerial.Items.Add(s);
            lstSerial.SelectedIndex = lstSerial.Items.Count - 1;
            lstSerial.SelectedIndex = -1;
            Zumo.WriteLine(s);
        }

    }
}

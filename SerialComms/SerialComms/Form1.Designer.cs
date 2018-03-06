namespace SerialComms
{
    partial class SerialComms
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
            this.components = new System.ComponentModel.Container();
            this.OneSheeld = new System.IO.Ports.SerialPort(this.components);
            this.lstSerial = new System.Windows.Forms.ListBox();
            this.Zumo = new System.IO.Ports.SerialPort(this.components);
            this.SuspendLayout();
            // 
            // OneSheeld
            // 
            this.OneSheeld.BaudRate = 115200;
            this.OneSheeld.PortName = "COM8";
            this.OneSheeld.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.OneSheeld_DataReceived);
            // 
            // lstSerial
            // 
            this.lstSerial.FormattingEnabled = true;
            this.lstSerial.Location = new System.Drawing.Point(12, 12);
            this.lstSerial.Name = "lstSerial";
            this.lstSerial.Size = new System.Drawing.Size(395, 264);
            this.lstSerial.TabIndex = 5;
            // 
            // Zumo
            // 
            this.Zumo.BaudRate = 57600;
            this.Zumo.PortName = "COM6";
            // 
            // SerialComms
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(421, 288);
            this.Controls.Add(this.lstSerial);
            this.Name = "SerialComms";
            this.ShowIcon = false;
            this.Load += new System.EventHandler(this.SerialComms_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.IO.Ports.SerialPort OneSheeld;
        private System.Windows.Forms.ListBox lstSerial;
        private System.IO.Ports.SerialPort Zumo;
    }
}


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Runtime.InteropServices; // 用 DllImport 需用此 命名空间
using System.Reflection; // 使用 Assembly 类需用此 命名空间
using System.Reflection.Emit; // 使用 ILGenerator 需用此 命名空间



namespace USB2UARTSPIIICDLLCsharpTEST
{
    public partial class Form1 : Form
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        //关于dll中函数的说明请查阅USB2UARTSPIIICDLL.h文件

        [DllImport("USB2UARTSPIIICDLL.dll")]
        public static extern int OpenUsb(uint UsbIndex);

        [DllImport("USB2UARTSPIIICDLL.dll")]
        public static extern int CloseUsb(uint UsbIndex);

        [DllImport("USB2UARTSPIIICDLL.dll")]
        public static extern int ConfigIICParam(uint rate,uint clkSLevel,uint UsbIndex);


        [DllImport("USB2UARTSPIIICDLL.dll")]
        public static extern int IICSendData(byte strartBit,byte stopBit,byte[] sendBuf,uint slen,uint UsbIndex);

        [DllImport("USB2UARTSPIIICDLL.dll")]
        public static extern int IICRcvData(byte stopBit,byte[] rcvBuf,uint rlen,uint UsbIndex);

       //需要用到dll中的其它函数，需要在这里添加声明

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            byte[] temp = new byte[1024];
            int num = 0;
            //打开索引为0的USB转SPI/IIC转接板的USB
            if (OpenUsb(0) < 0)
            {
                MessageBox.Show("打开USB错误");
                return;
            }
            //设置索引为0的USB转SPI/IIC转接板IIC主模式 速率100K
            ConfigIICParam(6,1000,0);


            //IIC主机向从机发送数据  发送两个字节
            temp[0] = 0x40 << 1;
            temp[1] = 0xE3;
            IICSendData(1, 1, temp, 2, 0);


            temp[0] = (0x40 << 1) + 1;
            IICSendData(1,0, temp, 1, 0);
            IICRcvData( 1, temp, 2, 0);

            CloseUsb(0);
        }
    }
}

// OpenNI includes
#include "kinectmotor.h"

#include <iostream>

using namespace std;

KinectMotor::KinectMotor()
{
        m_isOpen = false;

        m_xnUSBInit = false;
        m_xnUSBOpenDeviceByPath = false;
        m_xnUSBSendControl = false;
        m_noDevice = false;

}

KinectMotor::~KinectMotor()
{
        close();
}

bool KinectMotor::open()
{
        XnStatus res;

        if (!m_xnUSBInit){
            // Init OpenNI USB
            res = xnUSBInit();
            if (res != XN_STATUS_OK) {
                    xnPrintError(res, "xnUSBInit failed");
                    return false;
            }
        }
        m_xnUSBInit = true;

        if (!m_xnUSBOpenDeviceByPath){

            // Open "Kinect motor" USB device
            res = xnUSBEnumerateDevices(0x045E /* VendorID */, 0x02B0 /*ProductID*/, &m_paths, &m_count);
            if (res != XN_STATUS_OK) {
                xnPrintError(res, "xnUSBEnumerateDevices failed");
                return false;
            }

            // Open first found device
            res = xnUSBOpenDeviceByPath(m_paths[0], &m_dev);


            if (res != XN_STATUS_OK) {
                if (res == 131155){
                    m_noDevice = true;
                }
                xnPrintError(res, "xnUSBOpenDeviceByPath failed");
                return false;
            }
        }

        m_xnUSBOpenDeviceByPath = true;

        XnUChar buf[1]; // output buffer

        if (!m_xnUSBSendControl){
        // Init motor
        res = xnUSBSendControl(m_dev, XN_USB_CONTROL_TYPE_STANDARD, 0x10, 0x00, 0x00, buf, sizeof(buf), 0);
        if (res != XN_STATUS_OK) {
                xnPrintError(res, "xnUSBSendControl failed");
                close();
                return false;
        }
        }

        m_xnUSBSendControl = true;


        res = xnUSBSendControl(m_dev, XN_USB_CONTROL_TYPE_VENDOR, 0x06, 0x01, 0x00, NULL, 0, 0);
        if (res != XN_STATUS_OK) {
                xnPrintError(res, "xnUSBSendControl failed");
                close();
                return false;
        }

        cout << "The sensor is initialized" << endl;

        m_isOpen = true;

        return m_isOpen;
}

void KinectMotor::close()
{
        if (m_isOpen) {
                xnUSBCloseDevice(m_dev);
                m_isOpen = false;
        }
}

bool KinectMotor::isOpened(){
    return m_isOpen;
}

bool KinectMotor::move(int angle)
{

    if(m_isOpen){
        XnStatus res;

        // Send move control request
        res = xnUSBSendControl(m_dev, XN_USB_CONTROL_TYPE_VENDOR, 0x31, angle, 0x00, NULL, 0, 0);
        if (res != XN_STATUS_OK) {
                xnPrintError(res, "xnUSBSendControl failed");
                return false;
        }
        return true;
    }
}


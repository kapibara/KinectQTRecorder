#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <XnUSB.h>

class KinectMotor
{
public:
        KinectMotor();
        virtual ~KinectMotor();

        /**
         * Open device.
         * @return true if succeeded, false - overwise
         */
        bool open();

        /**
         * Close device.
         */
        void close();

        /**
         * Check status.
         */
        bool isOpened();

        /**
         * Move motor up or down to specified angle value.
         * @param angle value
         * @return true if succeeded, false - overwise
         */
        bool move(int angle);

        bool isNoDeviceError(){return m_noDevice;}

private:
        XN_USB_DEV_HANDLE m_dev;
        bool m_isOpen;

        bool m_xnUSBInit;
        bool m_xnUSBOpenDeviceByPath;
        bool m_xnUSBSendControl;
        bool m_noDevice;

        const XnUSBConnectionString *m_paths;
        XnUInt32 m_count;
};

#endif // MOTORCONTROL_H

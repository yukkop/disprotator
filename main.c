#include <Windows.h>
#include <iostream>

int main(int argc, TCHAR* argv[])
{
    if (RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 0x42)) //0x42 is 'b'
    {
        printf("Hotkey 'ALT+b' registered, using MOD_NOREPEAT flag\n");
    }

    DISPLAY_DEVICE displayDevice;
    displayDevice.cb = sizeof(DISPLAY_DEVICE);

    int index = 0;
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            if (index == 3)
                index = 0;
            else
                index = 3;

            int displayIndex = 1; // index of the second display (change as needed)
            int numDisplays = 0;
            while (EnumDisplayDevices(NULL, numDisplays, &displayDevice, 0))
            {
                if ((displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
                    (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) == 0)
                {
                    // Found a secondary display
                    DEVMODE devMode;
                    devMode.dmSize = sizeof(DEVMODE);
                    EnumDisplaySettingsEx(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &devMode, 0);

                    devMode.dmDisplayOrientation = index; // DMDO_DEFAULT will return to the original orientation
                    DWORD temp = devMode.dmPelsHeight;
                    devMode.dmPelsHeight = devMode.dmPelsWidth;
                    devMode.dmPelsWidth = temp;

                    LONG ret;
                    ret = ChangeDisplaySettingsEx(displayDevice.DeviceName, &devMode, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
                    if (ret == DISP_CHANGE_SUCCESSFUL)
                    {
                        ChangeDisplaySettingsEx(displayDevice.DeviceName, NULL, NULL, 0, NULL);
                        // Reset the display to apply the changes
                    }

                    if (--displayIndex == 0)
                        break;
                }

                numDisplays++;
            }
        }
    }

    return 0;
}
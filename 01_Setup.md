# Setup

## Mbed Studio

Install using paru/pacman:

```sh
paru -Syu mbed-studio
```

Alternatively navigate to [Mbed Studio](https://os.mbed.com/studio/) and select
the correct download.

On Linux, mark the downloaded file executable:

```sh
chmod 744 Mbed-Studio-1.4.3.sh
```

And then execute it:

```sh
./Mbed-Studio-1.4.3.sh
```

The installation can take quite some time.

## First Project

Inside the Mbed Studio, click `File` -> `Import Program`. Paste
[https://github.com/iotkitv3/gpio](https://github.com/iotkitv3/gpio) into the
"URL" field. Then click on "Add Program".

Modify the `main.cpp` file like this:

```cpp
/** DigitalOut creates a positive tension on a pin.
*/
#include "BUILD/DISCO_L475VG_IOT01A/ARMC6/mbed_config.h"
#include "mbed.h"

DigitalOut led1(MBED_CONF_IOTKIT_LED1);
DigitalOut led2(MBED_CONF_IOTKIT_LED2);
DigitalOut led3(MBED_CONF_IOTKIT_LED3);
DigitalOut led4(MBED_CONF_IOTKIT_LED4);

DigitalIn usrBtn(MBED_CONF_IOTKIT_BUTTON1);

#define WAIT_TIME 500

int main()
{
    uint8_t counter = 0;

    led1 = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;

    while (1) 
    {
        uint64_t state = usrBtn.read();
        counter += state;

        if (state > 4) state = 0;

        led1 = (counter >> 0) & 1;
        led2 = (counter >> 1) & 1;
        led3 = (counter >> 2) & 1;
        led4 = (counter >> 3) & 1;

        thread_sleep_for(WAIT_TIME);         
    }
}
```

## Deploy the Program

After a clean build, click the "Run program" button, located right of the
"Clean Build" button. On the top of the screen a notification might appear,
stating that the board needs to be restarted. Simply disconnect and reconnect
the USB cable again.

The application should be executed now, output is written to the serial console
(the window in the bottom part of the screen, labeled
"DISCO\_some\_numbers\_and\_letters").

## Trouble Shooting

Click "Clean Build". The build starts, but after some while prints out:

<samp>
[ERROR] armclang: error: System clock tampering detected. License checkout will fail.
armclang: error: Failed to check out a license.
armclang: note:
Information about this error is available at: http://ds.arm.com/support/lic56/m1004
</samp>

The link that supposedly should be pointing to more information about the error
leads to a 404 page.

Then the compilation fails with:

<samp>
armclang: error: ARM Compiler does not support '-mcpu=cortex-m4'
</samp>

Searching online doesn't help a lot. According to some ARM dev on a forum, they
themselves haven't figured it out yet. The suspected problem is, that during
the build process, a file with a future "mtime" is created. Searching my system
for such a file `find / -newermt '(date "+%Y-%m-%d %I:%M")' -iname "arm"
2>/dev/null` has let to no results.

After some trying out I figured, that you can press "Build" over and over
again. Every time, the building process will continue from where it left off,
until either the "License Check" fails or the build finally completes.

### Update (18.03.2022)

I found out that the error only happens when the IoTKit is connected to the
computer. Simply disconnect the board and the compiler stays happy :-).

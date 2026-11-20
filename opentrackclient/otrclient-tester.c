/**
 * Compile like so
 *

 Linux:

assuming your install prefix was /usr/local

 If you do not require opentrackclient but expect it to be in /usr/local/lib
       cc -DOTR_OPTIONAL -I/usr/local/include otrclient-tester.c -L/usr/local/lib -lopentrackclient-loader -o otrclient-tester -Wl,-rpath /usr/local/lib
 You may need to link against dl via -ldl as well

 Or if you require the opentrackclient library to be somehwre e.g. /usr/local/lib:
        cc -DOTR_REQUIRED -I/usr/local/include otrclient-tester.c -o otrclient-tester -L/usr/local/lib -lopentrackclient -Wl,-rpath /usr/local/lib


 macOS:

 Weakly link against the opentrackclient framework expecting people to have it in /Library/Frameworks/
        cc -DOTR_OPTIONAL otrclient-tester.c -F. -weak_framework opentrackclient -o otrclient-tester -rpath /Library/Frameworks

 Or if you require and ship the opentrackclient library with your program:
        cc -DOTR_REQUIRED otrclient-tester.c -F. -framework opentrackclient -o otrclient-tester -rpath @executable_path

 Or if you require the librarz and you expect the opentrackclient.dylib in /usr/local/lib then 
 cc -DOTR_REQUIRED -I/usr/local/include otrclient-tester.c -o otrclient-tester -L/usr/local/lib -lopentrackclient -Wl,-rpath /usr/local/lib


 When you're creating a sandboxed app the whole thing will not work anyway, because Apple will probably not let you access the shared memory.
 I guess easiest will be to use UDP networking instead.
 */

#include "opentrackclient/opentrackclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

int main(int argc,char** argv) {

#if defined(OTR_OPTIONAL)
    if(OTRGetVersion == NULL) {
        fprintf(stderr,"opentrackclient library not found.\n");
        exit(-1);
    }
#endif

    printf("Now using opentraclclient version %s (%s)\n",OTRGetVersion(),OTRGetProvider());


    OTRHeadpose data;

    while(1) {

        if(!OTRGetData(&data))
            break;



        printf("x=%f y=%f z=%f yaw=%f pitch=%f roll=%f\n",  data.X,
                                                            data.Y,
                                                            data.Z,
                                                            data.Yaw,
                                                            data.Pitch,
                                                            data.Roll);

        usleep(1000*250);
    }


}

/**
 * Compile with:
 *

 Linux:

 # assuming your isntall prefix is /usr/local


 If you do not require opentrackclient (recommended):
       cc -DOTR_OPTIONAL -I/usr/local/include otrclient-tester.c -L/usr/local/lib -lopentrackclient-loader64 -o otrclient-tester
 You may need to link against dl via -ldl as well

 Or if you really require the opentrackclient library:
        cc -DOTR_REQUIRED -I/usr/local/include otrclient-tester.c -o otrclient-tester \
            -L/usr/local/lib -lopentrackclient64 -Wl,-rpath /usr/local/lib


 macOS:

 Weakly link against the opentrackclient framework:
        cc -DOTR_OPTIONAL otrclient-tester.c -F. -weak_framework opentrackclient -o otrclient-tester
 Or if you really require the opentrackclient library:
        cc -DOTR_REQUIRED otrclient-tester.c -F. -framework opentrackclient -o otrclient-tester


 In Xcode add /Library/Application\ Support/opentrack/Frameworks/opentrackclient.framework to the target but set it to "optional" and don't embed it.
 When you're creating a sandboxed app the whole thing will not work anyway, because Apple will probably not let you access the shared memory.
 I guess easiest will be to use UDP networking instead.
 */

#include "opentrackclient/opentrackclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

int main(int argc,char** argv) {

    if(OTRGetVersion == NULL) {
        fprintf(stderr,"opentrackclient library not found.\n");
        exit(-1);
    }

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

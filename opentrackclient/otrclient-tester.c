/**
 * Compile with:
 *
 Posix:
 If you do not require opentrackclient (recommended):
       cc -DOTR_OPTIONAL -I/opt/opentrack/include otrclient-tester.c -L/opt/opentrack/lib -lopentrackclient-loader64 -o otrclient-tester
   or
       cc -DOTR_OPTIONAL -I/opt/opentrack/include opentrackclient-loader.c otrclient-tester.c -o otrclient-tester
 You may need to link against dl via -ldl as well

 Or if you really require the opentrackclient library:
        cc -DOTR_REQUIRED -I/opt/opentrack/include otrclient-tester.c -o otrclient-tester \
            -L/opt/opentrack/lib -lopentrackclient64 -Wl,-rpath /opt/opentrack/lib


 macOS:
 Weakly link against the opentrackclient framework:
        cc -DOTR_OPTIONAL otrclient-tester.c -F/Library/Frameworks -weak_framework opentrackclient -o otrclient-tester
 Or if you really require the opentrackclient library:
        cc -DOTR_REQUIRED otrclient-tester.c -F/Library/Frameworks -framework opentrackclient -o otrclient-tester

 In Xcode add /Library/Frameworks/opentrackclient.framework to the target but set it to "optional" and don't embed it.
 */

#include "opentrackclient/opentrackclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

int main(int argc,char** argv) {

    if(OTRGetVersion == NULL) {
        fprintf(stderr,"opentrackclient library not found. Opentrack might not be installed.\n");
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

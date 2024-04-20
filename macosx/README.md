# How I finally succeeded to build opentrack on macOS Sonoma as of 20.04.2024 on macOS 14.4
## Including PointTracker and NeuralNet Face-Tracker!


First install https://www.macports.org for your architecture (Intel or arm64), I did not get it to work with homebrew.
If you use homebrew, it may be necessary to temporarily comment out (#) the line that read something like this: `eval "$(/opt/homebrew/bin/brew shellenv)"` in case you have it in your ~/.zprofile or ~/.bash_profile file.

Open a Terminal:

        cd ~/Desktop/
        
        Unfortunately there's no onnxruntime in macports so we'll donwload it

        curl -L https://github.com/microsoft/onnxruntime/releases/download/v1.17.3/onnxruntime-osx-universal2-1.17.3.tgz > onnxruntime-osx.tgz
        
        tar -xzf onnxruntime-osx.tgz 


        git clone https://github.com/matatata/opentrack.git

        

        export PATH=$PATH:/opt/local/bin:/opt/local/libexec/qt5/bin
        
        sudo port selfupdate

In case you're on a Apple-Silicon Mac:

        export OTR_OSX_ARCH=arm64

        sudo port install cmake qt5 opencv4 libomp create-dmg ImageMagick
        
        cd ~/Desktop/opentrack
        
        cmake \
        -DOpenCV_DIR=/opt/local/libexec/opencv4/cmake \
        -DONNXRuntime_LIBRARY=~/Desktop/onnxruntime-osx-universal2-1.17.3/lib/libonnxruntime.dylib \
        -DONNXRuntime_INCLUDE_DIR=~/Desktop/onnxruntime-osx-universal2-1.17.3/include \
        -DOpenMP_CXX_FLAG="-Xclang -fopenmp" \
        -DOpenMP_CXX_INCLUDE_DIR=/opt/local/include/libomp \
        -DOpenMP_CXX_LIB_NAMES=libomp \
        -DOpenMP_C_FLAG="-Xclang -fopenmp" \
        -DOpenMP_C_INCLUDE_DIR=/opt/local/include/libomp \
        -DOpenMP_C_LIB_NAMES=libomp \
        -DOpenMP_libomp_LIBRARY=/opt/local/lib/libomp/libomp.dylib \
        -S . -B build --toolchain cmake/apple.cmake
        
        cd build
        make

Have a cup of tea.

        open install
        
If everything went fine You'll now see the opentrack.app in ~/Desktop/opentrack/build/install. If you open it it probably crashes on Apple-Silicon with something like `Termination Reason: Namespace CODESIGNING, Code 2 Invalid Page` in case you look at logs after clicking "Report to Apple" or similar.

In that case you'll have to sign it locally to be able to run it. I wonder why this error messages sucks so much.

        cd install
        codesign --force --deep --sign - opentrack.app
        
It should say `opentrack.app: replacing existing signature`
        
Now you should be able to start it. It will ask for permissions to access the Documents folder, because it wants to creatE/store the profile files. If you start tracking using trackers like PointTracker and neuralnet-Face that want to use a Webcam then macOS asks for permission. Grant permission, stop Tracking and restart again. Or maybe restart the app.

# Notes on X-Plane

Set the output to "UDP over network". Configure the remote address to be 127.0.0.1 Port 4242. Install the x-plane plugin https://github.com/amyinorbit/headtrack
Remove the qurantine flags by issuing `sudo xattr -cr .` inside it's folder before starting X-Plane. Only do that for software you trust! Start X-Plane and Fly! In X-Plane's Plugin-Menu activate HeadTracking. It should now pick up da Head-Position from opentrack. Also remember to configure a "Center" Keyboard Binding in opentrack's Options. You'll need it. Tweak the mappings and so on. Have fun.




  





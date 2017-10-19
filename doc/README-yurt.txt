##################################################################
      Instructions for building MinVR at the Brown/CCV YURT

 1. Use the following modules:
    $ module load cave
    $ module load cmake
    $ module load vrpn
    $ module load nvidia-driver
    $ module load freeglut

    You will need these modules loaded both to build the software
    *and* to run it after.  You can include these load commands in a
    file called .modules in your home directory, and they will be
    loaded automatically.

 2. Get the repo:

    $ git clone https://github.com/MinVR/MinVR.git 

    Or however you prefer to clone it.  If you're considering helping
    with the project, it is better to fork the repo on github, and
    then clone your forked repo.

 3. Create a build directory within the repo and cd to it:

    $ cd MinVR
    $ mkdir build
    $ cd build

 4. Run cmake with these flags:

    $ cmake .. -DMINVR_OPENGL_PLUGIN=ON -DMINVR_VRPN_PLUGIN=ON -DMINVR_SCALABLE_PLUGIN=ON -DMINVR_FREEGLUT_PLUGIN=ON -DBUILD_VRPN=OFF

 5. Execute make:

    $ make

 6. Install the files somewhere. By default the software will be
    installed to build/install, but you can change that with the
    -DINSTALLPATH=somepath option on the cmake execution in step 4
    above.

    $ make install

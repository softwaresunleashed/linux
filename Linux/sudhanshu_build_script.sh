# Export following Environment variables, needed for build
export PATH=$HOME/rpi_tools/compiler/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/:/usr/bin/python:$PATH
export ARCH=arm
export CROSS_COMPILE=arm-bcm2708hardfp-linux-gnueabi-
export KBUILD_OUTPUT=_build_output
export MODULES_OUTPUT=_modules_to_copy

# Clean all the build artifacts
# CAUTION : THE COMMAND BELOW WILL EVEN CLEAN .config FILE, SO YOU HAVE TO COPY IT AGAIN AND RE-CONFIGURE.
## make mrproper

# Check if Build Output (Artifacts) exists, If NOT then create one.
if [ ! -d "$KBUILD_OUTPUT" ]; then
	mkdir $KBUILD_OUTPUT
fi



# Trigger Kernel Configuration Command
make xconfig 


# Marks beginning of the Build process
echo "Builds Starts Now -->" 
start_time=$(date)

# Trigger Kernel & Modules Build Command
make -j4 && \
make -j4 modules_install INSTALL_MOD_PATH=$MODULES_OUTPUT
echo "Build Start Time --> " "$start_time" 
echo "Build End   Time --> " $(date)




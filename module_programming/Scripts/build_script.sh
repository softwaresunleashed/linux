#clear shell for new instance.
clear
########################## Credits ##########################
echo "=================Credits================"
echo "Author : Sudhanshu Gupta" 
echo "Date Created  : 01 Jan 2011" 
echo "Last Modified : 04 Jun 2011" 
echo "========================================"
###############################################################
echo 
echo
########################## Usage ##########################
echo "=================Usage================"
echo "Build Command : "
echo "$ sh ../Scripts/build_script.sh <kernel_src_root>"
echo "========================================"
###############################################################
echo 
echo
########################## Variables ##########################
KERNEL_DIR=$1
OUTPUT_FOLDER="__BUILD_ARTIFACTS"
BUILD_ARTIFACTS_DIR="$KERNEL_DIR/$OUTPUT_FOLDER"
MAJOR_VER=2
MINOR_VER=6
SUB_MINOR_VER=38
KERNEL_VER=$MAJOR_VER.$MINOR_VER.$SUB_MINOR_VER
###############################################################


########################## Build Cmds ##########################
## For building Kernel Source
BUILD_COMMAND="make -j30 && make -j30 modules && make -j30 modules_install && make -j30 install && cd /boot && mkinitramfs -o initrd.img.$KERNEL_VER $KERNEL_VER"


## For building modules
##BUILD_COMMAND="make -C $KERNEL_DIR O=$BUILD_ARTIFACTS_DIR SUBDIRS=$PWD modules"

###############################################################
echo "==================Build Env============="
echo "Using Kernel Directory ==> " $KERNEL_DIR
echo "Using Build Directory ==> " $BUILD_ARTIFACTS_DIR
echo "========================================"
echo
echo "Creating Build Directory (if non-existent)..."
mkdir -p $BUILD_ARTIFACTS_DIR
echo
# New Method of building Modules (via Prebuilt Kernel code) : Build Command
echo "Module Build Process Started..."
echo 
echo "Build Command : "
echo $BUILD_COMMAND

## ~~~~~~~~~~~~~~~~~~~~~~~~~
echo  cd $KERNEL_DIR
cd $KERNEL_DIR
#$BUILD_COMMAND
## ~~~~~~~~~~~~~~~~~~~~~~~~~
echo 
echo "Module Build Process End..."



######### OLD MAKEFILE IMPLEMENTATION ################

# trigger build make file.
#make -f makefile

# ============================================================================================= #
#$INCLUDE_FOLDERS=-I/usr/src/linux-headers-2.6.32-24 -I/usr/src/linux-headers-2.6.32-24/include -I/usr/src/linux-headers-2.6.32-24/asm

#INCLUDE_FOLDERS=/usr/src/linux-headers-2.6.32-24 /usr/src/linux-headers-2.6.32-24/include /usr/src/linux-headers-2.6.32-24/asm


# build command
#gcc -c hello_world_module.c -I/usr/src/linux-headers-2.6.32-24/include -I/usr/src/linux-headers-2.6.32-24/arch -I/usr/src/linux-headers-2.6.32-24/include/linux
#gcc -c hello_world_module.c -I $INCLUDE_FOLDERS
#${INCLUDE_FOLDERS}

# ============================================================================================= #


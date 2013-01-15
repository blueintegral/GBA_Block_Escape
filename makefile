###################################################################
#	Make file for homework4.						  
#																  
#	NOTE: Do not change this file unless otherwise instructed.    
#																  
###################################################################

DIR = c:/gba_studio
include $(DIR)/system/master.mak



 ####################
# GBA ROM Image Name #
 ####################

PROGNAME="homework4"



 #########################################
# Set a list of files you want to compile #
 #########################################
 
OFILES += main.o ball.o myLib.o PressEnter.o view.o GameOver.o death1.o death2.o level2.o level3.o level4.o level5.o win.o nuclear_blast.o LinkedList.o


 #########################################################
# Compile using Krawall (sound stuff, don't mess with it) #
 #########################################################
 
USE_KRAWALL=yes
KRAWALL_IS_REGISTERED=no
KRAWALL_FILES=secondpm.s3m



 ######################################
# Standard Makefile targets start here #
 ######################################
all : $(PROGNAME).$(EXT) clean



 ##############################################
# Most Makefile targets are predefined for you,#
# such as vba, clean ... in the following file #
 ##############################################

include $(DIR)/system/standard-targets.mak



 #####################################
# Custom  Makefile targets start here #
 #####################################

gfx: makefile





                             	         *Group 69*
                            Adam Miles     and   Dominic Reynolds
                          stu# 101266150    |     stu# 101274879


The purpose of this program is to run a multi-threaded simulation that is to create a map of rooms,
spawining in a ghost at a random location in the house, and having 4 hunters with ghost hunting
devices to go through this house and collect evidence that the ghost can leave behind. The hunters 
are to hopefully escape alive with their evidence and trying to correctly guess what type of ghost
haunts this house!!!



The files that are included in this is the main.c which runs the whole program, house.c which creates
and initializes the rooms, hunter.c which contains all the hunter actions, ghost.c that contains all
the ghost actions, evidence.c that holds functions to clean up memory and add evidences found to a 
list, utils.c that contains utils provided by the prof, logger.c that helps log the ghost and hunter
actions to the screen, defs.h which contains all the definitions, and the Makefile that compiles the
program.



To compile this program you must redirect you linux terminal to the appropriate location where your
files are stored and simply type the following:
                                                  "make"
this then should complie the program.



To then run the program, you must type in your terminal the following:
                                                                         "./FP"
this then should run the program.



Once the program is running, the program will prompt you to enter a name for all four hunters and a.
equipment device for them to use. After that, all you need to do is sit back, and relax! The program
should simultaneously call the ghost turn and all 4 hunter turns on their own thread with the logger 
then logging their action to the screen so you can see exactly what is happening! Once the program is 
complete, it will print out if the hunters successfully determined the ghost while printing the evidence 
they found to solidify this claim, or the hunters may lose by leaving out of fear or boredome causing 
the ghost to win.



BONUS MARKS IMPLEMENTATIONS:
● Allow the user to specify both name and device of hunters at the beginning.
● Use print formatting to make the simulation data clear and easy to read using consistent fixed-width
columns
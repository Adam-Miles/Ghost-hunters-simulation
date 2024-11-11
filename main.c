#include "defs.h"

int main(){
    // Initialize the random number generator
    srand(time(NULL));

    //Initializing the house, then populating it
    HouseType house;
    initHouse(&house);
    populateRooms(&house);

    //Initializing the ghost
    GhostType main_ghost;
    initGhost(&main_ghost, &house);

    //Looping and initializing the 4 hunters
    HunterType hunter[NUM_HUNTERS];
    for(int i = 0; i < NUM_HUNTERS; i++){
        initHunter(&hunter[i], &house);
    }


    //---NOW ENTERING THE MULTI-THREADING STAGE OF THE CODE----

    //Initializing the ghost and hunter threads
    pthread_t ghost_thread;
    pthread_t hunter_threads[NUM_HUNTERS];
    
    //Looping to create the hunter threads
    for(int i = 0; i < NUM_HUNTERS; i++){
        pthread_create(&hunter_threads[i], NULL, takeHunterTurn, &hunter[i]);
    }
    //Creating the ghost thread
    pthread_create(&ghost_thread, NULL, takeGhostTurn, &main_ghost);

    //Looping to join the hunter threads
    for(int i = 0; i < NUM_HUNTERS; i++){
        pthread_join(hunter_threads[i], NULL);
    }
    //Join the hunter threads
    pthread_join(ghost_thread, NULL);

    //---NOW EXITING THE MULTI-THREADING STAGE OF THE CODE----



    // NICELY PRINTING THE RESULTS HERE
    printf("=======================================\n");
    printf("All done! Let's tally the results...\n");
    printf("=======================================\n");
    //This will loop to print out each hunters reason for leaving
    for(int i = 0; i < NUM_HUNTERS; i++){
        //This will print out that the hunter left in fear
        if(strcmp(hunter[i].reason_for_leaving, "FEAR") == 0){
            printf("    %s has run away in fear!\n", hunter[i].name);
        }
        //This will print out that the hunter left in boredom
        else if(strcmp(hunter[i].reason_for_leaving, "BORED") == 0){
            printf("    %s has left in boredom!\n", hunter[i].name);
        }
    }
    printf("---------------------------------------\n");


    //Intializing ev_node to be able to print out the evidence that hunters ended up finding
    EvidenceNodeType* ev_node = house.shared_evidence->all_evidence->head;
    char found_ev[MAX_STR];

    //Calling a guess ghost function to figure out what the ghost might be based on found evidence
    char ghost_kind[MAX_STR];
    guessed_ghost(house.shared_evidence->all_evidence, ghost_kind);

    //Using a utils function to store the actual ghost
    char actual_ghost[MAX_STR];
    ghostToString(main_ghost.ghost, actual_ghost);

    //This will trigger if all the hunters left the game in fear
    if(strcmp(hunter[0].reason_for_leaving, "FEAR") == 0 && strcmp(hunter[1].reason_for_leaving, "FEAR") == 0 && strcmp(hunter[2].reason_for_leaving, "FEAR") == 0 && strcmp(hunter[3].reason_for_leaving, "FEAR") == 0){
        //The following 3 print statements are for neatness
        printf("All the hunters have run away in fear!\n");
        printf("The ghost has won!\n");
        printf("The hunters have failed!\n");

        //This print statment will print out what the hunters determined the ghost to be
        printf("Using the evidence they found, they incorrectly determined that the ghost is a %s\n", ghost_kind);

        //This will print out the actual ghost
        printf("The ghost is actually a %s\n", actual_ghost);

        //The following will print out, with the help of a loop, the evidence that hunters ended up finding
        printf("The hunters collected the following evidence:\n");
        while(ev_node != NULL){
            evidenceToString(ev_node->data, found_ev);
            printf("    * %s\n", found_ev);
            ev_node = ev_node->next;
        }
    }
    
    //This will trigger if at least one hunter reviewed the evidence collected and left due to enough evidence to make the guess
    else if(strcmp(hunter[0].reason_for_leaving, "EVIDENCE") == 0 || strcmp(hunter[1].reason_for_leaving, "EVIDENCE") == 0 || strcmp(hunter[2].reason_for_leaving, "EVIDENCE") == 0 || strcmp(hunter[3].reason_for_leaving, "EVIDENCE") == 0){
        //The following 2 print statements are for neatness
        printf("It seems the ghost has been discovered!\n");
        printf("The hunters have won the game!\n");

        //This print statment will print out what the hunters determined the ghost to be
        printf("Using the evidence they found, they correctly determined that the ghost is a %s\n", ghost_kind);

        //The following will print out, with the help of a loop, the evidence that hunters ended up finding
        printf("The hunters collected the following evidence:\n");
        while(ev_node != NULL){
            evidenceToString(ev_node->data, found_ev);
            printf("    * %s\n", found_ev);
            ev_node = ev_node->next;
        }
    }
    
    //This will trigger if the hunters left in both fear or boredome
    else{
        //The following 3 print statements are for neatness
        printf("All the hunters have left in boredom or fear!\n");
        printf("The ghost has won!\n");
        printf("The hunters have failed!\n");

        //This print statment will print out what the hunters determined the ghost to be
        printf("Using the evidence they found, they incorrectly determined that the ghost is a %s\n", ghost_kind);

        //This will print out the actual ghost
        printf("The ghost is actually a %s\n", actual_ghost);

        //The following will print out, with the help of a loop, the evidence that hunters ended up finding
        printf("The hunters collected the following evidence:\n");
        while(ev_node != NULL){
            evidenceToString(ev_node->data, found_ev);
            printf("    * %s\n", found_ev);
            ev_node = ev_node->next;
        }
    }
    
    //This calls a house memory cleanup that cleans up the whole program
    houseMemoryCleanup(&house);
    return 0;
}
#include "defs.h"

void initGhost(GhostType* gPtr, HouseType* hPtr){
    
    //Setting the ghost to a random ghost
    gPtr -> ghost = randomGhost();

    //Setting the boredom timer to 0
    gPtr -> boredom_timer = 0;
    

    //Chosing a random room for the ghost to spawn in
    int ghost_location = randInt(1, 12);
    //Initializing a ghost room node
    RoomNodeType* ghost_room_node = hPtr->rooms->head;
    //Looping to find where the ghost location wanted to spawn at and placing it in that room
    for(int i = 0; i < hPtr->rooms->size; i++){
        if(i == ghost_location){
            //adds the ghost to the new room
            gPtr -> ghost_current_room = ghost_room_node -> data;
            //Setting this rooms ghost presence to true
            gPtr->ghost_current_room->ghost_in_room = gPtr;
        }else{
            //This will just go to the next node
            ghost_room_node = ghost_room_node -> next;
        }
    }

    //Initializing the array evidence types of each type of ghost
    char POLTERGEIST_evidence[3] = {EMF, TEMPERATURE, FINGERPRINTS};
    char BANSHEE_evidence[3] = {EMF, TEMPERATURE, SOUND};
    char BULLIES_evidence[3] = {EMF, FINGERPRINTS, SOUND};
    char PHANTOM_evidence[3] = {TEMPERATURE, FINGERPRINTS, SOUND};

    //Switch cases for storing the three types of evidence the type of ghost leaves
    switch(gPtr->ghost){
        case POLTERGEIST:
            //Loops to fill the POLTERGEIST evidence_to_leave array with its right 3 evidence pieces
            for(int i = 0; i < 3; i++){
                gPtr -> evidence_to_leave[i] = POLTERGEIST_evidence[i];
            }
            break;
        case BANSHEE:
            //Loops to fill the BANSHEE evidence_to_leave array with its right 3 evidence pieces
            for(int i = 0; i < 3; i++){
                gPtr -> evidence_to_leave[i] = BANSHEE_evidence[i];
            }
            break;
        case BULLIES:
            //Loops to fill the BULLIES evidence_to_leave array with its right 3 evidence pieces
            for(int i = 0; i < 3; i++){
                gPtr -> evidence_to_leave[i] = BULLIES_evidence[i];
            }
            break;
        case PHANTOM:
            //Loops to fill the PHANTOM evidence_to_leave array with its right 3 evidence pieces
            for(int i = 0; i < 3; i++){
                gPtr -> evidence_to_leave[i] = PHANTOM_evidence[i];
            }
            break;
        //Default case that will just leave
        default:
            break;
    }

    //Logs init of the ghost
    printf("==================================================================================================\n");
    l_ghostInit(gPtr -> ghost, gPtr -> ghost_current_room->room_name);
}

//---------------------------------------------------------------------------------------------------------

void ghostLeftEv(GhostType* ghost){
    //Choosing a random ev to leave
    int evidence_index_picker = randInt(0, 3);

    //Setting up a variable to store the ev to leave
    EvidenceType the_ev_to_leave;

    //Storing the variable to leave from the appropriate list
    the_ev_to_leave = (ghost->evidence_to_leave[evidence_index_picker]);

    //Locking by calling the semaphore
    sem_wait(&ghost->ghost_current_room->room_lock);

    //Callling existing function
    addEv(ghost->ghost_current_room->left_evidence, the_ev_to_leave);

    //Then unlocking
    sem_post(&ghost->ghost_current_room->room_lock);
    
    //Logging the ghost evidence that was left
    printf("========================================================\n");
    l_ghostEvidence(the_ev_to_leave, ghost->ghost_current_room->room_name);
}

//---------------------------------------------------------------------------------------------------------

char changeGhostRoom(GhostType* ghost){
    //if hunter is in the same room as the ghost
    if(ghost->ghost_current_room->hunter_count == 0){
        //Locking by calling the semaphore
        sem_wait(&ghost->ghost_current_room->room_lock);

        //Setting this rooms ghost presence to false
        ghost->ghost_current_room->ghost_in_room = NULL;
        //Choosing a random room to leave to
        int room_index_picker = randInt(0, ghost->ghost_current_room->rooms->size);

        
        //Creating a tracker of the adjacent rooms
        RoomNodeType* ghost_room_node = ghost->ghost_current_room->rooms->head;

        for(int i = 0; i < ghost->ghost_current_room->rooms->size; i++){
            if(i == room_index_picker){
                //Then unlocking
                sem_post(&ghost->ghost_current_room->room_lock);

                //adds the ghost to the new room
                ghost -> ghost_current_room = ghost_room_node -> data;

                //Locking by calling the semaphore
                sem_wait(&ghost->ghost_current_room->room_lock);

                //Setting the new rooms ghost presence to true
                ghost->ghost_current_room->ghost_in_room = ghost;

                //Logging the ghost movement
                printf("========================================================\n");
                l_ghostMove(ghost->ghost_current_room->room_name);
                
                //Then unlocking
                sem_post(&ghost->ghost_current_room->room_lock);
                break;
            }else{
                //Going to the next node
                ghost_room_node = ghost_room_node -> next;
            }
        }
        return C_TRUE;
    }else{
        return C_FALSE;
    }
}

//---------------------------------------------------------------------------------------------------------

void *takeGhostTurn(void* ghost_temp){
    //Initializing a temp ghost
    GhostType* ghost = (GhostType*)ghost_temp;

    //Looping to keep taking a ghost turn
    while(ghost->boredom_timer <= BOREDOM_MAX){
        //Choosing a random choice for the ghost
        int ghost_choice = randInt(0, 3);

        //Initializing an exitflag for a while loop
        char exitFlag = C_FALSE;

        // keep trying to make a valid move
        while(exitFlag != C_TRUE){
            //This choice will leave evidence
            if(ghost_choice == 0){
                ghostLeftEv(ghost);
                exitFlag = C_TRUE;
            }
            //This choice will change rooms
            else if( ghost_choice == 1){
                exitFlag = changeGhostRoom(ghost);
                //If the ghost cant change rooms, then it will pick a new random choice to do
                ghost_choice = randInt(0, 3);
            }
            //This choice will just do nothing
            else if(ghost_choice == 2){
                exitFlag = C_TRUE;
            }
        }
        //increasing or reseting the boredom timer for the ghost
        if (ghost->ghost_current_room->hunter_count > 0){
            ghost -> boredom_timer = 0;
        }
        else{
            ghost -> boredom_timer++;
        }

    }
    //Logging the ghost exit then exiting the thread
    printf("========================================================\n");
    l_ghostExit(LOG_BORED);
    pthread_exit(ghost);
}

//---------------------------------------------------------------------------------------------------------

void guessed_ghost(EvidenceListType* Ev, char *ghost_kind){
    //If the Ev size is less than 3, then the ghost cant be guessed and returns UNKNOWN
    if (Ev->size < 3){
        ghostToString(GH_UNKNOWN, ghost_kind);
    }
    
    //Finding which piece of evidence is missing then storing it
    else{
        EvidenceType missing_ev = EMF;
        for(int i = 0; i < EV_COUNT; i++){
            char found = C_FALSE;
        EvidenceNodeType* node = Ev->head;
            while(node != NULL){
                if(node->data == missing_ev){
                    found = C_TRUE;
                }
                node = node->next;
            }
            if(!found){
                break;
            }
            else{
                missing_ev += 1;
            }
        }

        char buffer[MAX_STR];
        //Using a switch case using the missing evidence to find out which ghost it is
        switch(missing_ev){
            //If the evidence missing from the collected evidence is an EMF then its a phantom and convers the ghost enum to a string
            case EMF:
                ghostToString(PHANTOM, buffer);
                break;
            
            //If the evidence missing from the collected evidence is TEMPERATURE then its a bullies and convers the ghost enum to a string
            case TEMPERATURE:
                ghostToString(BULLIES, buffer);
                break;
            
            //If the evidence missing from the collected evidence is FINGERPRINTS then its a banshee and convers the ghost enum to a string
            case FINGERPRINTS:
                ghostToString(BANSHEE, buffer);
                break;
            
            //If the evidence missing from the collected evidence is SOUND then its a poltergeist and convers the ghost enum to a string
            case SOUND:
                ghostToString(POLTERGEIST, buffer);
                break;

            //Default case that just breaks if the other cases were not met
            default:
                break;
        }
        //Sets the output value to the type of ghost
        strcpy(ghost_kind, buffer);
    }
}
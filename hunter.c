#include "defs.h"

void initHunter(HunterType* hunter, HouseType* house){
    
    //Prompting the user to name the hunter and the collecting the users input
    printf("Enter the name of a hunter: ");
    char hunter_name[MAX_STR];
    fgets(hunter_name, MAX_STR, stdin);
    hunter_name[strlen(hunter_name)-1] = 0;
    strcpy(hunter->name, hunter_name);

    //This while loop will ask the user to assign an equipment device for the hunter to use
    char exitFlag = C_FALSE;
    while(exitFlag != C_TRUE){
        //Prompting the user with its options and then collecting their response
        printf("Please enter an equipment type for %s (a: EMF, b: TEMPERATURE, c: FINGERPRINTS, d: SOUND): ", hunter_name);
        char user_choice;
        scanf("%s", &user_choice);
        while((getchar()) != '\n');

        //Making the user choice input to upper
        user_choice = toupper(user_choice);
        //Decreasing the user choice ascii decimal
        user_choice -= 65;
        
        //If the user choice is within the ascii decimal range then it sets it to the correct equipment device
        if(user_choice <=3 && user_choice >= 0){
            hunter -> equipment_device = user_choice;
            exitFlag = C_TRUE;
        }
        //Else it just tells them the selection is invalid and restarts the loop to ask them for input again
        else{
            printf("Invalid selection\n");
            exitFlag = C_FALSE;
        }

    }

    //Setting the hunter evidence to the shared all evidence list
    hunter->hunterEv = house->shared_evidence;

    //Setting the hunter in the van
    hunter->hunter_current_room = house->rooms->head->data;

    //Increasing the hunter count for the currrent van room by 1
    hunter->hunter_current_room->hunter_count =+ 1;

    //Initializing the hunters fear and boredom to 0
    hunter->fear = 0;
    hunter->boredom = 0;

    //Logging the hunter
    l_hunterInit(hunter -> name, hunter -> equipment_device);
    printf("==================================================================================================\n");
}


//---------------------------------------------------------------------------------------------------------

void MoveHunter(HunterType* hunter){
    //Locking this with a semaphore
    sem_wait(&hunter->hunter_current_room->room_lock);

    //Choosing a random room to leave to
    int room_index_picker = randInt(0, hunter->hunter_current_room->rooms->size);

    //Creating a tracker of the adjacent rooms
    RoomNodeType* hunter_room_node = hunter->hunter_current_room->rooms->head;
    
    //Loops to move the hunter to another adjacent room
    for(int i = 0; i < hunter->hunter_current_room->rooms->size; i++){
        if(i == room_index_picker){
            //This loop is making a temp array to store the old array in a better order
            hunter->hunter_current_room->hunter_count -=1;

            //Unlocking
            sem_post(&hunter->hunter_current_room->room_lock);
                
            //adds the new room to the hunter
            hunter -> hunter_current_room = hunter_room_node -> data;

            //Locking this with a semaphore
            sem_wait(&hunter->hunter_current_room->room_lock);

            //Increasing the hunter count
            hunter->hunter_current_room->hunter_count +=1;

            //Unlocking
            sem_post(&hunter->hunter_current_room->room_lock);

            //Logging the hunter movement
            printf("========================================================\n");
            l_hunterMove(hunter->name, hunter->hunter_current_room->room_name);
            
            break;
        }else{
            //Going to the next room node
            hunter_room_node = hunter_room_node -> next;
        }
    }
}

//---------------------------------------------------------------------------------------------------------

void HunterCollect(HunterType* hunter){
    //Locking this with a semaphore
    sem_wait(&hunter->hunter_current_room->room_lock);

    
    //Locking this with a semaphore
    sem_wait(&hunter->hunterEv->evidence_lock);

    //Initializing variables
    EvidenceNodeType *LeftEv = hunter->hunter_current_room->left_evidence->head;
    int does_it_match = C_FALSE;

    //Looping through to see if the left evidence matches the hunters equipment device and if not the it goes to the next ev
    while(LeftEv != NULL){
        if(LeftEv->data == hunter->equipment_device){
            does_it_match = C_TRUE;
            break;
        }
        else{
            does_it_match = C_FALSE;
            LeftEv = LeftEv->next;
        }
    }

    //Looping through to see if the evidence is already been added to the all_evidence and if not the it goes to the next node
    EvidenceNodeType *node = hunter->hunterEv->all_evidence->head;
    int is_it_already_added = C_FALSE;
    while(node != NULL){
        if(node->data == hunter->equipment_device){
            is_it_already_added = C_TRUE;
            break;
        }
        else{
            is_it_already_added = C_FALSE;
            node = node->next;
        }
    }
    
    /*This will add the evidence if and only if the evidence left by the ghost is of the correct type that
    the hunter is looking for AND if the evidence was not already added to all the evidence to collect*/
    if(does_it_match == C_TRUE && is_it_already_added == C_FALSE){
       addEv(hunter->hunterEv->all_evidence, hunter->equipment_device);
       printf("========================================================\n");
       l_hunterCollect(hunter->name, hunter->equipment_device, hunter->hunter_current_room->room_name);
    }

    //Unlocking
    sem_post(&hunter->hunterEv->evidence_lock);
    sem_post(&hunter->hunter_current_room->room_lock);
}

//---------------------------------------------------------------------------------------------------------

char HunterReview(HunterType* hunter){

    //Initializing a variable the with the shared evidence list size
   int list_size = hunter->hunterEv->all_evidence->size;
    
    //If the list is of size 3, then this means there are 3 unique pieces of evidence found and logs it
    if(list_size == 3){
        printf("========================================================\n");
        l_hunterReview(hunter->name, LOG_SUFFICIENT);
        return C_TRUE;
    }
    //If the list is less than the size 3, then this means there are not enough pieces of evidence found and logs it
    else if(list_size < 3){
        printf("========================================================\n");
        l_hunterReview(hunter->name, LOG_INSUFFICIENT);
        return C_FALSE;
    }
    //This is a log statment for an unknown case
    else{
        printf("========================================================\n");
        l_hunterReview(hunter->name, LOG_UNKNOWN);
        return C_FALSE;
    }
}

//---------------------------------------------------------------------------------------------------------

char HunterExit(HunterType* hunter){
    //If the hunter reached max boredom it returns true
    if(hunter->boredom >= BOREDOM_MAX){
        //Logs the hunter leaving of boredom
        printf("========================================================\n");
        l_hunterExit(hunter->name, LOG_BORED);
        //Sets the hunters reason for leaving to bored
        strcpy(hunter->reason_for_leaving, "BORED");
        return C_TRUE;
    }
    //If the hunter reached max fear it returns true
    else if(hunter->fear >= FEAR_MAX){
        //Logs the hunter leaving out of fear
        printf("========================================================\n");
        l_hunterExit(hunter->name, LOG_FEAR);
        //Sets the hunters reason for leaving to fear
        strcpy(hunter->reason_for_leaving, "FEAR");
        return C_TRUE;
    }
    //If the fear or boredom isnt at max, it just returns false
    else{
        return C_FALSE;
    }

}

//---------------------------------------------------------------------------------------------------------
void* takeHunterTurn(void* hunter_temp){
    //Initializing varialbes 
    HunterType* hunter = (HunterType*)hunter_temp;
    char exitFlag = C_FALSE;

    //Looping to take the hunter turn
    while(exitFlag == C_FALSE){
        //Inceasing the fear and reseting the boredom
        if(hunter->hunter_current_room->ghost_in_room != NULL){
            hunter->fear++;
            hunter->boredom = 0;
        }
        //Increasing the boredom
        else{
            hunter->boredom++;
        }

        //Choosing a random choice for the ghost
        int hunter_choice =randInt(0, 3);

        //Seeing if we are to exit due to boredom or fear
        if(HunterExit(hunter) == C_TRUE){
            //If the hunter is to exit, then the following removes the hunter from the room count and itself fully from the room
            hunter->hunter_current_room->hunter_count--;
            hunter->hunter_current_room = NULL;
            exitFlag = C_TRUE;
            //The hunter will then exit the thread
            pthread_exit(hunter);
        }

        //Moves hunter
        if(hunter_choice == 0){
            MoveHunter(hunter);
            exitFlag = C_FALSE;
        }
        //This has the hunter collect evidence if there is any to collect
        else if(hunter_choice == 1){
            HunterCollect(hunter); 
            exitFlag = C_FALSE;
        }
        //Has the hunter review if we have enough evidence
        else if(hunter_choice == 2){
            //This will activate if we have enough evidence after reviewing
            if(HunterReview(hunter) == C_TRUE){
                //Logging the hunter exit reason
                printf("========================================================\n");
                l_hunterExit(hunter->name, LOG_EVIDENCE);

                //Setting the hunters reason for leaving
                strcpy(hunter->reason_for_leaving, "EVIDENCE");

                //Removing the hunter room count and itself fully from the room
                hunter->hunter_current_room->hunter_count--;
                hunter->hunter_current_room = NULL;
                exitFlag = C_TRUE;
                //Exiting the thread
                pthread_exit(hunter);
            }
            //If the review came up as insufficient, then it just will go back in the loop
            else{
                exitFlag = C_FALSE;
            }
        }
    }
    return 0;
}
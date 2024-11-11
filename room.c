#include "defs.h"

RoomType* createRoom(char* r_Name){

    //Initializing a RoomType variable and then allocating memory for it
    RoomType* returned_room;
    returned_room = (RoomType*)malloc(sizeof(RoomType));

    //Setting the room name
    strcpy(returned_room -> room_name, r_Name);
    
    //Allocating memory for the left evidence then initializing it
    returned_room -> left_evidence = (EvidenceListType*)malloc(sizeof(EvidenceListType));
    initEvidenceList(returned_room -> left_evidence);

    //Allocating memory for the rooms then initializing it
    returned_room -> rooms = (RoomListType*)malloc(sizeof(RoomListType));
    initRoomList(returned_room -> rooms);

    //Setting the hunter count at 0
    returned_room->hunter_count = 0;

    //Initializing the room lock semaphore
    sem_init(&returned_room->room_lock, 0, 1);
    
    //Setting the ghost in room indicator to NULL
    returned_room->ghost_in_room = NULL;

    //Finally it returns the newly created room
    return returned_room;
}

//---------------------------------------------------------------------------------------------------------

void roomNodeCreator(RoomType* roomNodeData, RoomNodeType** node){
    //alocate space for the node
    *node = (RoomNodeType*)malloc(sizeof(RoomNodeType));
    //Set the node's data
    (*node)->data = roomNodeData;
    (*node)->next = NULL;
}

//---------------------------------------------------------------------------------------------------------

void addRoom(RoomListType* r_list, RoomType* added_room){
    //create the node
    RoomNodeType *nodePtr;
    roomNodeCreator(added_room, &nodePtr);
    r_list->size += 1;
    //handle the case of adding to an empty list
    if(r_list->size == 1){
        r_list->head = nodePtr;
        r_list->tail = nodePtr;
    }
    //otherwise, add to the back
    else{
        r_list->tail->next = nodePtr;
        r_list->tail = nodePtr;
    }
    
}

//---------------------------------------------------------------------------------------------------------

void connectRooms(RoomType* r1, RoomType* r2){
    //Calling existing functions to connect the rooms
    addRoom(r1->rooms, r2);
    addRoom(r2->rooms, r1);
}

//---------------------------------------------------------------------------------------------------------

void initRoomList(RoomListType* rList){
    //Initializing the head and tail to null with a size of 0
    rList -> head = NULL;
    rList -> tail = NULL;
    rList -> size = 0;
}


//---------------------------------------------------------------------------------------------------------

void roomNodeCleanup(RoomListType* r_list){

    //If the size of the list is 0 then it just exits
    if(r_list->size == 0){
        return;
    }

    //initializing variables
    RoomNodeType* currNode = r_list -> head;
    RoomNodeType* nextNode = r_list -> head -> next;

    //This will loop through the room nodes and free the memory
    for(int i = 0; i < r_list -> size; i++){
        //Frees the node
        free(currNode);
        if(nextNode == NULL){
            break;
        }
        //setting the new current node
        currNode = nextNode;
        //setting the next node after the new current node
        nextNode = nextNode -> next;
    }
}

//---------------------------------------------------------------------------------------------------------

void roomDataCleanup(RoomListType* r_list){

    //initializing variables
    RoomNodeType* currNode = r_list -> head;
    RoomNodeType* nextNode = r_list -> head -> next;

    //Looping throught the room list
    for(int i = 0; i < r_list -> size; i++){
        //Frees the left evidence node
        evNodeCleanup(currNode->data->left_evidence); 
        //Frees the left evidence data
        free(currNode->data->left_evidence);

        //Frees the rooms node
        roomNodeCleanup(currNode->data->rooms);
        //Frees the rooms data
        free(currNode->data->rooms);
        //Frees the currNode data
        free(currNode->data);

        //If the next node is NULL then the loop will stop
        if(nextNode == NULL){
            break;
        }
        //setting the new current node
        currNode = nextNode;
        //setting the next node after the new current node
        nextNode = nextNode -> next;
    }
}

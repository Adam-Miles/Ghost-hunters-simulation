#include "defs.h"

void initEvidenceList(EvidenceListType* evList){
    //Initializing the head and tail to null with a size of 0
    evList -> head = NULL;
    evList -> tail = NULL;
    evList -> size = 0;
}

//---------------------------------------------------------------------------------------------------------

void initSharedEv(HunterEvidenceType* allEv){
    //Allocationg memory for the all evidence
    allEv -> all_evidence = (EvidenceListType*)malloc(sizeof(EvidenceListType));
    //Initializing a semaphore for the evidence
    sem_init(&allEv->evidence_lock, 0, 1);
    //Initializing the evidence list
    initEvidenceList(allEv->all_evidence);
}

//---------------------------------------------------------------------------------------------------------

void evNodeCreator(EvidenceType evNodeData, EvidenceNodeType** node){
    //alocate space for the node
    *node = (EvidenceNodeType*)malloc(sizeof(EvidenceNodeType));
    //Set the node's data
    (*node)->data = evNodeData;
    (*node)->next = NULL; 
}

//---------------------------------------------------------------------------------------------------------

void addEv(EvidenceListType* evList, EvidenceType added_ev){

    //create the node by calling an existing function
    EvidenceNodeType *nodePtr;
    evNodeCreator(added_ev, &nodePtr);

    evList->size += 1;
    //handle the case of adding to an empty list
    if(evList->size == 1){
        evList->head = nodePtr;
        evList->tail = nodePtr;
    }
    //otherwise, add to the back
    else{
        evList->tail->next = nodePtr;
        evList->tail = nodePtr;
    }
}

//---------------------------------------------------------------------------------------------------------

void evNodeCleanup(EvidenceListType* evList){

    //If the size of the list is 0 then it just exits
    if(evList->size == 0){
        return;
    }

    //initializing variables
    EvidenceNodeType* currNode = evList -> head;
    EvidenceNodeType* nextNode = evList -> head -> next;

    //This will loop through the room nodes and free the memory
    for(int i = 0; i < evList -> size; i++){
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

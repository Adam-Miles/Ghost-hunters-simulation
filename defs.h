#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <ctype.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);

// ManMade Structs V

typedef struct House HouseType;
typedef struct RoomNode RoomNodeType;
typedef struct RoomList RoomListType;
typedef struct Room RoomType;
typedef struct Hunter HunterType;
typedef struct Ghost GhostType;
typedef struct EvidenceNode EvidenceNodeType;
typedef struct EvidenceList EvidenceListType;
typedef struct Evidence HunterEvidenceType;

struct House{
    HunterType *hunter;
    RoomListType *rooms;
    HunterEvidenceType *shared_evidence;    
};

struct RoomNode{
    RoomType* data;
    RoomNodeType* next;
};

struct RoomList{
    RoomNodeType* head;
    RoomNodeType* tail;
    int size;
};

struct Room{
    char room_name[MAX_STR];
    RoomListType* rooms;
    EvidenceListType* left_evidence;
    int hunter_count;
    GhostType* ghost_in_room;
    sem_t room_lock;
};

struct Hunter{
    RoomType* hunter_current_room;
    EvidenceType equipment_device;
    char name[MAX_STR];
    int fear;
    int boredom;
    HunterEvidenceType* hunterEv;
    char reason_for_leaving[MAX_STR];
};

struct Ghost{
    GhostClass ghost;
    RoomType* ghost_current_room;
    int boredom_timer;
    EvidenceType evidence_to_leave[3];
};

struct EvidenceNode{
    EvidenceType data;
    EvidenceNodeType* next;
};

struct EvidenceList{
    EvidenceNodeType* head;
    EvidenceNodeType* tail;
    int size;
};

struct Evidence{
    struct EvidenceList* all_evidence;
    sem_t evidence_lock;
};


/*
    Function:initHouse(HouseType* house)
    Purpose: Initalizes the fields of the house structure
    Parameters:
        Output: HouseType* house - The house being initalized
*/
void initHouse(HouseType* house);
/*
    Function: populateRooms(HouseType* house)
    Purpose: calls the functions needed to initalize, add and connect rooms to the house
    Parameters:
        Output: HouseType* house - A pointer to the house whose pointers are being initalized
*/
void populateRooms(HouseType* house);


/*
    Function: createRoom(char* r_Name)
    Purpose: initializes and returns a with the name specified by the r_Name parameter
    Parameters:
        input: char* r_name - A string containing the name for a given room
    Returns: A pointer to a RoomType sruct created by the function

*/
RoomType* createRoom(char* r_Name);
/*
    Function: connectRooms(RoomType*, RoomType*)
    Purpose: connects the two rooms entered as parameters by adding their pointers to eachother's adjacency list
    Parameters:
        RoomType* r1, r2: The two rooms being connected

*/
void connectRooms(RoomType* r1, RoomType* r2);
/*
    Function: initRoomList(RoomListType* rList)
    Purpose: initalizes the RoomListType structure
    Parameters:
        output: RoomListType* rList - the linked list of rooms being initalized
*/
void initRoomList(RoomListType* rList);
/*
    Function: addRoom(RoomListType* r_list, RoomType* added_room)
    Purpose: adds the room parameter to the linked list of rooms
    Parameters:
        output: RoomListType* r_list - The linked list to which the room is being appended
        input: RoomType* added_room - The room being appended to the linked list of rooms

*/
void addRoom(RoomListType* r_list, RoomType* added_room);
/*
    Function: roomNodeCleanup(RoomListType* r_list)
    Purpose: deallocates memory for all of the nodes in the provided linked list
    Parameters:
        Output: RoomListType* r_list - the list whoses nodes must be deallocated
*/
void roomNodeCleanup(RoomListType* r_list);


/*
    Function: initGhost(GhostType* gPtr, HouseType* hPtr)
    Purpose: Initalize the fields of the entered ghost
    Paramters:
        Output: GhostType* gPtr - The ghost who's data is being initalized
        Input: HouseType* hptr - the house containing the rooms that the ghost could start in
*/
void initGhost(GhostType* gPtr, HouseType* hPtr);
/*
    Function: ghostLeftEv(GhostType* ghost)
    Purpose: Add one of the ghosts's types of evidence to a room's evidence list
    Parameters:
        input: GhostType* ghost - The ghost leaving the evidence in it's room
*/
void ghostLeftEv(GhostType* ghost);
/*
    Function: changeGhostRoom(GhostType* ghost)
    Purpose: changes the ghosts room if there is no hunter in its current room
    Parameters:
        Input/output: GhostType* ghost - the ghost whose room is being updated
    Returns
        - C_TRUE if the room change was sucessful
        - C_FALSE if the room change was not sucessful
*/
char changeGhostRoom(GhostType* ghost);
/*
    Function: takeGhostTurn(void* ghost_temp)
    Purpose: Runs and manages the ghost thread
    Parameters:
        Input/Output: void* ghost_temp - a void pointer to the ghost whom is used for the thread

*/
void *takeGhostTurn(void* ghost_temp);


/*
    Function: initHunter(HunterType* hunter, HouseType* house)
    Purpose: intializes all fields of the provided HunterType structur
    Parameters:
        Output: HunterType* hunter - The hunter being initalized
        Input: HouseType* house - The house from which the hunter is operated
*/
void initHunter(HunterType* hunter, HouseType* house);
/*
    Function: MoveHunter(HunterType* hunter)
    Purpose: changes the room of a given hunter
    Parameters:
        Input/Output: HunterType* hunter - The hunter changing rooms
    
*/
void MoveHunter(HunterType* hunter);
/*
    Function: HunterCollect(HunterType* hunter)
    Purpose: Collects evidence matching the hunter's equipment if it is in the room
    Parameters:
        Input/output: HunterType* hunter - The hunter trying to collect evidence
*/
void HunterCollect(HunterType* hunter);
/*
    Function: HunterReview(HunterType* hunter)
    Purpose: Determine weather or not the hunters have sufficent evidence to determine the type of ghost in the house
    Parameters:
        Input: HunterType* - the hunter reviewing the evidence
    Returns:
        C_TRUE - if there is sufficent evidence
        C_FALSE - if there is not sufficent evidence
*/
char HunterReview(HunterType* hunter);
/*
    Function: HunterExit(HunterType* hunter)
    Purpose: Determines if a hunter must exit the simulation due to fear or boredom
    Parameters:
        Input/output: HunterType* hunter - The hunter in question about exiting

    Returns
        - C_TRUE if the hunter must exit
        - C_FALSE otherwise
*/
char HunterExit(HunterType* hunter);
/**************************************/
void getHunterData(HunterType* hunter);
/*
    Function: takeHunterTurn(void* hunter)
    Purpose: Manages the thread for a hunter structure and calls its functions
    Parameters:
        Input/output: void* hunter - A void pointer to the hunter associated with the thread being run
*/
void *takeHunterTurn(void* hunter);


/*
    Function: initEvidenceList(EvidenceListType* evList)
    Purpose: initalizes the fields for evidence lists
    Parameters: 
        Output: EvidenceListType* evList - The linked list being initalized
*/
void initEvidenceList(EvidenceListType* evList);
/*
    Function: initSharedEv(HunterEvidenceType* allEv)
    Purpose: initalizes the fields of the hunter's shared evidence list and its semaphore
    Parameters:
        Output: HunterEvidenceType* allEv - The structure containing the hunters' evidence list and its semaphore
*/
void initSharedEv(HunterEvidenceType* allEv);
/*
    Function: evNodeCleanup(EvidenceListType* evList)
    Purpose: deallocates memory used for nodes in which evidence is stored
    Parameters:
        Output EvidenceListType* evList - the linked list whose nodes are being freed up
*/
void evNodeCleanup(EvidenceListType* evList);
/*
    Function: addEv(EvidenceListType* evList, EvidenceType added_ev)
    Purpose: adds the provide evidence to the evidence linked list
    Parameters
        Output: EvidenceListType* evList - the linked list of evidence being added to
        Input: EvidenceType added_ev - The evidence being added to the linked list
*/
void addEv(EvidenceListType* evList, EvidenceType added_ev);
/*
    Function: evNodeCreator(EvidenceType evNodeData, EvidenceNodeType** node)
    Purpose: Allocates memory and adds data for a new node on the linked list
    Parameters:
        Input: EvidenceType evNodeData - the evidence stored in the node
        Output: EvidenceNodeType** node - the node storing the data being added
*/
void evNodeCreator(EvidenceType evNodeData, EvidenceNodeType** node);

//Helper Functions V
/*
    Function: roomNodeCreator(EvidenceType evNodeData, RoomNodeType** node)
    Purpose: Allocates memory and adds data for a new node on the linked list of rooms
    Parameters:
        Input: EvidenceType evNodeData - the room stored in the node
        Output: RoomNodeType** node - the node storing the data being added
*/
void roomNodeCreator(RoomType* roomNodeData, RoomNodeType** node);

/*
    Function: houseMemoryCleanup(HouseType* house)
    Purpose: Deallocates all memory used to store the house's fields in the program
    Parameters:
        Output: HouseType* house - the house structure with its fields being deallocated
*/
void houseMemoryCleanup(HouseType* house);
/*
    Function: roomDataCleanup(RoomListType* r_list)
    Purpose: Deallocates memory used to store the rooms and their fields in a room linked list
    Parameters:
        Output RoomListType* r_list - The list whose rooms are being deallocated
*/
void roomDataCleanup(RoomListType* r_list);
/*
    Function: guessed_ghost(EvidenceListType* Ev, char* str)
    Purpose: Determine the type of ghost found by the hunters with their given evidence
    Parameters:
        Input: EvidenceListType* Ev - the list of evidence collected by the hunters
        Output: char* str - a string representing the type of ghost detected
*/
void guessed_ghost(EvidenceListType* Ev, char* str);
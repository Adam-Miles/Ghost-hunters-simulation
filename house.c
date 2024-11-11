#include "defs.h"

void initHouse(HouseType* house /*HunterType* h_arr*/){
    //Sets the hunter to NULL
    house -> hunter = NULL;
    //Allocting memory for the shared evidence
    house -> shared_evidence = (HunterEvidenceType*)malloc(sizeof(HunterEvidenceType));
    //Allocting memory for the rooms
    house -> rooms = (RoomListType*)malloc(sizeof(RoomListType));

    //Initializing the rooms and shared evidence
    initRoomList(house -> rooms);
    initSharedEv(house -> shared_evidence);
}

//---------------------------------------------------------------------------------------------------------

void houseMemoryCleanup(HouseType* house){
    //Frees the room data then the nodes
    roomDataCleanup(house->rooms);
    roomNodeCleanup(house->rooms);

    //Frees the evidence nodes
    evNodeCleanup(house->shared_evidence->all_evidence);

    //Frees the all evidence 
    free(house->shared_evidence->all_evidence);

    //Frees the shared evidence
    free(house->shared_evidence);

    //Frees the rooms
    free(house->rooms);
}

//---------------------------------------------------------------------------------------------------------

void populateRooms(HouseType* house){
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");
    
    // This adds each room to each other's room lists
    // All rooms are two-way connections
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    addRoom(house->rooms, van);
    addRoom(house->rooms, hallway);
    addRoom(house->rooms, master_bedroom);
    addRoom(house->rooms, boys_bedroom);
    addRoom(house->rooms, bathroom);
    addRoom(house->rooms, basement);
    addRoom(house->rooms, basement_hallway);
    addRoom(house->rooms, right_storage_room);
    addRoom(house->rooms, left_storage_room);
    addRoom(house->rooms, kitchen);
    addRoom(house->rooms, living_room);
    addRoom(house->rooms, garage);
    addRoom(house->rooms, utility_room);
}
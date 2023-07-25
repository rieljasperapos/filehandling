#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Title Deed structure
typedef struct {
	int id;
	char name[20];
	char color[20];
	double amount[9]; 			// rent lot but empty, 1, 2, 3, 4, hotel, mortgage, house cost, hotel cost (house cost * 5) 
	int houseCount;				// will be used only if players owns already the property
	int hotelCount;				// will be used only if players owns already the property
	int mortgage;				// will be used only if players owns already the property and value is 1 - under mortgage, 0 - not in mortgage 
} TitleDeed;

typedef struct {
	char cardType[20];
	char cardDescription[100];
} Card, Cards[16];

// Used to store Title Deeds in a linkled list 
typedef struct prop {
	TitleDeed lot;
	struct prop *next;
} Prop, *PropLList, *SortedPropLList;

// Used to store Title Deeds in an array list 
typedef struct {
	TitleDeed *lot;
	int count;
	int max;
} PropAList, SortedPropAList;
// Player structure
typedef struct {
	int id;
	SortedPropLList owned;		// initially empty but insertSorted when adding ownership to a title deed based on id
	int moneyCount[7];			// 2pcs-$500, 2pcs-$100, 2pcs-$50, 6pcs-$20, 5pcs-$10, 5pcs-$5, 5pcs-$1
	double totalMoney;			// $1500
	int position;				// current position in board starts at 0
} Player;

// GameBoard structure
typedef struct {
	PropLList ownedBank;		// the collection of title deeds not yet owned by a player
	int boardLot[40];			// compromises the lot area
	int moneyCount[7];			// 30pcs-$500, 30pcs-$100, 30pcs-$50, 30pcs-$20, 30pcs-$10, 30pcs-$5, 30pcs-$1
	double totalMoney;			// $20580
	Player players[8];			// players of the game
	int playerCount;			// current count of players
	int houseCount;				// 32 houses
	int hotelCount;				// 12 hotels
	Cards chance;				// chance cards
	int currChanceTop;			// initially starts at 0 then moves once the player lands a chance lot
	Cards communityChest;		// commmunity chest cards
	int currCommunityChestTop;	// initially starts at 0 then moves once the player lands a community chest lot
	int dice[2];				// will contain the dice values (0 for the dice 1 and 1 for dice 2)
} GameBoard;

//	> 0 special(-1 community chest, -2 chance, -3 free parking, -4 income tax, -5 jail, -6 electric company, -7 water works, -8 road 1, -9 road 2, -10 road 3, -11 road 4, -12)
//  0 owned by bank, id of the player if lot is owned by a player
//  initial value of board lot 0, *0, -1, *0, -4, -8, !0, -2, !0, !0, -5, *0, -6, *0, *0, -9, !0, -1, !0, !0, -3, *0, -2, *0, *0, -10, !0, !0, -7, !0, -12, *0, *0, -1, *0, -11, -2, !0, -4, !0

// must implement
bool acquireProperty(GameBoard * board, int player_id, int property_id);
bool addPlayer(GameBoard *board, int player_id);
void initGameBoard(GameBoard *board);
void displayTitleDeeds(PropLList list);

// void ownedByBankTitleDeeds(PropLList *list) {
// 	FILE *fp;
// 	TitleDeed titles;
// 	// Owned by bank title deeds retrieves all titles from the file
// 	fp = fopen("titledeed.cis", "rb");
// 	while(fread(&titles, sizeof(TitleDeed), 1, fp)) {
// 		PropLList newNode = malloc(sizeof(Prop));
// 		newNode->lot = titles;
// 		newNode->next = *list;
// 		*list = newNode;
// 	}
// 	fclose(fp);
// }

PropLList readFile(PropLList *list, char *filename, char *mode) {
	FILE *fp;
	TitleDeed titles;
	// Owned by bank title deeds retrieves all titles from the file
	fp = fopen(filename, mode);
	while(fread(&titles, sizeof(TitleDeed), 1, fp)) {
		PropLList newNode = malloc(sizeof(Prop));
		newNode->lot = titles;
		newNode->next = *list;
		*list = newNode;
	}
	fclose(fp);

	return *list;
}

// place the other function prototypes needed


int main(int argc, char *argv[]) {
	GameBoard board;
	board.ownedBank = NULL;
	initGameBoard(&board);
	// Added 3 player
	addPlayer(&board, 69);
	printf("Player Count: %d\n", board.playerCount);
	addPlayer(&board, 420);
	printf("Player Count: %d\n", board.playerCount);
	addPlayer(&board, 19);
	printf("Player Count: %d\n", board.playerCount);

	board.players[1].owned = NULL;
	// acquireProperty(&board, board.players[0].id, 5);
	acquireProperty(&board, board.players[1].id, 6);
	acquireProperty(&board, board.players[1].id, 3);
	acquireProperty(&board, board.players[1].id, 1);
	printf("%d\n", board.boardLot[1]);
	displayTitleDeeds(board.players[1].owned);

	
	return 0;
}

void initGameBoard(GameBoard *board) {
	// retrieve titles from the file "titledeed.cis"
	// board->ownedBank = NULL;
	readFile(&board->ownedBank, "titledeed.cis", "rb");
	// printf("%s\n", board->ownedBank->lot.name);
	displayTitleDeeds(board->ownedBank);
	// Setting board lot into the designated values
	while (board->ownedBank != NULL) {
		board->boardLot[board->ownedBank->lot.id] = 0;
		board->ownedBank = board->ownedBank->next;
	}
	int i = 0;
	while (i < 40) {
		if (i == 0) {
			board->boardLot[i] = 0;
		}
		if (i == 2 || i == 17 || i == 33) { // Community Chests
			board->boardLot[i] = -1;
		} if (i == 7 || i == 22 || i == 36) { // Chance Marked as ?
			board->boardLot[i] = -2;
		} if (i == 20) {
			board->boardLot[i] = -3;
		} if (i == 4 || i == 38) {
			board->boardLot[i] = -4;
		} if (i == 10) {
			board->boardLot[i] = -5;
		} if (i == 12) {
			board->boardLot[i] = -6;
		} if (i == 28) {
			board->boardLot[i] = -7;
		} if (i == 5) {
			board->boardLot[i] = -8;
		} if (i == 15) {
			board->boardLot[i] = -9;
		} if (i == 25) {
			board->boardLot[i] = -10;
		} if (i == 35) {
			board->boardLot[i] = -11;
		} if (i == 30) {
			board->boardLot[i] = -12;
		}
		i++;
	}
	
	int j;
	for(j = 0; j < 40; j++) {
		printf("Lot[%d] : %d\n", j, board->boardLot[j]);
	}
	
	board->moneyCount[0] = 500 * 30;
	board->moneyCount[1] = 100 * 30;
	board->moneyCount[2] = 50 * 30;
	board->moneyCount[3] = 20 * 30;
	board->moneyCount[4] = 10 * 30;
	board->moneyCount[5] = 5 * 30;
	board->moneyCount[6] = 1 * 30;

	int k;
	board->totalMoney = 0;
	int total = 0;
	for (k = 0; k < 7; k++) {
		board->totalMoney += board->moneyCount[k];
	}
	printf("\nTotal Money: $%.2lf\n", board->totalMoney);

	// Initialize the players to empty
	board->playerCount = 0;
	for (int i = 0; i < 8; i++) {
		board->players[i].id = 0;
	}
	for (int i = 0; i < 8; i++) {
		if (board->players[i].id != 0) {
			board->playerCount++;
		}
	}
	printf("Player Count: %d\n", board->playerCount);

	board->houseCount = 32;
	printf("House Count: %d\n", board->houseCount);
	board->hotelCount = 12;
	printf("Hotel Count: %d\n", board->hotelCount);
}

bool addPlayer(GameBoard *board, int player_id) {
	printf("\n");
	board->players[board->playerCount].id = player_id;
	printf("Active Player #%d\n", board->playerCount + 1);
	printf("Player ID: %d\n", board->players[board->playerCount].id);
	// Starting Position set as 0
	board->players[board->playerCount].position = 0;

	// Properties owned set to empty
	board->players[board->playerCount].owned = NULL;

	// Player will initially receive 2pcs - $500, 2pcs - $100, 2pcs - $50, 6pcs - $20, 5pcs - $10, 5pcs - $5
	board->players[board->playerCount].moneyCount[0] = 500 * 2;
	board->players[board->playerCount].moneyCount[1] = 100 * 2;
	board->players[board->playerCount].moneyCount[2] = 50 * 2;
	board->players[board->playerCount].moneyCount[3] = 20 * 6;
	board->players[board->playerCount].moneyCount[4] = 10 * 5;
	board->players[board->playerCount].moneyCount[5] = 5 * 5;
	board->players[board->playerCount].moneyCount[6] = 1 * 5;

	board->players[board->players->position].totalMoney = 0;

	// Total money of the player adds up based from the money count array
	for (int i = 0; i < 7; i++) {
		board->players[board->players->position].totalMoney += board->players[board->players->position].moneyCount[i];
	}
	printf("Total Money: $%.2lf\n", board->players[board->players->position].totalMoney);
	board->playerCount++;
}

void displayTitleDeeds(PropLList list) {
	while (list != NULL) {
		printf("%d - %s - %s\n", list->lot.id, list->lot.name, list->lot.color);
		list = list->next;
	}
}

bool acquireProperty(GameBoard * board, int player_id, int property_id) {
	readFile(&board->ownedBank, "titledeed.cis", "rb");
	board->boardLot[property_id] = player_id;
	while (board->ownedBank != NULL) {
		if (board->ownedBank->lot.id == property_id) {
			SortedPropLList *trav;
			trav = &board->players[1].owned;
			while (*trav != NULL && (*trav)->lot.id > property_id) {
				trav = &(*trav)->next;
			}
			SortedPropLList newNode = malloc(sizeof(Prop));
			newNode->lot = board->ownedBank->lot;
			newNode->next = *trav;
			*trav = newNode;
		}
		board->ownedBank = board->ownedBank->next;
	}
	
	// displayTitleDeeds(list);
}





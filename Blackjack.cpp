// CardGame.cpp : Defines the entry point for the application.
//

#include "Blackjack.h"
#include <algorithm>
#include <vector>
#include <random>
#include <iostream>

using namespace std;

	class Card {
	private: 
		string rank;
		string suit;
	public: 

		Card() : rank(""), suit("") {}
		Card(string r, string s) : rank(r), suit(s) {}

		string getRank() { return rank; }
		string getSuit() { return suit; }

		void cardInfo() {
			cout << rank << " of " << suit << " ";
		}

		
	};

	class Player {

	private:
		double bank;
		string name;
		double betAmount;

	public: 
		bool isStood; // track if our player has stood
		bool isBust;  // track if our player has busted
		Player() : name("Burndi"), bank(100.0), betAmount(0) {}
		Player(string n, double b, double a) : name(n), bank(b), betAmount(a) {}

		double getBank() { return bank; }
		string getName() { return name; }
		double getBet() { return betAmount; }

		void setBetAmt(double b) { betAmount = b; }

		void adjustBalance(string outcome) { // adjust balance based on win or lose
			if (outcome == "blackjack") {
				bank += betAmount * 2.5;
			}
			else if (outcome == "win") {
				bank += betAmount;
			}
			else if (outcome == "lose") {
				bank -= betAmount;
			}
			else if (outcome == "push") {
				// no change
			}
		}
	};

	// constants
	const int HAND_MAX = 21; // used for blackjack and busting


	void createDeck(vector<Card>& d);
	void shuffleDeck(vector<Card>& d);
	void resetDeck(vector<Card>& d);  // sets deck to 0 cards
	void setBet(Player& p, double b); // player sets bet
	void dealHand(vector<Card>& d, vector<Card>& dealer, vector<Card>& player); // deals each player their 2 starting cards
	void playHand(vector<Card>& d, vector<Card>& dealer, vector<Card>& player, Player& p); // the hand is played 
	bool isFace(vector<Card> d, int i); // checks if a card is a face card or 10
	int sumHand(vector<Card> d); // sums current cards held
	void dealCard(vector<Card>& d, vector<Card>& player);
	void flushHands(vector<Card>& d, vector<Card>& p);


	int main() {

		// variables to be passed to player object
		string name = ""; 
		double amount = 0;
		double betAmt = 0;

		// deck of cards to be used for the blackjack hand
		vector<Card> deck;

		cout << "Welcome to the Casino! Please enter your name and initial balance: \n";
		cin >> name >> amount;
		Player playa(name, amount, 0);

		// create and shuffle the deck to be used for the hand
		createDeck(deck);
		shuffleDeck(deck);

		// prompt player for bet
		cout << "Greetings " << playa.getName() << "! ";
			while (playa.getBank() > 0) { // game will be played until balance reaches 0
				cout << "Please enter bet amount for the hand: ";
				cin >> betAmt;
				setBet(playa, betAmt);

				// hand starts
				vector<Card> dealer;
				vector<Card> player;
				flushHands(dealer, player); // clears previous hands
				dealHand(deck, dealer, player);

				playHand(deck, dealer, player, playa);
			}

			cout << "\nNo money left! Thanks for playing.";
	}

	void playHand(vector<Card>& d, vector<Card>& dealerHand, vector<Card>& playerHand, Player& player) { // driver for the actual hand of blackjack
		player.isBust = false;
		player.isStood = false;
		int handCount = 0; // >0 and doubling down is not an option

		cout << "\nThe dealer shows: ";  dealerHand[1].cardInfo(); cout << "\n";

		// check for player Blackjack
		if (playerHand.size() == 2 && sumHand(playerHand) == HAND_MAX && dealerHand.size() == 2 && sumHand(dealerHand) != HAND_MAX) {
			cout << "You have: "; playerHand[0].cardInfo(); cout << " | "; playerHand[1].cardInfo(); cout << "(" << sumHand(playerHand) << ")\n";
			cout << "##### Blackjack! #####\n";
			player.adjustBalance("blackjack"); // payout for blackjack
			cout << "New balance: " << player.getBank() << "\n";
		
			return;
		}
		//  can split if the 2 ranks are the same
	//	vector<Card> split;
	//	bool isSplit = false;
	//	if (playerHand[0].getRank() == playerHand[1].getRank()) {
		//	cout << "Would you like to split your hand? Y/N: ";
		//	char decision = ' ';
			//cin >> decision;
			//switch (decision) {
		//	case 'Y':
			//	split.push_back(playerHand.back());
			//	playerHand.pop_back();
			//	isSplit = true;
			//	break;
			//case 'N':
			//	break;
			//default: 
			//	cout << "Invalid Input - Assuming \"No\"";
			//}
		//}

		// show initial hand
		cout << "You have: "; 
		for (Card c : playerHand) {
			c.cardInfo(); cout << "| ";
		}
		cout << "(" << sumHand(playerHand) << ")\n";

		
		while (player.isStood == false && player.isBust == false) { //gameplay loop
			
			char decision = ' ';
			double dd = player.getBet(); // putting bet amount into dd, to be doubled if player doubles down
			
			if (handCount == 0) { cout << "\nWould you like to hit (H), double down (D), or stand (S): "; }
			else { cout << "\nWould you like to hit (H) or stand (S): "; }
			cin >> decision;

			switch (decision) {
			case 'H': // logic for hitting
				handCount++;
				dealCard(d, playerHand);
				cout << "\nYou drew: "; playerHand.back().cardInfo(); cout <<"\n";
				cout << "You have: ";
				for (Card c : playerHand) {
					c.cardInfo(); cout << "| ";
				}
				cout << "(" << sumHand(playerHand) << ")\n";
				if (sumHand(playerHand) > HAND_MAX) { player.isBust = true; } // bust detection
				if (sumHand(playerHand) == HAND_MAX) { cout << "\n"; player.isStood = true; } // automatic stand detection
				break;
			case 'D': // logic for doubling
				handCount++;
				dd *= 2;
				if (dd > player.getBank()) { cout << "Insufficient balance!\n "; break; } // check for sufficient balance to double
				player.setBetAmt(dd);
				dealCard(d, playerHand);
				if (sumHand(playerHand) > HAND_MAX) { player.isBust = true; } // bust detection
				else { player.isStood = true; } // automatic stand detection
				cout << "Card dealt: ";  playerHand.back().cardInfo(); cout << "(" << sumHand(playerHand) << ")\n\n";
				break;
				
			case 'S': // logic for standing
				player.isStood = true;
				cout << "\n" << player.getName(); cout << " stands on " << sumHand(playerHand) << "\n\n";
				break;
			default: 
				cout << "Invalid Input\n";
			}

		}
		if (player.isBust == true) {
			cout << "##### BUST #####\n";
			player.adjustBalance("lose");
			cout << "New balance: $" << player.getBank() << "\n"; // show player their new bank balance
			return;
		}
		
		// reveal dealer holdings
		cout << "Dealer holds: "; dealerHand[0].cardInfo(); cout << " | "; dealerHand[1].cardInfo(); cout << "(" << sumHand(dealerHand) << ")\n";
		
		// dealer draws to 17, standing on 17 or higher
		Player dealer; dealer.isBust = false;
		if (sumHand(dealerHand) < 17) { cout << "Dealer draws cards: \n"; } 

		while (sumHand(dealerHand) < 17) { // dealer draws cards while sum < 17
			dealCard(d, dealerHand);
			dealerHand.back().cardInfo(); cout << "(" << sumHand(dealerHand) << ")\n";
			if (sumHand(dealerHand) > HAND_MAX) { dealer.isBust = true;  cout << "#### DEALER BUST ####\n"; break; } // if dealer's holdings are over 21, bust
			else { dealer.isBust = false; }
		}


		// logic for determining winner
		if (sumHand(playerHand) == sumHand(dealerHand)) { player.adjustBalance("push"); cout << "#### PUSH ####\n\n"; }
		else if (sumHand(playerHand) < sumHand(dealerHand) && dealer.isBust == false) { player.adjustBalance("lose"); cout << "#### LOSE ####\n\n"; }
		else if (sumHand(playerHand) >  sumHand(dealerHand) || dealer.isBust == true) { player.adjustBalance("win"); cout << "#### WIN ####\n\n"; }
	
		cout << "New balance: $" << player.getBank() << "\n";
	}

	// "vector<Card> d" refers to the active deck
	void dealHand(vector<Card>& d, vector<Card>& dealer, vector<Card>& player) { // deal hand to dealer and player
		// always start with a fresh deck, counters be damned
		resetDeck(d);
		createDeck(d);
		shuffleDeck(d);

		for (int i = 0; i < 2; i++) {
			player.push_back(d.back()); // add last card of deck to player's hand
			d.pop_back();				// remove added card from deck
			dealer.push_back(d.back());
			d.pop_back();
		}
	}
	
	void createDeck(vector<Card>& d) {
		
		string suits[4] = { "Hearts", "Spades", "Diamonds", "Clubs" };
		string ranks[13] = { "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King" };
		for (int r = 0; r < 13; r++) {
			for (int s = 0; s < 4; s++) {
				d.push_back(Card(ranks[r], suits[s]));
			}
		}
	}
	
	void shuffleDeck(vector<Card> &d) { // shuffles the deck
		random_device rd;
		shuffle(d.begin(), d.end(), rd);
	}

	void resetDeck(vector<Card>& d) { // clears the vector of cards
		d.clear();
	}

	void setBet(Player& p, double b) {
		while (b > p.getBank()) { // if bet amount is greater than bank, prompt player for lower value
			cout << "Slow your roll there partner! Enter an amount less than " << p.getBank() << ":\n";
			cin >> b;
		}
		p.setBetAmt(b);
	}

	bool isFace(vector<Card> d, int i) { // checks if a card is a face card - equivalent to 10 "points", where i is the index to check
		if (d[i].getRank() == "King") {
			return true;
		}
		else if (d[i].getRank() == "Jack") {
			return true;
		}
		else if (d[i].getRank() == "Queen") {
			return true;
		}
		else if (d[i].getRank() == "10") {
			return true;
		}
		else {
			return false;
		}
	}

	int sumHand(vector<Card> d) {
		int aces = 0;
		int total = 0;
		for (int i = 0; i < d.size(); i++) {
			if (d[i].getRank() == "Ace") {
				total += 11;
				aces++;
			}
			else if (d[i].getRank() == "2") {
				total += 2;
			}
			else if (d[i].getRank() == "3") {
				total += 3;
			}
			else if (d[i].getRank() == "4") {
				total += 4;
			}
			else if (d[i].getRank() == "5") {
				total += 5;
			}
			else if (d[i].getRank() == "6") {
				total += 6;
			}
			else if (d[i].getRank() == "7") {
				total += 7;
			}
			else if (d[i].getRank() == "8") {
				total += 8;
			}
			else if (d[i].getRank() == "9") {
				total += 9;
			}
			else if (isFace(d, i)) {
				total += 10;
			}
			else {
				
			}
			while (total > HAND_MAX && aces > 0) {
				total -= 10;
				aces--;
			}
		}
		return total;
	}

	void dealCard(vector<Card>& d, vector<Card>& drawer) {
		drawer.push_back(d.back());
		d.pop_back();
	}

	void flushHands(vector<Card>& d, vector<Card>& p) {
		d.clear();
		p.clear();
	}
/*
 * @file    deckofcards.ino
 * @author  Jared Pellegrini <jpellegrini@gmail.com>
 * @copyright Copyright 2024 Jared Pellegrini
 * @license Apache License, Version 2.0 - https://www.apache.org/licenses/LICENSE-2.0
 * @brief   Selects random cards one at a time from a standard deck, with or without jokers.
 * @version 0.2
 * @date    2024-03-18 (Created 2024-03-17)
 *
 * Targets the M5 Cardputer.
 *
 * A card will only be drawn from the deck once per shuffle.
 * Toggling jokers will force a re-shuffle.
 * 
 * NOTES:
 * 1. String (Capital String) is an Arduino String not a C++ std::string
 *
 * CHANGELOG
 * v0.3: Reset cardsToShow on shuffle
 * v0.2: Display up to 8 drawn cards. [C] to clear.
 * v0.1: First version.
 */

#include "M5Cardputer.h"
#include <vector>

const uint8_t MAJOR_VERSION = 0;
const uint8_t MINOR_VERSION = 2;
const uint8_t MAX_CARDS_TO_SHOW = 8;

uint8_t fontHeight;
uint8_t displayHeight;
uint8_t displayWidth;

bool allowJokers = 1;
uint8_t cardsToShow = 0;

String suits[4] = {"Clubs", "Diamonds", "Hearts", "Spades"};
std::vector<String> deck;
std::vector<String> drawn;

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextFont(&fonts::FreeSans9pt7b);
  M5Cardputer.Display.setTextSize(1);
  fontHeight = M5Cardputer.Display.fontHeight(&fonts::FreeSans9pt7b); // 9
  displayWidth = M5Cardputer.Display.width();
  displayHeight = M5Cardputer.Display.height();
  
  // Initialize the random seed
  randomSeed(analogRead(0));
  shuffleDeck();

  displaySplash();
}

void loop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isKeyPressed('/') || M5Cardputer.Keyboard.isKeyPressed('?')) {
      displaySplash();
    } else if (M5Cardputer.Keyboard.isKeyPressed(' ')) {
      displayDeck();
    } else if (M5Cardputer.Keyboard.isKeyPressed('d')) {
      drawCard();
    } else if (M5Cardputer.Keyboard.isKeyPressed('c')) {
      cardsToShow = 0;
      displayDeck();
    } else if (M5Cardputer.Keyboard.isKeyPressed('s')) {
      shuffleDeck();
      displayDeck();
    } else if (M5Cardputer.Keyboard.isKeyPressed('j')) {
      allowJokers = !allowJokers;
      shuffleDeck();
      displayDeck();
    }
  }
}

void shuffleDeck() {
  //reset the number of cards shown
  cardsToShow = 0;
  //empty the drawn pile
  drawn.clear();
  //empty the deck
  deck.clear();
  // insert 2-10, then J, Q, K, A for each suit
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 2; j <= 10; j++) { deck.push_back(String(j) + "-" + suits[i]); }
    deck.push_back("J-" + suits[i]);
    deck.push_back("Q-" + suits[i]);
    deck.push_back("K-" + suits[i]);
    deck.push_back("A-" + suits[i]);
  }
  // add the two jokers if allowed
  if (allowJokers) {
    deck.push_back("Joker-RED");
    deck.push_back("Joker-BLACK");
  }
}

void displaySplash() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setTextDatum(textdatum_t::top_center);
  M5Cardputer.Display.setTextColor(BLACK, WHITE);
  M5Cardputer.Display.drawString(" Deck of Cards ", displayWidth / 2, 1 * fontHeight);
  M5Cardputer.Display.setTextColor(DARKGREEN);
  M5Cardputer.Display.drawString("by Jared Pellegrini", displayWidth / 2, 3 * fontHeight);
  M5Cardputer.Display.setTextColor(YELLOW);
  M5Cardputer.Display.drawString("press space to continue", displayWidth / 2, 5 * fontHeight);

  M5Cardputer.Display.setTextColor(DARKGREY);
  M5Cardputer.Display.setTextDatum(textdatum_t::bottom_left);
  M5Cardputer.Display.drawString(" ? returns home", 0, displayHeight);
  M5Cardputer.Display.setTextColor(WHITE);
  M5Cardputer.Display.setTextDatum(textdatum_t::bottom_right);
  M5Cardputer.Display.drawString("v" + String(MAJOR_VERSION) + "." + String(MINOR_VERSION) + " ", displayWidth, displayHeight);
}

void displayDeck() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setTextColor(deck.size() > 0 ? DARKGREEN : MAROON);
  M5Cardputer.Display.setTextDatum(textdatum_t::top_left);
  M5Cardputer.Display.drawString("Cards Remaining: " + String(deck.size()), 0, 0);
  M5Cardputer.Display.setTextColor(DARKGREY);
  M5Cardputer.Display.setTextDatum(textdatum_t::top_right);
  M5Cardputer.Display.drawString("[D]raw", displayWidth, 0);
  M5Cardputer.Display.setTextDatum(textdatum_t::top_left);

  if (cardsToShow > 0 && cardsToShow <= drawn.size()) {
    for (uint8_t i = 1; i <= cardsToShow; i++) {
      //split into two columns
      if (i <= MAX_CARDS_TO_SHOW / 2) {
        M5Cardputer.Display.drawString(displayCard(drawn.at(drawn.size() - i)), 0, (i + 1) * fontHeight);
      } else if (i <= MAX_CARDS_TO_SHOW) {
        M5Cardputer.Display.drawString(displayCard(drawn.at(drawn.size() - i)), displayWidth / 2, (i - 3) * fontHeight);
      }
    }
  }

  M5Cardputer.Display.setTextColor(DARKGREY);
  M5Cardputer.Display.setTextDatum(textdatum_t::bottom_left);
  String s = allowJokers ? "[J]okers=on" : "[J]okers=off";
  M5Cardputer.Display.drawString(s, 0, displayHeight);
  M5Cardputer.Display.setTextDatum(textdatum_t::bottom_center);
  M5Cardputer.Display.drawString("      [C]lear", displayWidth / 2, displayHeight);
  M5Cardputer.Display.setTextDatum(textdatum_t::bottom_right);
  M5Cardputer.Display.drawString("[S]huffle", displayWidth, displayHeight);
}

void drawCard() {
  if (!deck.empty()) {
    // generate a random index, pull that card from the deck and add it to the drawn pile
    int8_t cardIndex = random(0, deck.size());
    drawn.push_back(deck.at(cardIndex));
    deck.erase(deck.begin()+cardIndex);
    if (cardsToShow < MAX_CARDS_TO_SHOW) { cardsToShow++; }
    displayDeck();
  }
}

String displayCard(String thisCard) {
  if (thisCard == "Joker-RED") {
    M5Cardputer.Display.setTextColor(WHITE, RED);
    return " JOKER ";
  } else if (thisCard == "Joker-BLACK") {
    M5Cardputer.Display.setTextColor(BLACK, WHITE);
    return " JOKER ";
  } else if (thisCard.endsWith("Clubs") || thisCard.endsWith("Spades")) {
    M5Cardputer.Display.setTextColor(WHITE);
  } else if (thisCard.endsWith("Diamonds") || thisCard.endsWith("Hearts")) {
    M5Cardputer.Display.setTextColor(RED);
  } else {
    M5Cardputer.Display.setTextColor(DARKGREEN);
  }
  return thisCard;
}

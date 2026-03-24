import numpy as np
import eval7
from eval7 import rangestring

import random

def hole_simulation(card1, card2,n =100000, num_players = 1, range_string = None):

    if range_string is None:

        if num_players < 1 or num_players > 9:
            return "Invalid: num_players must be between 1 and 9"
        
        
        c1 = eval7.Card(card1)
        c2 = eval7.Card(card2)
        if c1 == c2:
            return "Invalid card: duplicate"

        wins = 0
        hero_hand = [c1, c2]
        deck = eval7.Deck()
        deck.cards.remove(c1)
        deck.cards.remove(c2)
        stripped = deck.cards

        for _ in range(0, n):


            deck.cards = list(stripped)
            deck.shuffle()
            villian_hands = [deck.deal(2) for _ in range(num_players)]
            Community_cards = deck.deal(5)
            hero7 = hero_hand + Community_cards
            villian_scores = [eval7.evaluate(hand + Community_cards) for hand in villian_hands]
            hero_score = eval7.evaluate(hero7)
            best_villain_score = max(villian_scores)

            if hero_score > best_villain_score:
                wins+=1

        
        return (wins/n) * 100
    else:

        if num_players < 1 or num_players > 9:
            return "Invalid: num_players must be between 1 and 9"
        
        
        c1 = eval7.Card(card1)
        c2 = eval7.Card(card2)

        dead_cards = [c1, c2]
        villian_range = get_range_combos(range_string="AA, KK, QQ", dead_cards=dead_cards)

        if c1 == c2:
            return "Invalid card: duplicate"

        wins = 0
        hero_hand = [c1, c2]
        deck = eval7.Deck()
        deck.cards.remove(c1)
        deck.cards.remove(c2)
        stripped = deck.cards

        for _ in range(10000):

            deck.cards = list(stripped)
            villian_hands = [list(random.choice) for _ in range(num_players)]
            
        

def flop_simulation(hole1, hole2, flop1, flop2, flop3, n= 100000, num_players = 1):

    if num_players < 1 or num_players > 9:
        return "Invalid: num_players must be between 1 and 9"
    
    h1 = eval7.Card(hole1)
    h2 = eval7.Card(hole2)
    f1 = eval7.Card(flop1)
    f2 = eval7.Card(flop2)
    f3 = eval7.Card(flop3)
    wins = 0
    hero_hand = [h1, h2]
    flop = [f1, f2, f3]
    dead = hero_hand + flop
    deck = eval7.Deck()
    for c in dead:
            deck.cards.remove(c)
    stripped = deck.cards
    for _ in range(0, n):
        deck.cards = list(stripped)
        deck.shuffle()
        villian_hands = [deck.deal(2) for _ in range(num_players)]
        Community_cards = flop + deck.deal(2)
        villian_scores = [eval7.evaluate(hand + Community_cards) for hand in villian_hands]
        hero7 = hero_hand + Community_cards
        hero_score = eval7.evaluate(hero7)
        best_villian_score  = max(villian_scores)
        if hero_score > best_villian_score:
            wins+=1
    return (wins/n) * 100

def turn_simulation(hole1, hole2, flop1, flop2, flop3, turn, n = 100000, num_players = 1):

    if num_players < 1 or num_players > 9:
        return "Invalid: num_players must be between 1 and 9"

    h1 = eval7.Card(hole1)
    h2 = eval7.Card(hole2)
    f1 = eval7.Card(flop1)
    f2 = eval7.Card(flop2)
    f3 = eval7.Card(flop3)
    t = eval7.Card(turn)
    wins = 0
    hero_hand = [h1, h2]
    flop_turn = [f1,f2,f3,t]
    dead = hero_hand + flop_turn
    deck = eval7.Deck()
    for c in dead:
        deck.cards.remove(c)
    stripped = deck.cards
    for _ in range(n):

        deck.cards = list(stripped)
        deck.shuffle()
        villian_hands = [deck.deal(2) for _ in range(num_players)]
        Community_Cards = flop_turn + deck.deal(2)

        hero7 = hero_hand + Community_Cards
        villian_score = [eval7.evaluate(hand+Community_Cards) for hand in villian_hands]

        hero_score = eval7.evaluate(hero7)
        best_villian_score = max(villian_score)

        if hero_score > best_villian_score:
            wins+=1
    
    return (wins/n) * 100

def river_simulation(hole1, hole2, flop1, flop2, flop3, turn, river, n= 100000, num_players = 1):

    if num_players < 1 or num_players > 9:
        return "Invalid: num_players must be between 1 and 9"

    h1 = eval7.Card(hole1)
    h2 = eval7.Card(hole2)
    f1 = eval7.Card(flop1)
    f2 = eval7.Card(flop2)
    f3 = eval7.Card(flop3)
    t = eval7.Card(turn)
    r = eval7.Card(river)
    wins = 0
    hero_hand = [h1, h2]
    full_community = [f1,f2,f3,t, r ]
    hero7 = hero_hand + full_community
    dead = hero7
    hero_score = eval7.evaluate(hero7)
    deck = eval7.Deck()
    for c in dead:
        deck.cards.remove(c)
    stripped = deck.cards

    for _ in range(n):
        deck.cards = list(stripped)
        deck.shuffle()
        villian_hands = [deck.deal(2) for _ in range(num_players)]
        villian_score = [eval7.evaluate(hand + full_community) for hand in villian_hands]
        best_villian_score = max(villian_score)
        if hero_score > best_villian_score:
            wins+=1
    
    return (wins/n) * 100

def get_range_combos(range_string, dead_cards):

    if not rangestring.validate_string(range_string):
        return "Invalid range string"
    
    combos = rangestring.string_to_hands(range_string)

    filtered = [(hand, w) for hand, w in combos 
                if hand[0] not in dead_cards and hand[1] not in dead_cards]
    return filtered
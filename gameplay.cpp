#include "cardgame.hpp"

int cardgame::random(const int range) {
  auto seed_iterator = _seed.begin();

  if (seed_iterator == _seed.end()) {
    seed_iterator = _seed.emplace(_self, [&](auto &seed) {});
  }

  int prime = 65537;
  auto new_seed_value = (seed_iterator->value + now()) % prime;

  _seed.modify(seed_iterator, _self,
               [&](auto &s) { s.value = new_seed_value; });

  int random_result = new_seed_value % range;
  return random_result;
}

void cardgame::draw_one_card(vector<uint8_t> &deck, vector<uint8_t> &hand) {
  int deck_card_idx = random(deck.size());

  int first_empty_slot = -1;
  for (int i = 0; i <= hand.size(); i++) {
    auto id = hand[i];
    if (card_dict.at(id).type == EMPTY) {
      first_empty_slot = i;
      break;
    }
  }
  eosio_assert(first_empty_slot != -1, "No empty slot in the player's hand");

  hand[first_empty_slot] = deck[deck_card_idx];

  deck.erase(deck.begin() + deck_card_idx);
}

int cardgame::calculate_attack_point(const card &card1, const card &card2) {
  int result = card1.attack_point;
  return result;
}

int cardgame::ai_best_card_win_strategy(const int ai_attack_point,
                                        const int player_attack_point) {

  eosio::print("Best Card Wins");
  if (ai_attack_point > player_attack_point)
    return 3;
  if (ai_attack_point < player_attack_point)
    return -2;
  return -1;
}

int cardgame::ai_min_loss_strategy(const int ai_attack_point,
                                   const int player_attack_point) {

  eosio::print("Minimum Losses");
  if (ai_attack_point > player_attack_point)
    return 1;
  if (ai_attack_point < player_attack_point)
    return -4;
  return -1;
}

int cardgame::ai_points_tally_strategy(const int ai_attack_point,
                                       const int player_attack_point) {
  eosio::print("Points Tally");
  return ai_attack_point - player_attack_point;
}

int cardgame::ai_loss_prevention_strategy(const int8_t life_ai,
                                          const int ai_attack_point,
                                          const int player_attack_point) {
  eosio::print("Loss Prevention");
  if (life_ai + ai_attack_point - player_attack_point > 0)
    return 1;
  return 0;
}

int cardgame::calculate_ai_card_score(const int strategy_idx,
                                      const int8_t life_ai, const card &ai_card,
                                      const vector<uint8_t> hand_player) {
  int card_score = 0;
  for (int i = 0; i < hand_player.size(); i++) {
    const auto player_card_id = hand_player[i];
    const auto player_card = card_dict.at(player_card_id);

    int ai_attack_point = calculate_attack_point(ai_card, player_card);
    int player_attack_point = calculate_attack_point(player_card, ai_card);

    switch (strategy_idx) {
    case 0: {
      card_score +=
          ai_best_card_win_strategy(ai_attack_point, player_attack_point);
      break;
    }
    case 1: {
      card_score += ai_min_loss_strategy(ai_attack_point, player_attack_point);
      break;
    }
    case 2: {
      card_score +=
          ai_points_tally_strategy(ai_attack_point, player_attack_point);
      break;
    }
    default: {
      card_score += ai_loss_prevention_strategy(life_ai, ai_attack_point,
                                                player_attack_point);
      break;
    }
    }
  }
  return card_score;
}

int cardgame::ai_choose_card(const game &game_data) {
  int available_strategies = 4;
  if (game_data.life_ai > 2)
    available_strategies--;
  int strategy_idx = random(available_strategies);

  int chosen_card_idx = -1;
  int chosen_card_score = std::numeric_limits<int>::min();

  for (int i = 0; i < game_data.hand_ai.size(); i++) {
    const auto ai_card_id = game_data.hand_ai[i];
    const auto ai_card = card_dict.at(ai_card_id);

    if (ai_card.type == EMPTY)
      continue;

    auto card_score = calculate_ai_card_score(strategy_idx, game_data.life_ai,
                                              ai_card, game_data.hand_player);

    if (card_score > chosen_card_score) {
      chosen_card_score = card_score;
      chosen_card_idx = i;
    }
  }

  return chosen_card_idx;
}
#include "gameplay.cpp"

void cardgame::login(name username)
{
  require_auth(username);

  auto user_iterator = _users.find(username.value);
  if (user_iterator == _users.end())
  {
    user_iterator = _users.emplace(
        username, [&](auto &new_user) { new_user.username = username; });
  }
}

void cardgame::startgame(name username)
{
  require_auth(username);

  auto &user = _users.get(username.value, "User doesn't exist");

  _users.modify(user, username, [&](auto &modified_user) {
    game game_data;

    for (uint8_t i = 0; i < 4; i++)
    {
      draw_one_card(game_data.deck_player, game_data.hand_player);
      draw_one_card(game_data.deck_ai, game_data.hand_ai);
    }

    modified_user.game_data = game_data;
  });
}

void cardgame::endgame(name username)
{
  require_auth(username);

  auto &user = _users.get(username.value, "User does not exist");

  _users.modify(user, username, [&](auto &modified_user) {
    modified_user.game_data = game();
  });
}

void cardgame::playcard(name username, uint8_t player_card_idx)
{
  require_auth(username);

  eosio_assert(player_card_idx < 4, "playcard: Invalid hand index");

  auto &user = _users.get(username.value, "User doesn't exist");

  eosio_assert(user.game_data.status == ONGOING,
               "playcard: This game has ended. Please start a new one");
  eosio_assert(user.game_data.selected_card_player == 0,
               "playcard: The player has played his card this turn!");

  _users.modify(user, username, [&](auto &modified_user) {
    game &game_data = modified_user.game_data;

    // Assign the selected card from the player's hand
    game_data.selected_card_player = game_data.hand_player[player_card_idx];
    game_data.hand_player[player_card_idx] = 0;

    int ai_card_idx = ai_choose_card(game_data);
    game_data.selected_card_ai = game_data.hand_ai[ai_card_idx];
    game_data.hand_ai[ai_card_idx] = 0;

    resolve_selected_cards(game_data);

    update_game_status(modified_user);
  });
}

void cardgame::nextround(name username)
{
  require_auth(username);

  auto &user = _users.get(username.value, "User does not exist");

  eosio_assert(user.game_data.status == ONGOING, "nextround: This game has ended. Please start a new game.");

  eosio_assert(user.game_data.selected_card_player != 0 && user.game_data.selected_card_ai != 0, "nextround: Please play a card first.");

  _users.modify(user, username, [&](auto &modified_user) {
    game &game_data = modified_user.game_data;

    game_data.selected_card_player = 0;
    game_data.selected_card_ai = 0;
    game_data.life_lost_player = 0;
    game_data.life_lost_ai = 0;

    if (game_data.deck_player.size() > 0)
      draw_one_card(game_data.deck_player, game_data.hand_player);
    if (game_data.deck_ai.size() > 0)
      draw_one_card(game_data.deck_ai, game_data.hand_ai);
  });
}

EOSIO_DISPATCH(cardgame, (login)(startgame)(playcard)(nextround)(endgame))

#include <Player.h>

Player::Player() {}

Player::Player(const int& id, const String& nickname) {
    _playerID = id;
    _nickname = nickname;    
}

int Player::getPlayerID() {
    return _playerID;
}

void Player::setPlayerID(int id) {
    _playerID = id;
}

String Player::getNickname() {
    return _nickname;
}

void Player::setNickname(String nickname) {
    _nickname = nickname;
}

int Player::getScore() {
    return _score;
}

void Player::setScore(int score) {
    _score = score;
}
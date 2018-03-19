#include <Player.h>

Player::Player() {}

Player::Player(const int& id, const String& nickname) {
    _playerID = id;
    _nickname = nickname;  
	_alive = true;
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

bool Player::isAlive() {
	return _alive;
}

void Player::die() {
	_alive = false;
}
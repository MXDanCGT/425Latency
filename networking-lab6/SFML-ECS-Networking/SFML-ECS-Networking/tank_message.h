#pragma once

// A simple tank update message
// FIXME: Consider what else we need to send and include it here.
struct TankMessage {
	// The coordinates of the tank within the game world.

	//TankMessage(float gTime, int gID, sf::Vector2f gPos, sf::Vector2f gRot) : time(gTime), id(gID), position(gPos), rotation(gRot) {};

	float time = 0;
	int id = 0;
	// Changed position to a vector2 for ease of interpolation calculations
	sf::Vector2f position = {0.f, 0.f};

	//Rotation of the tank & barrel - [0] -> tank rotation, [1] -> barrel rotation
	sf::Vector2f rotation = { 0.f, 0.f };
};
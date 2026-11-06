#pragma once

#include <SFML\Network.hpp>
#include <deque>

enum PredictionMethod { None, Linear, Quadratic, Cubic };
enum ConnState {NotReady, Ready, Disconnected};

// Fixme:: ideally we should link specific tank/character to each connection,
// rather than assuming the order in which connections are made is the same as
// the order in which tanks/characters are stored in the game.

// This could also be made int oa class, given the increased functionality.
struct Connection
{

	std::unique_ptr<sf::TcpSocket> socket;
	ConnState state = Disconnected;

	std::deque<TankMessage> message_history;
	float inter_t = 0.f; // This value goes from 0.f to 1.f during interpolation and will be accumulated based on time delta.

	//Function for handling the increase of step and inter_t - to ensure this happens once an update, not multiple times for both interp funcs
	void UpdateInterp(float dt)
	{
		// Calculate interpolation step
		float step = 0.f;

		if (message_history.size() > 1) // Make sure we have at least 2 messages for Linear interpolation.
			step = 1.f / ((message_history[0].time - message_history[1].time) / dt);
		else
			step = 0.f;

		// Add step to inter_t
		inter_t += step;

		// Clamp inter_t to max of 1.f
		if (inter_t > 1.f)
			inter_t = 1.f;
	}

	// Interpolate position based on selected method and value of t. Return TankMessage with updated position.
	sf::Vector2f InterpPos(PredictionMethod Method, sf::Vector2f PredPos) {

		switch (Method)
		{

		case PredictionMethod::None:
			return message_history[0].position;
			break;
		case PredictionMethod::Linear:
			if (message_history.size() > 1) { // Make sure we have at least 2 messages for Linear interpolation.
				// Calculate new position by interpolating between last two known positons.
				return (1 - inter_t) * message_history[0].position + inter_t * PredPos;
			}
			else
				return PredPos; // Just return the last known position.
			break;
		default:
			return PredPos;
			break;
		}
	}

	//Interpolate the rotation of the tank body and barrel
	sf::Vector2f InterpRot(PredictionMethod Method)
	{
		switch (Method)
		{

		case PredictionMethod::None:
			return message_history[0].rotation;
			break;
		case PredictionMethod::Linear:
			if (message_history.size() > 1) { // Make sure we have at least 2 messages for Linear interpolation.
				// Calculate new position by interpolating between last two known positons.
				//return (message_history[1].rotation + (message_history[0].rotation - message_history[1].rotation) * inter_t);
				//^ v0 + t(v1 - v0)

				return (1 - inter_t) * message_history[1].rotation + inter_t * message_history[0].rotation;
			}
			else
				return message_history[0].rotation; // Just return the last known position.
			break;
		default:
			return message_history[0].rotation;
			break;
		}
	}

	auto PredictPos(float GameTime, PredictionMethod Method) -> const sf::Vector2f //Lets try this trailing types thing?
	{
		//y = mx + c, construct linear equation where y is the position and x is the time
		if (message_history.size() <= 1)
			return sf::Vector2f();


		const sf::Vector2f M = (message_history[0].position - message_history[1].position) / (message_history[0].time - message_history[1].time);
		const sf::Vector2f C = (message_history[0].position - M * message_history[0].time);
		//Now we've constructed our equation for a line, is it as simple as putting us where we'd be in the exact same step again?

		return M * GameTime + C;
	}


};
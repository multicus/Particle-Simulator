#include "Particle.h"
#include "Constants.h"
using namespace Settings;
typedef Particle::Memento Memento;

Particle::Particle()
{
	throw "Direct instantiation of 'Particle' class not allowed";
}

Particle::Particle(ParticleType* type, int speed, sf::Vector2f direction)
	: mType{ type }
	, mSpeed{speed}
	, mDirection{normalize(direction)} // normalize direction so speed values have the same weight in all objects
	, mParent{NULL}
	, mChildren{}
	, mLocalPosition{0,0}
	, mId{counter++}
{
}

/*
Memento Class definitions
*/
Memento::Memento(ParticleState state)
{
	mState = state;
}
ParticleState Memento::getSavedState()
{
	return mState;
}

/*
Memento Pattern method definitions
*/
Memento Particle::saveToMemento()
{
	ParticleState newState;
	newState.position = this->getPosition();
	newState.direction = this->mDirection;

	return Memento(newState);
}
void Particle::restoreFromMemento(Memento memento)
{
	ParticleState state = memento.getSavedState();
	setPosition(state.position);
	mDirection = state.direction;
}

int Particle::getId()
{
	return mId;
}

/*
Particle method definitions
*/
void Particle::update(float dt)
{
	sf::Vector2f pos;
	sf::Vector2f newDirection;
	float shapeRadius = getWidth() / 2;

	// move object 
	if (mParent == NULL)
		move(sf::Vector2f(mSpeed*mDirection.x*dt, mSpeed* mDirection.y*dt));
	else
		setPosition(mParent->getPosition() + mLocalPosition);
	
	pos = getPosition();

	// check if particle has hit window border
	if (pos.x + shapeRadius >= Settings::WINDOW_X)
	{
		mDirection.x *= -1;
		setPosition(Settings::WINDOW_X - shapeRadius, getPosition().y);
	}
	if (pos.x <= 0)
	{
		mDirection.x *= -1;
		setPosition(0, getPosition().y);
	}
	if (pos.y + shapeRadius >= Settings::WINDOW_Y)
	{
		mDirection.y *= -1;
		setPosition(getPosition().x, Settings::WINDOW_Y - shapeRadius);
	}
	if (pos.y <= 0)
	{
		mDirection.y *= -1;
		setPosition(getPosition().x, 0);
	}


	// update children
	for (Particle* child : mChildren)
		child->update(dt);
}

void Particle::attachChild(Particle * child, sf::Vector2f offset)
{
	mChildren.push_back(child);
	child->mParent = this;
	child->setPosition(getPosition() + offset);

	child->mLocalPosition = offset;
}

void Particle::setPosition(float x, float y)
{
	Transformable::setPosition(x+mLocalPosition.x, y+mLocalPosition.y);
	for (auto& child : mChildren)
		child->setPosition(getPosition().x, getPosition().y);
}

void Particle::setPosition(const sf::Vector2f position)
{
	Transformable::setPosition(position+mLocalPosition);
	for (auto& child : mChildren)
		child->setPosition(getPosition());
}

int Particle::getWidth()
{
	return mType->mShape->getGlobalBounds().width;
}

Particle::~Particle()
{
	for (Particle* child : mChildren)
		delete child;
}

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply transform of current node
	states.transform *= getTransform();

	// draw flyweight shape and children with changed transform
	mType->draw(target, states);

	// draw children
	for (const Particle* child : mChildren)
		child->mType->draw(target, states);
}

sf::Vector2f Particle::normalize(const sf::Vector2f& v)
{
	float vLength = sqrtf(pow(v.x, 2) + pow(v.y, 2));
	return sf::Vector2f(v.x / vLength, v.y / vLength);
}

int Particle::counter = 0;	// intitialize static counter for IDs

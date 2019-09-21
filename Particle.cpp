#include "Particle.h"
#include "Constants.h"
using namespace Settings;

Particle::Particle()
{
	throw "Direct instantiation of 'Particle' class not allowed";
}

Particle::Particle(float speed, sf::Vector2f direction, sf::Shape* shape)
	: mSpeed{speed}
	, mDirection{direction}
	, mParent{NULL}
	, mChildren{}
	, mLocalPosition{0,0}
	, mShape{shape}
{
}

void Particle::update(float dt)
{
	sf::Vector2f pos;
	sf::Vector2f newDirection;

	// move object 
	if (mParent == NULL)
		move(sf::Vector2f(mSpeed*mDirection.x*dt, mSpeed* mDirection.y*dt));
	else
		setPosition(mParent->getPosition() + mLocalPosition);
	pos = getPosition();

	// check if particle has hit window border
	if (pos.x >= Settings::WINDOW_X || pos.x <= 0)
	{
		mDirection.x *= -1;
		setPosition(Settings::WINDOW_X, getPosition().y);
	}
	if (pos.y >= Settings::WINDOW_Y || pos.y <= 0)
	{
		mDirection.y *= -1;
		setPosition(getPosition().x, Settings::WINDOW_Y);
	}

	// update children
	for (Particle* child : mChildren)
		child->update(dt);
}

void Particle::setTexture(const sf::Texture& tex)
{
	mShape->setTexture(&tex);
}

void Particle::attachChild(Particle * child, sf::Vector2f offset)
{
	mChildren.push_back(child);
	child->mParent = this;

	child->mLocalPosition = offset;
}

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply transform of current node
	states.transform *= getTransform();

	// draw node and children with changed transform
	target.draw(*mShape, states);

	// draw children
	for (const Particle* child : mChildren)
		child->draw(target, states);
}
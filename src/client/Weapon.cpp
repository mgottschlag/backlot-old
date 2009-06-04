/*
Copyright (C) 2009  Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Weapon.hpp"
#include "Engine.hpp"
#include "support/tinyxml.h"

#include <string>
#include <iostream>

namespace backlot
{
	Weapon::Weapon() : ReferenceCounted()
	{
		sound = new Sound();
	}
	Weapon::~Weapon()
	{
		// Remove from loaded maps
		std::map<std::string, Weapon*>::iterator it = weapons.find(name);
		if (it != weapons.end())
		{
			weapons.erase(it);
		}
	}

	SharedPointer<Weapon> Weapon::get(std::string name, int id)
	{
		// Get already loaded weapon
		std::map<std::string, Weapon*>::iterator it = weapons.find(name);
		if (it != weapons.end())
		{
			it->second->setID(id);
			return it->second;
		}
		// Load map
		WeaponPointer weapon = new Weapon();
		if (!weapon->load(name))
		{
			return 0;
		}
		weapon->setID(id);
		return weapon;
	}

	SharedPointer<Weapon> Weapon::get(int id)
	{
		// TODO: This is quite slow, better create a second map
		std::map<std::string, Weapon*>::iterator it = weapons.begin();
		while (it != weapons.end())
		{
			if (it->second->getID() == id)
			{
				return it->second;
			}
			it++;
		}
		return 0;
	}

	bool Weapon::load(std::string name)
	{
		// Remove from loaded maps
		std::map<std::string, Weapon*>::iterator it = weapons.find(name);
		if (it != weapons.end())
		{
			weapons.erase(it);
		}
		// Load weapon
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/weapons/" + name + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << name << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("weapon");
		if (!node)
		{
			std::cerr << "Parser error: <weapon> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Textures
		const char *playerimage = root->Attribute("playerimage");
		if (!playerimage)
		{
			std::cerr << "Weapon player image missing." << std::endl;
			return false;
		}
		playertexture = new Texture(ETF_Linear);
		playertexture->load(std::string("sprites/") + playerimage);
		const char *image = root->Attribute("image");
		if (!image)
		{
			std::cerr << "Weapon image missing." << std::endl;
			return false;
		}
		texture = new Texture(ETF_Linear);
		texture->load(std::string("sprites/") + image);
		// Get attributes
		rate = -1;
		TiXmlNode *attributenode = root->FirstChild("attributes");
		while (attributenode)
		{
			TiXmlElement *attributes = attributenode->ToElement();
			if (attributes)
			{
				if (attributes->Attribute("rate", &rate) == 0)
				{
					std::cerr << "No rate of fire available." << std::endl;
					return false;
				}
				attributes->QueryFloatAttribute("deviation", &deviation);
				if (attributes->Attribute("magazines", &magazines) == 0
					|| attributes->Attribute("magazinesize", &magazinesize) == 0)
				{
					std::cerr << "No magazine info available." << std::endl;
					return false;
				}
				reloadtime = 0;
				attributes->QueryFloatAttribute("reloadtime", &reloadtime);
 				if (attributes->Attribute("sound") == 0)
 				{
 					std::cerr << "No weapon sound available." << std::endl;
 					return false;
 				}
				std::string weaponsound = "sounds/" + (std::string)attributes->Attribute("sound");
				if (sound->load(weaponsound) == false)
				{
					std::cerr << "Could not load weaponsound correctly." << std::endl;
					return false;
				}
				break;
			}
			attributenode = node->IterateChildren("attributes", attributenode);
		}
		// Get projectile info
		projtype = -1;
		TiXmlNode *projectilenode = root->FirstChild("projectile");
		while (projectilenode)
		{
			TiXmlElement *projectile = projectilenode->ToElement();
			if (projectile)
			{
				const char *type = projectile->Attribute("type");
				if (!type || !strcmp(type, "bullet"))
				{
					projtype = 0;
				}
				else if (!strcmp(type, "ray"))
				{
					projtype = 0;
				}
				if (!projtype)
				{
					projspeed = 1;
					projectile->QueryFloatAttribute("speed", &projspeed);
				}
				projsize = 1;
				projectile->QueryFloatAttribute("size", &projsize);
				// Texture
				if (projectile->Attribute("image"))
				{
					bullettexture = new Texture();
					std::string path = std::string("sprites/") + projectile->Attribute("image");
					if (!bullettexture->load(path))
					{
						bullettexture = 0;
					}
				}
				// Hit damage
				hitdamage = 0;
				TiXmlNode *hitnode = projectile->FirstChild("hit");
				while (hitnode)
				{
					TiXmlElement *hit = hitnode->ToElement();
					if (hit)
					{
						if (hit->Attribute("damage", &hitdamage) == 0)
						{
							std::cerr << "No hit damage available." << std::endl;
							return false;
						}
						break;
					}
					hitnode = projectile->IterateChildren("hit", hitnode);
				}
				// Explosion info
				explosion = false;
				TiXmlNode *explosionnode = projectile->FirstChild("explosion");
				while (explosionnode)
				{
					TiXmlElement *explosion = explosionnode->ToElement();
					if (explosion)
					{
						if (explosion->Attribute("damage", &explosiondamage) == 0)
						{
							std::cerr << "No explosion damage available." << std::endl;
							return false;
						}
						explosionradius = 1;
						explosion->QueryFloatAttribute("radius", &explosionradius);
						break;
					}
					explosionnode = projectile->IterateChildren("explosion", explosionnode);
				}
				break;
			}
			projectilenode = node->IterateChildren("projectile", projectilenode);
		}
		if (rate == -1 || projtype == -1)
		{
			std::cerr << "No attributes or projectile info available." << std::endl;
			return false;
		}
		// Insert into weapon list
		this->name = name;
		weapons.insert(std::pair<std::string, Weapon*>(name, this));
		return true;
	}

	void Weapon::setID(int id)
	{
		this->id = id;
	}
	int Weapon::getID()
	{
		return id;
	}

	void Weapon::playSound()
	{
		if (sound->isPlaying() == false)
			sound->play(1);
	}

	std::map<std::string, Weapon*> Weapon::weapons;
}

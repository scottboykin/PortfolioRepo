#pragma once

#include <string>
#include "XMLReader.h"
#include "vec2.h"

namespace SBGame
{
    class Unit;

    class UnitBlueprint
    {

    public:

        enum DamageTypes
        {
            MELEE,
            RANGE,
            EXPLOSION
        };

        enum ArmorTypes
        {
            LIGHT,
            MEDIUM,
            HEAVY,
            STRUCTURE
        };

        UnitBlueprint( SBUtil::XMLNode& node );

        void parseChild( TiXmlElement& element );

        std::string& getName(){ return m_name; }
        std::string& getAbilityName( int abilityNum ){ return m_abilitiesNames[abilityNum]; }

        int getGoldCost(){ return m_goldCost; }
        int getStoneCost(){ return m_stoneCost; }

        float getTimeToBuild(){ return m_timeToBuild; }
        float getMaxHealth(){ return m_maxHealth; }
        float getDamage(){ return m_damage; }
        float getArmor(){ return m_armor; }
        float getMoveSpeed(){ return m_moveSpeed; }

        DamageTypes getDamageType(){ return m_damageType; }
        ArmorTypes getArmorType(){ return m_armorType; }

        Unit* createUnitFromBlueprint( char teamNum, SBMath::vec2 position = SBMath::vec2() );

    private:

        std::string m_name;
        std::string m_abilitiesNames[4];

        int m_goldCost;
        int m_stoneCost;

        float m_timeToBuild;
        float m_maxHealth;
        float m_damage;
        float m_armor;
        float m_moveSpeed;

        DamageTypes m_damageType;

        ArmorTypes m_armorType;



    };
}
#pragma once

#include <string>
#include "vec2.h"
#include "UnitBlueprint.h"

namespace SBGame
{
    class Unit
    {

    public:

        Unit( UnitBlueprint& blueprint, char teamNum, SBMath::vec2 position = SBMath::vec2() );

        enum AIState
        {
            AI_IDLE,
            AI_MOVE,
            AI_HARVEST
        };

        std::string& getName(){ return *m_name; }
        std::string& getAbilityName( int abilityNum ){ return (*m_abilitiesNames)[abilityNum]; }

        int getGoldCost(){ return m_goldCost; }
        int getStoneCost(){ return m_stoneCost; }

        float getTimeToBuild(){ return m_timeToBuild; }
        float getMaxHealth(){ return m_maxHealth; }
        float getCurrentHealth(){ return m_currentHealth; }
        float getDamage(){ return m_damage; }
        float getArmor(){ return m_armor; }
        float getMoveSpeed(){ return m_moveSpeed; }

        const SBMath::vec2& getPosition(){ return m_position; }

        UnitBlueprint::DamageTypes getDamageType(){ return m_damageType; }
        UnitBlueprint::ArmorTypes getArmorType(){ return m_armorType; }

        void update( float dt );
        void render();

        void move( SBMath::vec2 destination );

    private:

        char m_teamNum;

        AIState m_state;

        std::string* m_name;
        std::string* m_abilitiesNames[4];

        int m_goldCost;
        int m_stoneCost;

        SBMath::vec2 m_position;
        SBMath::vec2 m_destination;

        float m_timeToBuild;
        float m_currentHealth;
        float m_maxHealth;
        float m_damage;
        float m_armor;
        float m_moveSpeed;

        UnitBlueprint::DamageTypes m_damageType;

        UnitBlueprint::ArmorTypes m_armorType;



    };
}
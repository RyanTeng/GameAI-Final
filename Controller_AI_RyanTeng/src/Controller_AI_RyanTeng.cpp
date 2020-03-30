// MIT License
// 
// Copyright(c) 2020 Arthur Bacon and Kevin Dill
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Controller_AI_RyanTeng.h"

#include "Constants.h"
#include "EntityStats.h"
#include "iPlayer.h"
#include "Vec2.h"
#include <time.h>

// Placements that the AI can choose from
static const Vec2 ksAggressiveWest(LEFT_BRIDGE_CENTER_X, RIVER_TOP_Y - 0.5f);
static const Vec2 ksDefensiveWest(LEFT_BRIDGE_CENTER_X, 0.f);

static const Vec2 ksAggressiveEast(RIGHT_BRIDGE_CENTER_X, RIVER_TOP_Y - 0.5f);
static const Vec2 ksDefensiveEast(RIGHT_BRIDGE_CENTER_X, 0.f);

// center positions can be a mix-up.
static const Vec2 ksAggressiveCenter(GAME_GRID_WIDTH / 2.0f, RIVER_TOP_Y - 0.5f);
static const Vec2 ksDefensiveCenter(GAME_GRID_WIDTH / 2.0f, 0.f);

Controller_AI_RyanTeng::Controller_AI_RyanTeng() 
{
    /* initialize random seed: */
    srand(time(NULL));

    this->diff = 0.75;
    switch (rand() % 3) {
    case 0: this->personality = random; break;
    case 1: this->personality = aggressive; break;
    case 2: this->personality = defensive; break;
    default: this->personality = random; break;
    }
}

Controller_AI_RyanTeng::Controller_AI_RyanTeng(Difficulty difficulty, Personality personality) {
    {
        switch (difficulty) {
            case very_easy: this->diff = 0.5f; break;
            case easy: this->diff = 0.66f; break;
            case medium: this->diff = 0.75f; break;
            case hard: this->diff = 0.9f; break;
            case extreme: this->diff = 1.0f; break;
            default: this->diff = 0.75; break;
        }
        this->personality = personality;
    }
}

void Controller_AI_RyanTeng::tick(float deltaTSec)
{
    assert(m_pPlayer);

    // is the player the North player?
    bool isNorth = m_pPlayer->isNorth();

    // calculate the amount of pressure for both lanes
    this->calculatePressure();

    // roll to see if we check west or east lane first.
    bool west = rand() % 2;

    // check to see if the mob will be placed closer to the center of the board or towards a lane. This is for mix-ups. Lower chance to happen
    bool place_center = rand() % 10 < 1;

    // get the current elixir, to be used in calculations
    float curr_elixir = m_pPlayer->getElixir();

    // grab a random mob. Don't check to see if the mob is able to be placed. If the mob can't be placed, save elixer for next tick.
    iEntityStats::MobType chosen_mob = m_pPlayer->GetAvailableMobTypes()[rand() % m_pPlayer->GetAvailableMobTypes().size()];

    // roll to see if the unit gets placed aggressively or defensively. Only used by the Random personality
    bool rand_aggressive = rand() % 2;

    switch (this->personality) {
    case random:
        if (place_center) {
            if (rand_aggressive) {
                m_pPlayer->placeMob(chosen_mob, ksAggressiveCenter.Player2Game(isNorth));
            }
            else {
                m_pPlayer->placeMob(chosen_mob, ksDefensiveCenter.Player2Game(isNorth));
            }
        }
        if (west) {
            if (rand_aggressive) {
                m_pPlayer->placeMob(chosen_mob, ksAggressiveWest.Player2Game(isNorth));
            }
            else {
                m_pPlayer->placeMob(chosen_mob, ksDefensiveWest.Player2Game(isNorth));
            }
        }
        else 
        {
            if (rand_aggressive) {
                m_pPlayer->placeMob(chosen_mob, ksAggressiveEast.Player2Game(isNorth));
            }
            else {
                m_pPlayer->placeMob(chosen_mob, ksDefensiveEast.Player2Game(isNorth));
            }
        }
        break;
    case defensive:
        // get the Mob with the highest cost available to place this turn. NO IDEA HOW TO DO THIS. Chooses randomly atm.
        chosen_mob = m_pPlayer->GetAvailableMobTypes()[rand() % m_pPlayer->GetAvailableMobTypes().size()];
        for (iEntityStats::MobType mob : m_pPlayer->GetAvailableMobTypes()) {
            // implement some code that finds the highest cost unit to place 
        }
        // place units only if the pressure difference in lanes is too great, or if our elixir is maxxed out.
        if (this->diff_pressure_west > 0 || (west && curr_elixir >= MAX_ELIXIR)) {
            m_pPlayer->placeMob(chosen_mob, ksDefensiveWest.Player2Game(isNorth));
        }
        else {
            if (this->diff_pressure_east > 0 || curr_elixir >= MAX_ELIXIR) {
                m_pPlayer->placeMob(chosen_mob, ksDefensiveEast.Player2Game(isNorth));
            }
        }
        break;
    case aggressive:
        // get the Mob with the lowest cost available to place this turn. NO IDEA HOW TO DO THIS. Chooses randomly atm.
        chosen_mob = m_pPlayer->GetAvailableMobTypes()[rand() % m_pPlayer->GetAvailableMobTypes().size()];
        for (iEntityStats::MobType mob : m_pPlayer->GetAvailableMobTypes()) {
            // implement some code that finds the lowest cost unit to place 
        }
        // Places units only in the lane that is winning
        bool west_winning;
        this->diff_pressure_west <= this->diff_pressure_east ? west_winning = true : west_winning = false;
        if (west_winning) {
            m_pPlayer->placeMob(chosen_mob, ksAggressiveWest.Player2Game(isNorth));
        }
        else {
            m_pPlayer->placeMob(chosen_mob, ksAggressiveEast.Player2Game(isNorth));
        }
    }
}

void Controller_AI_RyanTeng::calculatePressure() {
    float curr_west_lane_pressure = 0.f;
    float curr_east_lane_pressure = 0.f;

    // loop through all the opponents mobs and find out how many stats exist within each lane.
    for (unsigned int i = 0; i < m_pPlayer->getNumOpponentMobs(); i++) {
        iPlayer::EntityData curr_mob = m_pPlayer->getOpponentMob(i);

        // is the mob in the east or west lane?
        bool west_lane = curr_mob.m_Position.x < GAME_GRID_WIDTH / 2.0f;

        // get all the relevant stats of the current mob
        int mob_damage = curr_mob.m_Stats.getDamage();
        float mob_attack_speed = curr_mob.m_Stats.getAttackTime();
        int mob_health = curr_mob.m_Health;
        float mob_speed = curr_mob.m_Stats.getSpeed();

        // My current algorithm to calculate pressure, modified by the difficulty setting.
        float lane_pressure = (float)((mob_damage / mob_attack_speed) + mob_health + (mob_speed * 0.5f)) * this->diff;

        // add the mob's pressure to the lane that it is pressuring
        west_lane ? curr_west_lane_pressure += lane_pressure : curr_east_lane_pressure += lane_pressure;
    }


    // update the AI's curernt lane pressure values
    this->west_lane_pressure = curr_west_lane_pressure;
    this->east_lane_pressure = curr_east_lane_pressure;
    this->diff_pressure_west = curr_west_lane_pressure - this->west_lane_pressure;
    this->diff_pressure_east = curr_east_lane_pressure - this->east_lane_pressure;
}




/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_ENEMY_H
#define HHUOS_ENEMY_H

#include "lib/util/game/3d/Model.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/math/Random.h"

class Player;

class Enemy : public Util::Game::D3::Model {

public:

    enum Type {
        STATIONARY = 1,
        ORBIT_PLAYER_CLOCKWISE = 2,
        ORBIT_PLAYER_COUNTER_CLOCKWISE = 3,
        FLY_TOWARDS_PLAYER = 4,
        KEEP_DISTANCE = 5
    };

    /**
     * Constructor.
     */
    Enemy(Player &player, Util::ArrayList<Enemy*> &enemies, const Util::Math::Vector3D &position, const Util::Math::Vector3D &rotation, const Util::Math::Vector3D &scale, Type type);

    /**
     * Copy Constructor.
     */
    Enemy(const Enemy &other) = delete;

    /**
     * Assignment operator.
     */
    Enemy &operator=(const Enemy &other) = delete;

    /**
     * Destructor.
     */
    ~Enemy() override = default;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    [[nodiscard]] int16_t getHealth() const;

    void takeDamage(uint8_t damage);

    static const constexpr uint32_t TAG = 3;

private:

    Player &player;
    Util::ArrayList<Enemy*> &enemies;
    Util::Math::Random random;

    Type type;
    int16_t health = 50;
    double invulnerabilityTimer = 0;
    double missileTimer = 0;

    static const Util::Math::Vector3D MAX_ROTATION_DELTA;
};

#endif
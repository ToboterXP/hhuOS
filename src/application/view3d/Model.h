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

#ifndef HHUOS_VIEW3D_MODEL_H
#define HHUOS_VIEW3D_MODEL_H

#include "lib/util/game/3d/Model.h"

class Model : public Util::Game::D3::Model {

public:
    /**
     * Constructor.
     */
    explicit Model(const Util::String &modelPath);

    /**
     * Copy Constructor.
     */
    Model(const Model &other) = delete;

    /**
     * Assignment operator.
     */
    Model &operator=(const Model &other) = delete;

    /**
     * Destructor.
     */
    ~Model() override = default;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;
};

#endif
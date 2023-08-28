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

#include "Model.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/math/Math.h"

namespace Util::Game::D3 {

Model::Model(uint32_t tag, const String &modelPath, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale) : Entity(tag, position, rotation, scale), modelPath(modelPath) {}

Model::Model(uint32_t tag, const String &modelPath, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale, const Graphic::Color &color) : Entity(tag, position, rotation, scale), modelPath(modelPath), color(color) {}

void Model::initialize() {
    objectFile = ObjectFile::open(modelPath);
    calculateTransformedVertices();
}

void Model::calculateTransformedVertices() {
    const auto &vertices = objectFile->getVertices();
    for (uint32_t i = 0; i < vertices.length(); i++) {
        auto vertex = vertices[i];

        vertex = vertex.scale(getScale());
        vertex = vertex.rotate(getRotation());
        vertex = vertex + getPosition();

        transformedBuffer[i] = vertex;
    }
}

void Model::draw(Graphics &graphics) {
    graphics.setColor(color);
    graphics.drawModel(transformedBuffer, objectFile->getEdges());
}

void Model::onTransformChange() {
    calculateTransformedVertices();
}

}
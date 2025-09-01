/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2024 Jon Evans <jon@craftyjon.com>
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KICAD_SERIALIZABLE_H
#define KICAD_SERIALIZABLE_H

#include <wx/debug.h>
#include <kicommon.h>

// DISABLED FOR MINIMAL BUILD - Protobuf API removed
// Forward declaration for protobuf types commented out
namespace google {
    namespace protobuf {
        class Any;
    }
}

/**
 * Interface for objects that can be serialized to Protobuf messages
 * NOTE: For minimal build, serialization methods are disabled
 */
class KICOMMON_API SERIALIZABLE
{
public:
    virtual ~SERIALIZABLE() = default;

    // DISABLED FOR MINIMAL BUILD - API functionality removed
    // These methods are kept as empty virtual functions to maintain inheritance
    virtual void Serialize( google::protobuf::Any &aContainer ) const;
    virtual bool Deserialize( const google::protobuf::Any &aContainer );
};

#endif //KICAD_SERIALIZABLE_H

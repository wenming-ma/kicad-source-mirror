/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
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

#ifndef QT_INFOBAR_H_
#define QT_INFOBAR_H_

// This file provides Qt compatibility for infobar functionality
// It includes the Qt-converted wx_infobar.h and provides the expected type aliases

#include "wx_infobar.h"

// Type alias for Qt compatibility - maps QT_INFOBAR to the Qt-converted WX_INFOBAR class
typedef WX_INFOBAR QT_INFOBAR;

#endif // QT_INFOBAR_H_
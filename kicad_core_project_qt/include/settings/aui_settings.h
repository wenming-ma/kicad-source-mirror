// QT_TRANSFORMATION_COMPLETED

#ifndef _AUI_SETTINGS_H_
#define _AUI_SETTINGS_H_

#include <settings/json_settings.h>

class QDockWidget;
class QPoint;
class QRect;
class QSize;

KICOMMON_API void to_json( nlohmann::json& aJson, const QPoint& aPoint );
KICOMMON_API void from_json( const nlohmann::json& aJson, QPoint& aPoint );
KICOMMON_API bool operator<( const QPoint& aLhs, const QPoint& aRhs );

KICOMMON_API void to_json( nlohmann::json& aJson, const QSize& aPoint );
KICOMMON_API void from_json( const nlohmann::json& aJson, QSize& aPoint );
KICOMMON_API bool operator<( const QSize& aLhs, const QSize& aRhs );

KICOMMON_API void to_json( nlohmann::json& aJson, const QRect& aRect );
KICOMMON_API void from_json( const nlohmann::json& aJson, QRect& aRect );
KICOMMON_API bool operator<( const QRect& aLhs, const QRect& aRhs );

KICOMMON_API void to_json( nlohmann::json& aJson, const QDockWidget& aPaneInfo );
KICOMMON_API void from_json( const nlohmann::json& aJson, QDockWidget& aPaneInfo );
KICOMMON_API bool operator<( const QDockWidget& aLhs, const QDockWidget& aRhs );
KICOMMON_API bool operator==( const QDockWidget& aLhs, const QDockWidget& aRhs );

#endif // _AUI_SETTINGS_H_

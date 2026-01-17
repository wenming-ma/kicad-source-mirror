
#ifndef KICAD_API_UTILS_H
#define KICAD_API_UTILS_H

#include <optional>
// #include <google/protobuf/any.pb.h>

#include <core/typeinfo.h>
#include <lib_id.h>
// #include <api/common/types/base_types.pb.h>
#include <layer_ids.h>
#include <geometry/shape_line_chain.h>
#include <math/vector2d.h>
#include <math/vector3.h>
#include <gal/color4d.h>

class SHAPE_LINE_CHAIN;
class KIID_PATH;

extern const KICOMMON_API QChar* const traceApi;

/*
namespace kiapi::common
{

KICOMMON_API std::optional<KICAD_T> TypeNameFromAny( const google::protobuf::Any& aMessage );

KICOMMON_API LIB_ID LibIdFromProto( const types::LibraryIdentifier& aId );

KICOMMON_API types::LibraryIdentifier LibIdToProto( const LIB_ID& aId );

KICOMMON_API void PackVector2( types::Vector2& aOutput, const VECTOR2I& aInput );

KICOMMON_API VECTOR2I UnpackVector2( const types::Vector2& aInput );

KICOMMON_API void PackVector3D( types::Vector3D& aOutput, const VECTOR3D& aInput );

KICOMMON_API VECTOR3D UnpackVector3D( const types::Vector3D& aInput );

KICOMMON_API void PackBox2( types::Box2& aOutput, const BOX2I& aInput );

KICOMMON_API BOX2I UnpackBox2( const types::Box2& aInput );

KICOMMON_API void PackPolyLine( types::PolyLine& aOutput, const SHAPE_LINE_CHAIN& aSlc );

KICOMMON_API SHAPE_LINE_CHAIN UnpackPolyLine( const types::PolyLine& aInput );

KICOMMON_API void PackPolySet( types::PolySet& aOutput, const SHAPE_POLY_SET& aInput );

KICOMMON_API SHAPE_POLY_SET UnpackPolySet( const types::PolySet& aInput );

KICOMMON_API void PackColor( types::Color& aOutput, const KIGFX::COLOR4D& aInput );

KICOMMON_API KIGFX::COLOR4D UnpackColor( const types::Color& aInput );

KICOMMON_API void PackSheetPath( types::SheetPath& aOutput, const KIID_PATH& aInput );

} // namespace kiapi::common
*/

#endif //KICAD_API_UTILS_H

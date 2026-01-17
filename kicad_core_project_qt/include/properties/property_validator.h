// QT_TRANSFORMATION_COMPLETED

#ifndef KICAD_PROPERTY_VALIDATOR_H
#define KICAD_PROPERTY_VALIDATOR_H

#include <functional>
#include <optional>

#include <QVariant>
#include <QString>

class EDA_ITEM;
class UNITS_PROVIDER;

class VALIDATION_ERROR
{
public:
    virtual ~VALIDATION_ERROR() = default;

    virtual QString Format( UNITS_PROVIDER* aUnits ) const = 0;
};

using VALIDATOR_RESULT = std::optional<std::unique_ptr<VALIDATION_ERROR>>;

using PROPERTY_VALIDATOR_FN = std::function<VALIDATOR_RESULT( const QVariant&&, EDA_ITEM* aItem )>;

#endif //KICAD_PROPERTY_VALIDATOR_H

// QT_TRANSFORMATION_COMPLETED

#ifndef STD_OPTIONAL_VARIANT_H
#define STD_OPTIONAL_VARIANT_H

#include <optional>
#include <QVariant>

// Wrappers to allow use of std::optional<int> and std::optional<double> with the QVariant system.

class STD_OPTIONAL_INT_VARIANT_DATA
{
public:
    STD_OPTIONAL_INT_VARIANT_DATA();

    STD_OPTIONAL_INT_VARIANT_DATA( std::optional<int> aValue );

    bool Eq( const STD_OPTIONAL_INT_VARIANT_DATA& aOther ) const;

    QString GetType() const { return QStringLiteral( "std::optional<int>" ); }

    bool GetAsVariant( QVariant* aVariant ) const
    {
        if( m_value.has_value() )
            *aVariant = QVariant( m_value.value() );
        else
            *aVariant = QVariant();
        return true;
    }

    std::optional<int> Value() const
    {
        return m_value;
    }

    static STD_OPTIONAL_INT_VARIANT_DATA* VariantDataFactory( const QVariant& aVariant )
    {
        if( aVariant.isValid() && aVariant.canConvert<int>() )
            return new STD_OPTIONAL_INT_VARIANT_DATA( std::optional<int>( aVariant.toInt() ) );
        else
            return new STD_OPTIONAL_INT_VARIANT_DATA( std::nullopt );
    }

protected:
    std::optional<int> m_value;
};


class STD_OPTIONAL_DOUBLE_VARIANT_DATA
{
public:
    STD_OPTIONAL_DOUBLE_VARIANT_DATA();

    STD_OPTIONAL_DOUBLE_VARIANT_DATA( std::optional<double> aValue );

    bool Eq( const STD_OPTIONAL_DOUBLE_VARIANT_DATA& aOther ) const;

    QString GetType() const { return QStringLiteral( "std::optional<double>" ); }

    bool GetAsVariant( QVariant* aVariant ) const
    {
        if( m_value.has_value() )
            *aVariant = QVariant( m_value.value() );
        else
            *aVariant = QVariant();
        return true;
    }

    std::optional<double> Value() const
    {
        return m_value;
    }

    static STD_OPTIONAL_DOUBLE_VARIANT_DATA* VariantDataFactory( const QVariant& aVariant )
    {
        if( aVariant.isValid() && aVariant.canConvert<double>() )
            return new STD_OPTIONAL_DOUBLE_VARIANT_DATA( std::optional<double>( aVariant.toDouble() ) );
        else
            return new STD_OPTIONAL_DOUBLE_VARIANT_DATA( std::nullopt );
    }

protected:
    std::optional<double> m_value;
};

#endif //STD_OPTIONAL_VARIANT_H

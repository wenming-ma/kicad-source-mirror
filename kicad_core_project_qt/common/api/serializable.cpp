#include <api/serializable.h>


void SERIALIZABLE::Serialize( google::protobuf::Any &aContainer ) const
{
    (void)aContainer;
}


bool SERIALIZABLE::Deserialize( const google::protobuf::Any &aContainer )
{
    (void)aContainer;
    return false;
}

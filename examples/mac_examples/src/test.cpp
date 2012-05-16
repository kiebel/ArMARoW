#include <config.h>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>

typedef Mac::MessageType Message;

using boost::mpl::int_;
static const size_t payload_offset = offsetof(Message, payload);
static const size_t properties_offset = offsetof(Message, properties);
int main()
{
    BOOST_MPL_ASSERT_MSG(false, message_size,             (int_<sizeof(Message)>));
    BOOST_MPL_ASSERT_MSG(false, message_header_size,      (int_<sizeof(Message::Header)>));
    BOOST_MPL_ASSERT_MSG(false, message_propertie_size,   (int_<sizeof(Message::Properties)>));
    BOOST_MPL_ASSERT_MSG(false, message_payload_offset,   (int_<payload_offset>));
    BOOST_MPL_ASSERT_MSG(false, message_properties_offset, (int_<properties_offset>));
    return 0;
}

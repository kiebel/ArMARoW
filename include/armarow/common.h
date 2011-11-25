

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/assert.hpp>



namespace MPL{


//template <bool C, typename T>
//int assertion_failed( typename if_select_type<(C),void*,int>::type,T);



/*
//#define ARMAROW_STATIC_ASSERT_ERROR(expr, msg, types) \
//BOOST_MPL_ASSERT_MSG(expr, msg, types)
*/

#define JOIN(X,Y) DO_JOIN(X,Y)
#define DO_JOIN(X,Y) DO_JOIN2(X,Y)
#define DO_JOIN2(X,Y) X##Y

#define ARMAROW_STATIC_ASSERT_ERROR(expr, msg, types) \
BOOST_MPL_ASSERT_MSG(expr, JOIN(JOIN(JOIN(armarow_assertion_error_in_line_,__LINE__),_), msg),types);

/*
#define ARMAROW_STATIC_ASSERT_ERROR(expr,msg,types)  \
  struct JOIN(_ERROR_MSG_,msg);                      \
  enum {                                             \
    JOIN(E,__Line__)=                                \
       sizeof(assertion_failed<(expr)>(              \
		static_cast<JOIN(_ERROR_MSG_,msg)**>(0), \
		static_cast<args (*) types >(0)          \
            )                   \
       ) \
  }




//*/


}; //end namespace MPL



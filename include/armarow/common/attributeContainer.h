#pragma once

namespace armarow {
namespace common {

    /** \brief General attribute container
     *
     *  \tparam Tag the unique identifier of the specific attribute
     *  \tparam ParamType the parameter type of the attribute
     **/
    template<typename Tag, typename ValType>
    struct AttributeContainer
    {
        /** \brief forward declaration of the parameter type**/
        typedef ValType ValueType;

        /** \brief the local storage for the parameter value**/
        ValueType value;

        /** \brief parameter free constructor for getAttribute **/
        AttributeContainer(){}

        /** \brief parameterized constructor for setAttribute
         *
         *  \param param reference to the value of the parameter
         **/
        AttributeContainer(const ValueType& value) : value(value){}

        /** \brief copy constructor for different attributes with the same parameter type
         *
         *  \tparam OtherTag the unique identifier of the attribute which's parameter should be copied
         *  \param copy a reference to the source attribute
         **/
        template<typename OtherTag>
        AttributeContainer(AttributeContainer<OtherTag, ValueType>& copy) : value(copy.value){}
    };

}
}

#pragma once

namespace armarow {
namespace common {

    /** \brief General attribute container
     *
     *  \tparam Tag the unique identifier of the specific attribute
     *  \tparam ParamType the parameter type of the attribute
     **/
    template<typename Tag, typename ParamType>
    struct AttributeContainer
    {
        /** \brief the local storage for the parameter value**/
        ParamType param;

        /** \brief forward declaration of the parameter type**/
        typedef ParamType ParameterType;

        /** \brief parameter free constructor for getAttribute **/
        AttributeContainer(){}

        /** \brief parameterized constructor for setAttribute
         *
         *  \param param reference to the value of the parameter
         **/
        AttributeContainer(const ParamType& param) : param(param){}

        /** \brief copy constructor for different attributes with the same parameter type
         *
         *  \tparam OtherTag the unique identifier of the attribute which's parameter should be copied
         *  \param copy a reference to the source attribute
         **/
        template<typename OtherTag>
        AttributeContainer(AttributeContainer<OtherTag, ParamType>& copy) : param(copy.param){}
    };

}
}

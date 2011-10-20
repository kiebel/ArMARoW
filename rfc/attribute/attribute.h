namespace slyle0p1
{
	struct ttl
	{
		typedef int Parameter;
		enum Returns{
			success = 0,//standard
			badvalue = 1
			
		}
		static const bool writeable = true;
		static const bool readable  = true;
		
		Parameter min(){return 0;}
		Parameter max(){return 255;}
		
	}
	
	ttl:Returns get_attribute(ttl sel, &ttl:parameter par)
	{
		return ttl:Returns:success;
	}
	
	ttl:Returns set_attribute(ttl sel, const &ttl:parameter par)
	{
		if(par>ttl:max()||par<ttl:min())
			return ttl:Returns:badvalue;
		
		return ttl:Returns:success;
	}
}

namespace style0p2
{
	struct ttl
	{
		typedef int valuetype;
		
		valuetype value;
		
		ttl(valuetype parm)
		{
			value = parm;
		}
		
		struct Info
		{
			enum Returns
			{
				success = 0,//standard
				badvalue = 1
			};
			static const bool writeable = true;
			static const bool readable  = true;
			Parameter min(){return 0;}
			Parameter max(){return 255;}
		}
	}
	
	ttl:Info:Returns get_attribute(&ttl par)
	{
		par.value = 0;//fill parmeter
		return ttl:Info:Returns:success;
	}
	
	ttl:Info:Returns set_attribute(const &ttl par)
	{
		//next two lines are standart for comparable
		if(par.value>ttl:Info:max()||par.value<ttl:Info:min()) 
			return ttl:Info:Returns:badvalue;
		
		par.value; //work with value
		
		return ttl:Info:Returns:success;
	}
	
	struct call
	{
		typedef void(*valuetype)();
		
		valuetype value;
		
		ttl(valuetype parm)
		{
			value = parm;
		}
		
		struct Info
		{
			enum Returns
			{
				success = 0,//standard
				badvalue = 1
			};
			static const bool writeable = true;
			static const bool readable  = true;
			Parameter min(){return 0;}
			Parameter max(){return 255;}
		}
	}
}


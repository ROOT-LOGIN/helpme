#ifndef ___HELPME_COMMON_H_INCLUDE___
#define ___HELPME_COMMON_H_INCLUDE___

#ifndef DEBUG_ECHOMSG
#error Must #define DEBUG_ECHOMSG
#endif

template<typename Ty>
struct SimpleLinked
{
	Ty value;
	struct SimpleLinked<Ty> *__np;

	static inline SimpleLinked<Ty>* AllocRaw(void) {
		SimpleLinked<Ty>* ptr = (SimpleLinked<Ty>*)malloc(sizeof(SimpleLinked<Ty>));
		memset(ptr, 0, sizeof(SimpleLinked));
		return ptr;
	}

	SimpleLinked(void) {
		memset(this, 0, sizeof(SimpleLinked));
	}

	SimpleLinked(const Ty &rvalue) {
		memcpy(&value, &rvalue, sizeof(Ty));
		__np = NULL;
	}

	inline SimpleLinked* Search(const Ty &rvalue) {
		DEBUG_ECHOMSG("SimpleLinked searching for %x(%u)\n", rvalue, rvalue);
			SimpleLinked *ptr = this;
		while(ptr) {
			if(memcmp(&ptr->value, &rvalue, sizeof(Ty)) == 0) {
				DEBUG_ECHOMSG("got SimpleLinked %p\n", ptr);
					return ptr;
			}
			ptr = ptr->__np;
		}
		return NULL;
	}

	inline void Add(SimpleLinked *ptr) {
		if(!ptr) return;		
		SimpleLinked *cptr = this;
		while(cptr) {
			if(!cptr->__np) {
				DEBUG_ECHOMSG("SimpleLinked added for %p[%x(%u)]\n", cptr, ptr->value, ptr->value);
					cptr->__np = ptr; return;
			}
			cptr = cptr->__np;
		}		
	}

	inline SimpleLinked* Remove(const Ty& rvalue) {
		SimpleLinked *ptr = this;
		SimpleLinked *_ptr = NULL;
		while(ptr) {
			if(memcmp(&ptr->value, &rvalue, sizeof(Ty)) == 0) {
				if(_ptr)
					_ptr->__np = ptr->__np;
				return ptr;
			}
			_ptr = ptr;
			ptr = ptr->__np;
		}
		return NULL;
	}
};

template<typename Ty>
inline bool SimpleLinked_SetOrAdd(SimpleLinked<Ty> **ppLinked, SimpleLinked<Ty> *pValue)
{
	if(!ppLinked || ! pValue) return false;
	
	if(*ppLinked == NULL)
		*ppLinked = pValue;
	else
		(*ppLinked)->Add(pValue);
	return true;
}

#endif

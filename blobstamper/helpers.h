#ifndef HELPERS_H
#define HELPERS_H

void hexdump(void *pAddressIn, long  lSize);


/* Set of functions to_string_precise that acts as glibc to_string but
   keeps maximym precision for float types, and ads char and signed char type */

  /* For common int types just use to_string as is*/
  inline std::string
  to_string_precise(int __val)
  { return std::to_string(__val); }

  inline std::string
  to_string_precise(unsigned __val)
  { return std::to_string(__val); }

  inline std::string
  to_string_precise(long __val)
  { return std::to_string(__val); }

  inline std::string
  to_string_precise(unsigned long __val)
  { return std::to_string(__val); }

  inline std::string
  to_string_precise(long long __val)
  { return std::to_string(__val); }

  inline std::string
  to_string_precise(unsigned long long __val)
  { return std::to_string(__val); }

/* Code grabbed from /usr/include/c++/8/bits/basic_string.h and
adapted both for char and signed char, and float, double etc with
max precision
*/

  inline std::string
  to_string_precise(char __val)
  { return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf, 4 * sizeof(char),
					   "%d", __val); }

  inline std::string
  to_string_precise(signed char __val)
  { return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf, 4 * sizeof(signed char),
					   "%d", __val); }


/* 999 here is really bad idea, but for now I can't figure out better way*/
  inline std::string
  to_string_precise(float __val)
  {
     const int __n =
     __gnu_cxx::__numeric_traits<float>::__max_exponent10 + 999 + 20;
    return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf, __n,
					   "%.999g", __val);
  }

  inline std::string
  to_string_precise(double __val)
  {
     const int __n =
     __gnu_cxx::__numeric_traits<double>::__max_exponent10 + 999 + 20;
    return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf, __n,
					   "%.999g", __val);
  }

  inline std::string
  to_string_precise(long double __val)
  {
     const int __n =
     __gnu_cxx::__numeric_traits<long double>::__max_exponent10 + 999 + 20;
    return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf, __n,
					   "%.999g", __val);
  }


template<class T> class sized_ptr
{
  private:
    T* _ptr{nullptr};
    size_t _size;
  public:

    operator T*() const {return _ptr;};
    size_t size() {return _size;};
    sized_ptr(T* ptr, size_t size): _ptr{ptr}, _size{size} {};
    ~sized_ptr() {if (_ptr) free(_ptr);};
};

template<class T, class ArrayT> class VLATO_ptr
{
  private:
    T* _ptr{nullptr};
    size_t _length;
    size_t _offset;
  public:

    operator T*() const {return _ptr;};
    operator sized_ptr<T>() {sized_ptr<T> res(_ptr,size()); _ptr=NULL; return res;};

    size_t length() {return _length;};
    size_t offset() {return _offset;};
    size_t size()   {return _offset + _length * sizeof(ArrayT);};

    VLATO_ptr(size_t offsrt, size_t length);
    VLATO_ptr(T* ptr, size_t offset, size_t length): _ptr{ptr}, _offset{offset}, _length{length} {};
    ~VLATO_ptr() {if (_ptr) free(_ptr);}
};


template<class T, class ArrayT>
VLATO_ptr<T,ArrayT>::VLATO_ptr(size_t offset, size_t length)
{
  _ptr = (T*) malloc( offset + sizeof(ArrayT) * length);
  _length = length;
  _offset = offset;
}




#endif  /*HELPERS_H*/
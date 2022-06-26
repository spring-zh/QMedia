//
//  Variant.h
//  MediaCore
//
//  Created by shenpeng.zhang on 2021/10/19.
//  Copyright © 2021 QMedia. All rights reserved.
//

#ifndef _VARIANT_H_
#define _VARIANT_H_

#include <string>
#include <vector>

class Variant final {
public:
  
  enum Type {
    Invalid = 0,
    Bool = 1,
    Int = 2,
//    UInt = 3,
    LongLong = 4,
//    ULongLong = 5,
    Float = 6,
//    Char = 7,
//    List = 9,
    Double = 10,
//    Ptr = 11,
//    IntArray = 12,
//    FloatArray = 13,
    
    //extern type
    ExternType = 100,
    ExternString = 101,
    ExternIntArray = 102,
    ExternLongLongArray = 103,
    ExternFloatArray = 104,
    ExternDoubleArray = 105
  };
  
  inline Variant(const Variant& rhs) {
    this->type_ = rhs.type_;
    this->is_extern_ = rhs.is_extern_;
    this->is_null_ = rhs.is_null_;
    
    if (rhs.is_extern_) {
      this->data_.ext_ptr = rhs.data_.ext_ptr->clone();
    }else
      this->data_ = rhs.data_;
  }
  
  inline Variant(Variant&& rhs) {
    
    this->data_ = rhs.data_;
    this->type_ = rhs.type_;
    this->is_extern_ = rhs.is_extern_;
    this->is_null_ = rhs.is_null_;
    
    rhs.data_.ext_ptr = NULL;
    rhs.type_ = Invalid;
    rhs.is_null_ = 1;
    rhs.is_extern_ = 0;
  }
  
  inline Variant(): type_(Invalid) , is_extern_(0), is_null_(1) {
  }
  
  inline Variant(bool val): type_(Bool) , is_extern_(0), is_null_(0) {
    data_.b = val;
  }
  
  inline Variant(int val): type_(Int) , is_extern_(0), is_null_(0) {
    data_.i = val;
  }
  
  inline Variant(long long val): type_(LongLong) , is_extern_(0), is_null_(0) {
    data_.ll = val;
  }
  
  inline Variant(float val): type_(Float) , is_extern_(0), is_null_(0) {
    data_.f = val;
  }
  
  inline Variant(double val): type_(Double) , is_extern_(0), is_null_(0) {
    data_.d = val;
  }
  
  inline Variant(const char* const val): type_(ExternString) , is_extern_(1), is_null_(0) {
    data_.ext_ptr = new CloneableExtern<std::string>(val);;
  }
  
  /**
   *  construct with raw array
   */
  template <typename ArrayT, typename = typename std::enable_if<std::is_arithmetic<ArrayT>::value>::type>
  inline Variant(ArrayT array[], int size): is_extern_(1), is_null_(0) {
    if (std::is_same<ArrayT, int>::value) {
      type_ = ExternIntArray;
    }else if (std::is_same<ArrayT, long long>::value) {
      type_ = ExternLongLongArray;
    }else if (std::is_same<ArrayT, float>::value) {
      type_ = ExternFloatArray;
    }else if (std::is_same<ArrayT, double>::value) {
      type_ = ExternDoubleArray;
    }else
      type_ = ExternType;
    
    data_.ext_ptr = new CloneableExtern<std::vector<ArrayT>>(array, array + size);
  }

//  inline Variant(Cloneable* val, Type type = ExternType): type_(type) , is_extern_(1), is_null_(0) {
//    data_.ext_ptr = val;
//  }
  
  ~Variant() {
    ReleaseInternal();
  }

  template <typename T, typename = typename std::enable_if<std::is_copy_assignable<T>::value>::type>
  inline Variant(const T& val, Type ty = ExternType): type_(ty) , is_null_(0) {
    this->data_.ext_ptr = new CloneableExtern<T>(val);
    this->is_null_ = 0;
    this->is_extern_ = 1;
    this->type_ = ty;
  }
  
  template<Type ty = ExternType, typename T>
  inline Variant& operator=(const T &rhs) {
    ReleaseInternal();
    this->data_.ext_ptr = new CloneableExtern<T>(rhs);
    this->is_null_ = false;
    this->is_extern_ = true;
    this->type_ = ty;
    return *this;
  }
  
  inline Variant& operator=(Variant&& rhs) {
    ReleaseInternal();
    this->data_ = rhs.data_;
    this->type_ = rhs.type_;
    this->is_extern_ = rhs.is_extern_;
    this->is_null_ = rhs.is_null_;
    
    rhs.data_.ext_ptr = NULL;
    rhs.type_ = Invalid;
    rhs.is_null_ = 1;
    rhs.is_extern_ = 0;
    return *this;
  }
    
  inline Variant& operator=(const Variant& rhs) {
    ReleaseInternal();
    this->data_ = rhs.data_;
    this->type_ = rhs.type_;
    this->is_extern_ = rhs.is_extern_;
    this->is_null_ = rhs.is_null_;
    if (this->is_extern_) {
        this->data_.ext_ptr = rhs.data_.ext_ptr->clone();
    }
    return *this;
  }
  
  template<>
  inline Variant& operator=(const bool &rhs) {
    ReleaseInternal();
    this->data_.b = rhs;
    this->is_null_ = 0;
    this->is_extern_ = 0;
    this->type_ = Bool;
    return *this;
  }
  
  template<>
  inline Variant& operator=(const int &rhs) {
    ReleaseInternal();
    this->data_.i = rhs;
    this->is_null_ = 0;
    this->is_extern_ = 0;
    this->type_ = Int;
    return *this;
  }
  
  template<>
  inline Variant& operator=(const float &rhs) {
    ReleaseInternal();
    this->data_.f = rhs;
    this->is_null_ = 0;
    this->is_extern_ = 0;
    this->type_ = Float;
    return *this;
  }
  
  template<>
  inline Variant& operator=(const long long &rhs) {
    ReleaseInternal();
    this->data_.ll = rhs;
    this->is_null_ = 0;
    this->is_extern_ = 0;
    this->type_ = LongLong;
    return *this;
  }
  
  template<>
  inline Variant& operator=(const double &rhs) {
    ReleaseInternal();
    this->data_.d = rhs;
    this->is_null_ = 0;
    this->is_extern_ = 0;
    this->type_ = Double;
    return *this;
  }
  

  inline Variant& operator=(const char* const rhs) {
    ReleaseInternal();
    data_.ext_ptr = new CloneableExtern<std::string>(rhs);
    this->is_null_ = 0;
    this->is_extern_ = 1;
    this->type_ = ExternString;
    return *this;
  }
  
//  void setType(Type type) { type_ = type; }
  
  // value get
  Type getType() const { return (Type)type_; }
  
  template <typename T>
  inline const T& getVal() const {
    assert(is_extern_);
    return dynamic_cast<CloneableExtern<T>*>(this->data_.ext_ptr)->getValue();
  }
  
  template <typename T>
  inline operator T() const {
    static_assert(!std::is_same<char*, T>::value, "please use 'const char*' !! ");
    return getVal<T>();
  }
  
//  template <>
  inline operator bool() const {
    assert(type_ == Bool);
    return data_.b;
  }
  
//  template <>
  inline operator int() const {
    assert(type_ == Int);
    return data_.i;
  }
  
//  template <>
  inline operator float() const {
    assert(type_ == Float);
    return data_.f;
  }
  
//  template <>
  inline operator long long() const {
    assert(type_ == LongLong);
    return data_.ll;
  }
  
//  template <>
  inline operator double() const {
    assert(type_ == Double);
    return data_.d;
  }
  
  //  template <>
  inline operator const char*() const {
    assert(type_ == ExternString);
    return getVal<std::string>().c_str();
  }
  
  inline int getBoolVal() const {
    assert(type_ == Bool);
    return data_.b;
  }

  inline int getIntVal() const {
    assert(type_ == Int);
    return data_.i;
  }

  inline float getFloatVal() const {
    assert(type_ == Float);
    return data_.f;
  }

  inline float getLongLongVal() const {
    assert(type_ == LongLong);
    return data_.ll;
  }
  
  inline float getDoubleVal() const {
    assert(type_ == Double);
    return data_.d;
  }
  
private:
    
  typedef struct Cloneable {
    virtual Cloneable* clone() = 0;
    virtual ~Cloneable() {}
  }Cloneable;
  
  template <class T, typename = typename std::enable_if<std::is_copy_assignable<T>::value>::type>
  class CloneableExtern : public Cloneable {
  public:
    inline CloneableExtern() {}
    inline CloneableExtern(const CloneableExtern& rhs): value_(rhs.value_) {}
    inline CloneableExtern(const T& value): value_(value) {}
    inline CloneableExtern(T&& value): value_(std::move(std::forward<T>(value))) {}
    template <typename ...Args>
    inline CloneableExtern(Args ...args): value_(args...) {}
    
    Cloneable* clone() override {
      return new CloneableExtern<T>(*this);
    }
    
    inline const T& getValue() const { return value_; }
    
    T value_;
  };
  
  void ReleaseInternal() {
    if (is_extern_ && data_.ext_ptr) {
      delete data_.ext_ptr;
      data_.ext_ptr = NULL;
      is_extern_ = 0;
    }
    is_null_ = 1;
    type_ = Invalid;
  }

  union Data
  {
      char c;
      int i;
//      unsigned int u;
      long long ll;
      bool b;
      double d;
      float f;
      Cloneable* ext_ptr;
  } data_;

  unsigned int type_ : 30;
  unsigned int is_extern_ : 1;
  unsigned int is_null_ : 1;
};


#endif /* _VARIANT_H_ */

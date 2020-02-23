#ifndef hg5059_test_h
#define hg5059_test_h

#include "itkMacro.h"
#include "itkArray.h"
#include "itkVariableLengthVector.h"
#include "itkVector.h"
#include "itkFixedArray.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkPoint.h"
#include "itkRGBPixel.h"
#include "itkMatrix.h"
#include "itkVariableSizeMatrix.h"
#include "itkNumericTraits.h"
#include "itkSize.h"
#include <vector>


/** THIS DECLARATION OF NUMERICTRAITS IS HERE TEMPORARILY 
 * 
 * It should be moved to the Insight/Code/Common/itkNumericTraits.h file
 * when the new framework is integrated into ITK */
#ifdef _WIN64

 namespace itk { 

/** \class NumericTraits<unsigned long>
 * \brief Define traits for type unsigned long.
 * \ingroup DataRepresentation 
 */
template <>
class NumericTraits< std::vector<int>::size_type > : public vcl_numeric_limits< std::vector<int>::size_type > {
public:
  typedef std::vector<int>::size_type SelfType; 
  typedef SelfType                    ValueType;
  typedef SelfType                    PrintType;
  typedef SelfType                    AbsType;
  typedef SelfType                    AccumulateType;
  typedef long double                 RealType;
  typedef RealType                    ScalarRealType;
  typedef long double                 FloatType;

  static const SelfType ITKCommon_EXPORT Zero;
  static const SelfType ITKCommon_EXPORT One;

  static SelfType NonpositiveMin() { return min(); }
  static bool IsPositive(SelfType val) { return val != Zero; }
  static bool IsNonpositive(SelfType val) { return val == Zero; }
  static bool IsNegative(SelfType) { return false; }
  static bool IsNonnegative(SelfType) {return true; }
  static SelfType ZeroValue() { return Zero; }
};
}

#endif


namespace itk
{
namespace Statistics
{

/** \class MeasurementVectorTraits
 * \brief   
 * \ingroup Statistics 
 */


class MeasurementVectorTraits 
{
public:
 
  /** In the old framework, the FrequencyType is set to float. The problem is for 
      large histograms the total frequency can be more than 1e+7, than increasing 
      the frequency by one does not change the total frequency (because of lack of 
      precision). Using double type will also ultimately fall into the same problem.
      Hence in the new statistics framework, InstanceIdentifier/FrequencyTypes are 
      set to the the largest possible integer on the machine */  
  typedef std::vector<int>::size_type   InstanceIdentifier;

  /** Type defined for representing the frequency of measurement vectors */
  typedef InstanceIdentifier                                      AbsoluteFrequencyType;
  typedef NumericTraits< AbsoluteFrequencyType >::RealType        RelativeFrequencyType;
  typedef NumericTraits< AbsoluteFrequencyType >::AccumulateType  TotalAbsoluteFrequencyType;
  typedef NumericTraits< RelativeFrequencyType >::AccumulateType  TotalRelativeFrequencyType;

  typedef unsigned int MeasurementVectorLength;
  
  template<class TValueType, unsigned int VLength>
  static void SetLength( FixedArray< TValueType, VLength > &m, const unsigned int s )
    {
    if( s != VLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of a FixedArray of length " 
          << VLength << " to " << s );
      }
    m.Fill( NumericTraits< TValueType >::Zero );
    }
  
  template<class TValueType, unsigned int VLength>
  static void SetLength( FixedArray< TValueType, VLength > *m, const unsigned int s )
    {
    if( s != VLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of a FixedArray of length " 
          << VLength << " to " << s );
      }
    m->Fill( NumericTraits< TValueType >::Zero );
    }
  
  template< class TValueType >
  static void SetLength( Array< TValueType > & m, const unsigned int s )
    {
    m.SetSize( s );
    m.Fill( NumericTraits< TValueType >::Zero );
    }
  
  template< class TValueType >
  static void SetLength( Array< TValueType > * m, const unsigned int s )
    {
    m->SetSize( s );
    m->Fill( NumericTraits< TValueType >::Zero );
    }

  template< class TValueType >
  static void SetLength( VariableLengthVector< TValueType > & m, const unsigned int s )
    {
    m.SetSize( s );
    m.Fill( NumericTraits< TValueType >::Zero );
    }
  
  template< class TValueType >
  static void SetLength( VariableLengthVector< TValueType > * m, const unsigned int s )
    {
    m->SetSize( s );
    m->Fill( NumericTraits< TValueType >::Zero );
    }

  template< class TValueType >
  static void SetLength( std::vector< TValueType > & m, const unsigned int s )
    {
    m.resize( s );
    }
  
  template< class TValueType >
  static void SetLength( std::vector< TValueType > * m, const unsigned int s )
    {
    m->resize( s );
    }


  template< class TValueType, unsigned int VLength > 
  static MeasurementVectorLength 
               GetLength( const FixedArray< TValueType, VLength > &)
    { return VLength; }
  
  template< class TValueType, unsigned int VLength > 
  static MeasurementVectorLength 
               GetLength( const FixedArray< TValueType, VLength > *)
    { return VLength; }

  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const Array< TValueType > &m )
    {return m.GetSize(); }
  
  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const Array< TValueType > *m )
    {return m->GetSize(); }

  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const VariableLengthVector< TValueType > &m )
    {return m.GetSize(); }
  
  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const VariableLengthVector< TValueType > *m )
    {return m->GetSize(); }

  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const std::vector< TValueType > &m )
    {return m.size(); }
  
  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const std::vector< TValueType > *m )
    {return m->size(); }


  template< class TVectorType >
  static bool IsResizable( const TVectorType &  )
    {
    // Test whether the vector type is resizable or not
    //
    // If the default constructor creates a vector of
    // length zero, we assume that it is resizable,
    // otherwise that is a pretty useless measurement vector.
    TVectorType m;
    MeasurementVectorLength len = GetLength( m );
    return ( len == 0 );
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > &, 
                      const Array< TValueType2 > &b, const char *errMsg="Length Mismatch")
    {
    if( b.Size() == 0 )
      {
      return VLength;
      }
    if( b.Size() != 0 )
      {
      if (b.Size() != VLength)
        {
        itkGenericExceptionMacro( << errMsg );
        return 0;
        }
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > *, 
                      const Array< TValueType2 > *b, const char *errMsg="Length Mismatch")
    {
    if( b->Size() == 0 )
      {
      return VLength;
      }
    else if (b->Size() != VLength)
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > &, 
                      const VariableLengthVector< TValueType2 > &b, const char *errMsg="Length Mismatch")
    {
    if( b.Size() == 0 )
      {
      return VLength;
      }
    if( b.Size() != 0 )
      {
      if (b.Size() != VLength)
        {
        itkGenericExceptionMacro( << errMsg );
        return 0;
        }
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > *, 
                      const VariableLengthVector< TValueType2 > *b, const char *errMsg="Length Mismatch")
    {
    if( b->Size() == 0 )
      {
      return VLength;
      }
    else if (b->Size() != VLength)
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > &, 
                      const std::vector< TValueType2 > &b, const char *errMsg="Length Mismatch")
    {
    if( b.size() == 0 )
      {
      return VLength;
      }
    if( b.size() != 0 )
      {
      if (b.size() != VLength)
        {
        itkGenericExceptionMacro( << errMsg );
        return 0;
        }
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > *, 
                      const std::vector< TValueType2 > *b, const char *errMsg="Length Mismatch")
    {
    if( b->size() == 0 )
      {
      return VLength;
      }
    else if (b->size() != VLength)
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }


  template< class TValueType1, unsigned int VLength>
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > &, 
                const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( l == 0 )
      {
      return VLength;
      }
    else if( l != VLength )
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength>
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > *, 
               const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( l == 0 )
      {
      return VLength;
      }
    else if( l != VLength )
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }

  template< class TValueType >
  static MeasurementVectorLength Assert( const Array< TValueType > &a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a.Size() != l ) ) || ( a.Size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a.Size();
      }
    return 0;
    }
  
  template< class TValueType >
  static MeasurementVectorLength Assert( const Array< TValueType > *a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a->Size() != l )) || ( a->Size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a->Size();
      }
    return 0;
    }
   
  template< class TValueType >
  static MeasurementVectorLength Assert( const VariableLengthVector< TValueType > &a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a.Size() != l ) ) || ( a.Size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a.Size();
      }
    return 0;
    }
  
  template< class TValueType >
  static MeasurementVectorLength Assert( const VariableLengthVector< TValueType > *a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a->Size() != l ) ) || ( a->Size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a->Size();
      }
    return 0;
    }
  template< class TValueType >
  static MeasurementVectorLength Assert( const std::vector< TValueType > &a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a.size() != l ) ) || ( a.size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a.size();
      }
    return 0;
    }
  
  template< class TValueType >
  static MeasurementVectorLength Assert( const std::vector< TValueType > *a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a->size() != l ) ) || ( a->size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a->size();
      }
    return 0;
    }

  template< class TArrayType >
  static void  Assign( TArrayType & m, const TArrayType & v )
    {
    m = v;
    }
 
  template< class TValueType, unsigned int VLength >
  static void  Assign( FixedArray< TValueType, VLength > & m, const TValueType & v )
    {
    m[0] = v;
    }
 
};

/** \class MeasurementVectorTraitsTypes
 * \brief   
 * \ingroup Statistics 
 */

template < class TMeasurementVector >
class MeasurementVectorTraitsTypes
{
public:
  typedef typename TMeasurementVector::ValueType    ValueType;
};


template< class T >
class MeasurementVectorTraitsTypes< std::vector< T > >
{
public:
   typedef T ValueType;
};


/** Traits for generating the MeasurementVectorType that best matches a
 * particular pixel type. */
 
template<class TPixelType>
class MeasurementVectorPixelTraits
{
public:
  /* type of the vector that matches this pixel type */
  typedef TPixelType      MeasurementVectorType;
};


template<>
class MeasurementVectorPixelTraits<char>
{
public:
   typedef FixedArray< char, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<unsigned char>
{
public:
   typedef FixedArray< unsigned char, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<signed char>
{
public:
   typedef FixedArray< signed char, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<unsigned short>
{
public:
   typedef FixedArray< unsigned short, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<signed short>
{
public:
   typedef FixedArray< signed short, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<unsigned int>
{
public:
   typedef FixedArray< unsigned int, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<signed int>
{
public:
   typedef FixedArray< signed int, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<unsigned long>
{
public:
   typedef FixedArray< unsigned long, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<signed long>
{
public:
   typedef FixedArray< signed long, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<float>
{
public:
   typedef FixedArray< float, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<double>
{
public:
  typedef FixedArray< double, 1 >  MeasurementVectorType;
};


} // namespace Statistics
} // namespace itk

#endif  // __itkMeasurementVectorTraits_h

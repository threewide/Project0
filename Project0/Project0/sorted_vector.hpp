#if !defined(__SORTED_VECTOR_HPP__)
#define __SORTED_VECTOR_HPP__

/**	@file sorted_vector.hpp
	@author Garth Santor
	@date 2012-11-08

	sorted_vector template.
	*/

#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>

namespace gats {
	template<class InputIt1, class InputIt2>
	bool equal(InputIt1 first1, InputIt1 last1,
		InputIt2 first2)
	{
		for (; first1 != last1; ++first1, ++first2) {
			if (!(*first1 == *first2)) {
				return false;
			}
		}
		return true;
	}
};


template<typename T>
class sorted_vector
{
public:
	typedef T										value_type;
	typedef size_t									size_type;
	typedef	ptrdiff_t								difference_type;
	typedef	T&										reference;
	typedef	T const&								const_reference;
	typedef T*										pointer;
	typedef T const*								const_pointer;
	typedef pointer									iterator;
	typedef const_pointer							const_iterator;
	typedef std::reverse_iterator<iterator>			reverse_iterator;
	typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;

private:
	pointer	beg_;
	pointer end_;
	pointer cap_;

public:
	// Constructors
	~sorted_vector() { delete [] beg_; }
	sorted_vector() : beg_(nullptr), end_(nullptr), cap_(nullptr) { }
	explicit sorted_vector( size_type nElements, value_type const& value = value_type() );
	sorted_vector( sorted_vector<value_type> const& rhs );
	sorted_vector( sorted_vector<value_type> && rhs );
	template <class ForwardIter> sorted_vector(ForwardIter beg, ForwardIter end, typename std::enable_if< !std::is_integral<ForwardIter>::value>::type** = 0 );

	// Methods
	bool empty() const { return size() == 0; }
	size_type capacity() const { return cap_ - beg_; }
	size_type size() const { return end_ - beg_; }

	iterator erase( iterator location );
	iterator insert( value_type const& value );
	iterator find( value_type const& value ) const;
	reference at( size_type idx );
	
	iterator begin() { return iterator( beg_ ); }
	iterator end() { return iterator( end_ ); }
	const_iterator cbegin() const { return const_iterator( beg_ ); }
	const_iterator cend() const { return const_iterator( end_ ); }
	reverse_iterator rbegin() { return reverse_iterator( end_ ); }
	reverse_iterator rend() { return reverse_iterator( beg_ ); }
	const_reverse_iterator crbegin() { return const_reverse_iterator( end_ ); }
	const_reverse_iterator crend() { return const_reverse_iterator( beg_ ); }

	
	// operators
	reference operator [] ( size_type idx ) { return beg_[idx]; }
	const_reference operator [] ( size_type idx ) const { return beg_[idx]; }
	sorted_vector<T>& operator = ( sorted_vector<T> const& src );
	sorted_vector<T>& operator = ( sorted_vector<T> && src );
};



template <typename T>
sorted_vector<T>::sorted_vector( size_type nElements, value_type const& value ) 
	: beg_( new value_type [nElements] )
	, end_( beg_ + nElements )
	, cap_( beg_ + nElements )
{
	pointer p = beg_;
	while( p != end_ )
		*p++ = value;
}



template <typename T>
sorted_vector<T>::sorted_vector( sorted_vector<value_type> const& rhs )
	: beg_( new value_type [rhs.size()] )
	, end_( beg_ + rhs.size() )
	, cap_( end_ )
{
	std::copy( rhs.beg_, rhs.end_, beg_ );
}



template <typename T>
sorted_vector<T>::sorted_vector( sorted_vector<value_type> && rhs )
	: beg_( rhs.beg_ )
	, end_( rhs.end_ )
	, cap_( rhs.cap_ )
{
	rhs.beg_ = rhs.end_ = rhs.cap_ = nullptr;
}



template <typename T>
template <class ForwardIter> sorted_vector<T>::sorted_vector(ForwardIter beg, ForwardIter end, typename std::enable_if< !std::is_integral<ForwardIter>::value>::type** = 0 ) {
	auto nElements = std::distance( beg, end );
	beg_ = new value_type [ nElements ];
	end_ = beg_ + nElements;
	cap_ = beg_;
	while( beg != end )
		*cap_++ = *beg++;

	std::sort( beg_, end_ );
}



template <typename T>
typename sorted_vector<T>::iterator sorted_vector<T>::insert( value_type const& value ) {
	// check capacity for sufficient space
	if( end_ == cap_ ) {
		// INSUFFICIENT CAPACITY
		// create a new block
		size_type nCap = std::max( capacity() * 2, size_type(1) );
		pointer newBeg = new value_type [ nCap ];

		// copy up to insert point
		pointer source = beg_;
		pointer dest = newBeg;
		while( source != end_ && *source <= value )
			*dest++ = *source++;

		// insert new element
		pointer insertPoint = dest;
		*dest++ = value;

		// copy the rest
		while( source != end_ )
			*dest++ = *source++;

		// cleanup
		cap_ = newBeg + nCap;
		end_ = dest;
		delete [] beg_;
		beg_ = newBeg;

		return iterator( insertPoint );
	}
	
	// SUFFICIENT CAPACITY
	// shift until insertion point is found
	pointer insertPoint = end_;
	while( insertPoint != beg_ && *(insertPoint-1) > value ) {
		*insertPoint = *(insertPoint-1);
		--insertPoint;
	}
	*insertPoint = value;
	++end_;

	return iterator( insertPoint );
}



template <typename T>
typename sorted_vector<T>::iterator sorted_vector<T>::erase( iterator pos ) {
	auto nextPos = pos;

	for(;;) {
		auto currentPos = nextPos++;
		if( nextPos == end_ )
			break;

		*currentPos = std::move( *nextPos );
	}
	--end_;
	return pos;
}



template <typename T>
typename sorted_vector<T>::iterator sorted_vector<T>::find( value_type const& value ) const {
	auto front = beg_;
	auto back = end_;

	for( ;; ) {
		auto p = (back - front)/2 + front;
		if( p == back )
			return end_;
		else if( *p == value )
			return p;
		else if( *p > value )
			back = p;
		else
			front = p + 1;
	}
}



template <typename T>
typename sorted_vector<T>::reference sorted_vector<T>::at( size_type idx ) {
	if( idx >= size() )
		throw std::out_of_range( "idx > size" );

	return beg_[idx];
}



template <typename T>
sorted_vector<T>& sorted_vector<T>::operator = ( sorted_vector<T> const& src ) {
	std::unique_ptr<T> pNewBuffer( new T [ src.size() ] );
	std::copy( src.beg_, src.end_, pNewBuffer.get() );
	delete [] beg_;
	beg_ = pNewBuffer.release();
	end_ = beg_ + src.size();
	cap_ = end_;

	return *this;
}



template <typename T>
sorted_vector<T>& sorted_vector<T>::operator = ( sorted_vector<T> && src ) {
	delete [] beg_;

	beg_ = src.beg_;
	end_ = src.end_;
	cap_ = src.cap_;

	src.beg_ = nullptr;
	src.end_ = nullptr;
	src.cap_ = nullptr;

	return *this;
}



template <typename T>
bool operator == ( sorted_vector<T> const& lhs, sorted_vector<T> const& rhs ) {
	return lhs.size() == rhs.size() && gats::equal( lhs.cbegin(), lhs.cend(), rhs.cbegin() );
}



template <typename T>
bool operator != ( sorted_vector<T> const& lhs, sorted_vector<T> const& rhs ) {
	return lhs.size() != rhs.size() || !gats::equal( lhs.cbegin(), lhs.cend(), rhs.cbegin() );
}

#endif
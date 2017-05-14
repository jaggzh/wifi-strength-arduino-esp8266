// circle_buffer.h
#ifndef circle_buffer_h
#define circle_buffer_h

#include <Arduino.h>

template <class T> class CBuf {
	public:
		CBuf(int);
		~CBuf();            // destructor
		CBuf(const CBuf<T> &); // copy constructor
		void add(T);
		void init(T val);   // init all values to val
		T firstn(size_t n); // [0+n] (0 being a cbuf start)
		T lastn(size_t n);  // [0-n] (0 being the cbuf end)
		size_t len();
		void showbuf();
		void showbufraw();
		float avglastf(int n); // average last n stored data points
	private:
		size_t _next;    
		size_t _len;
		T *_buf;
};

template <typename T> CBuf<T>::CBuf(int count) {
	if (!count) {
		Serial.print("CBuf count cannot be 0");
		_buf = NULL;
	} else {
		_len = count;
		_buf = new T[count];
		_next = 0;
	}
}
template <typename T> CBuf<T>::CBuf(const CBuf<T> &obj) {
	_next = obj._next;            // copy member variables
	_len = obj._len;

	_buf = new T[obj._len];       // copy the buffer
	for (int i=0; i<_len; i++) {
		_buf[i] = obj._buf[i];
	}
}
template <typename T> CBuf<T>::~CBuf() {
	delete _buf;
}
template <typename T> void CBuf<T>::init(T d) {
	for (int i=0; i<_len; i++) _buf[i] = d;
}
template <typename T> void CBuf<T>::add(T d) {
	_buf[_next] = d;
	_next++;
	if (_next >= _len) _next = 0;
}
template <typename T> T CBuf<T>::firstn(size_t n) {
	size_t i = _next + (n % _len); // % in case they make us loop
	if (i >= _len) i -= _len;
	return _buf[i];
}
template <typename T> T CBuf<T>::lastn(size_t n) {
	int8_t i = _next - 1 - (n % _len);
	if (i < 0) i += _len;
	return _buf[i];
}
template <typename T> size_t CBuf<T>::len() {
	return _len;
}
template <typename T> void CBuf<T>::showbuf() {
	int n=_next;
	Serial.println("showbuf()");
	for (int i=0; i<_len; i++, n++) {
		if (n >= _len) n = 0; // wrap
		Serial.print(" CBuf[");
		Serial.print(n);
		Serial.print("]: ");
		Serial.print(_buf[n] );
		Serial.print("\n");
	}
}
template <typename T> void CBuf<T>::showbufraw() {
	Serial.println("showbufraw()");
	for (int i=0; i<_len; i++) {
		Serial.print(" RawBuf[");
		Serial.print(i);
		Serial.print("]: ");
		Serial.print(_buf[i] );
		Serial.print("\n");
	}
}
template <typename T> float CBuf<T>::avglastf(int n) {
	float tot = 0;
	for (int i=0; i<n; i++) {
		/* // for when I was debugging that I didn't initialize the values.
		Serial.print("Last[");
		Serial.print(i);
		Serial.print("] ");
		Serial.print(lastn(i));
		*/

		tot += lastn(i);

		/*
		Serial.print("  Tot: ");
		Serial.print(tot);
		Serial.print("\n");
		*/
	}
	/*
	Serial.print("  Avg: ");
	Serial.print(tot/n);
	Serial.print("\n");
	*/

	return tot/n;
}

#endif
// /circle_buffer.h

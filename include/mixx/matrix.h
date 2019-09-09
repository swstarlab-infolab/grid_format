#ifndef _MIXX_MATRIX_H_
#define _MIXX_MATRIX_H_
#include <mixx/dimension.h>
#include <mixx/detail/sequential_container_manipluation.h>

namespace mixx {

template <typename T>
class matrix2 {
public:
    using dim_type = dim2<mixx_size_t>;
    matrix2();
    ~matrix2();
    matrix2(matrix2 const& other);
    matrix2(matrix2&& other) noexcept;
    matrix2(dim_type const& dim);
    matrix2(mixx_size_t row, mixx_size_t col);
    matrix2& operator=(matrix2 const& rhs);
    matrix2& operator=(matrix2&& rhs) noexcept;
    void reshape(dim_type const& new_dim);
    void reshape(mixx_size_t row, mixx_size_t col);
    T& at(mixx_size_t offset);
    T& at(mixx_size_t row, mixx_size_t col);

    MIXX_FORCEINLINE mixx_size_t bufsize() const {
        return sizeof(T) * _dim.size();
    }

    MIXX_FORCEINLINE dim_type const& dim() const {
        return _dim;
    }

    MIXX_FORCEINLINE mixx_size_t cols() const {
        return _dim.x;
    }

    MIXX_FORCEINLINE mixx_size_t rows() const {
        return _dim.y;
    }

    MIXX_FORCEINLINE void reset() {
        _release_buffer();
        _dim.x = 0; _dim.y = 0;
    }

protected:

    MIXX_FORCEINLINE void _release_buffer() noexcept {
        if (_buffer != nullptr) {
            delete[] _buffer;
            _buffer = nullptr;
        }
    }

    dim2<mixx_size_t> _dim;
    T* _buffer;
};

template <typename T>
matrix2<T>::matrix2() {
    _dim.x = 0; _dim.y = 0;
    _buffer = nullptr;
}

template <typename T>
matrix2<T>::~matrix2() {
    _release_buffer();
}

template <typename T>
matrix2<T>::matrix2(matrix2 const& other) {
    _dim = other._dim;
    _buffer = new T[bufsize()];
    copy_array(_buffer, other._buffer, _dim.size());
}

template <typename T>
matrix2<T>::matrix2(matrix2&& other) noexcept {
    _dim = other._dim;
    _buffer = other._buffer;
    other._buffer = nullptr;
}

template <typename T>
matrix2<T>::matrix2(dim_type const& dim) {
    _dim = dim;
    _buffer = new T[bufsize()];
}

template <typename T>
matrix2<T>::matrix2(mixx_size_t row, mixx_size_t col):
    matrix2(dim_type{ col, row }) {
}

template <typename T>
matrix2<T>& matrix2<T>::operator=(matrix2 const& rhs) {
    _dim = rhs._dim;
    _buffer = new T[bufsize()];
    copy_array(_buffer, rhs._buffer, _dim.size());
    return *this;
}

template <typename T>
matrix2<T>& matrix2<T>::operator=(matrix2&& rhs) noexcept {
    _dim = rhs._dim;
    _buffer = rhs._buffer;
    rhs._buffer = nullptr;
    return *this;
}

template <typename T>
void matrix2<T>::reshape(dim_type const& new_dim) {
    T* new_buffer = new T[sizeof(T) * new_dim.size()];
    mixx_size_t const cols = (new_dim.x >= _dim.x) ? _dim.x : new_dim.x;
    mixx_size_t const rows = (new_dim.y >= _dim.y) ? _dim.y : new_dim.y;
    T* src = _buffer, *dst = new_buffer;
    for (mixx_size_t y = 0; y < rows; ++y) {
        move_array(dst, src, cols);
        src += _dim.x; 
        dst += new_dim.x;
    }
    _release_buffer();
    _buffer = new_buffer;
    _dim = new_dim;
}

template <typename T>
void matrix2<T>::reshape(mixx_size_t const row, mixx_size_t const col) {
    reshape(dim_type{ col, row });
}

template <typename T>
T& matrix2<T>::at(mixx_size_t offset) {
    return _buffer[offset];
}

template <typename T>
T& matrix2<T>::at(mixx_size_t const row, mixx_size_t const col) {
    return at(row * _dim.x + col);
}

} // !namespace mixx

#endif // _MIXX_MATRIX_H_

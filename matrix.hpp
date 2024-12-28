#pragma onces
#include <algorithm>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  // ================================================================= //
  // ====================== DEFAULT CONSTRUCTOR ====================== //
  // ================================================================= //
  Matrix() {
    matrix_.resize(N);
    for (size_t i = 0; i < N; ++i) {
      matrix_[i].resize(M, T());
    }
  }

  // ================================================================= //
  // =================== CONSTRUCTOR FROM ELEMENT ==================== //
  // ================================================================= //
  Matrix(const T& elem) {
    matrix_.resize(N);
    for (size_t i = 0; i < N; ++i) {
      matrix_[i].resize(M, elem);
    }
  }

  // ================================================================= //
  // =================== CONSTRUCTOR FROM VECTOR ===================== //
  // ================================================================= //
  Matrix(const std::vector<std::vector<T>>& fields) {
    matrix_.resize(N);
    for (size_t i = 0; i < N; ++i) {
      matrix_[i].resize(M);
    }

    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        matrix_[i][j] = fields[i][j];
      }
    }
  }

  // ================================================================= //
  // ========================= COPY CONSTRUCTOR ====================== //
  // ================================================================= //
  Matrix(const Matrix<N, M, T>& other) {
    matrix_ = other.matrix_;
    //  works only with the same size
  }

  // ================================================================= //
  // ========================== OPERATOR "=" ========================= //
  // ================================================================= //
  Matrix<N, M, T>& operator=(const Matrix<N, M, T>& other) {
    matrix_ = other.matrix_;
    return *this;
    //  works only with the same size
  }

  // ================================================================= //
  // ========================= OPERATOR "==" ========================= //
  // ================================================================= //
  bool operator==(const Matrix<N, M, T>& other) const {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        if (matrix_[i][j] != other.matrix_[i][j]) {
          return false;
        }
      }
    }

    return true;
  }

  // ================================================================= //
  // ========================= OPERATOR "+=" ========================= //
  // ================================================================= //
  Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        matrix_[i][j] += other.matrix_[i][j];
      }
    }

    return *this;
  }

  // ================================================================= //
  // ========================= OPERATOR "+" ========================== //
  // ================================================================= //
  Matrix<N, M, T> operator+(const Matrix<N, M, T>& other) const {
    Matrix<N, M, T> copy = *this;
    copy += other;

    return copy;
  }

  // ================================================================= //
  // ========================= OPERATOR "-=" ========================= //
  // ================================================================= //
  Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        matrix_[i][j] -= other.matrix_[i][j];
      }
    }

    return *this;
  }

  // ================================================================= //
  // ========================= OPERATOR "-" ========================== //
  // ================================================================= //
  Matrix<N, M, T> operator-(const Matrix<N, M, T>& other) const {
    Matrix<N, M, T> copy = *this;
    copy -= other;

    return copy;
  }

  // ================================================================= //
  // =================== OPERATOR "*=" FOR NUM ======================= //
  // ================================================================= //
  Matrix<N, M, T>& operator*=(T elem) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        matrix_[i][j] *= elem;
      }
    }

    return *this;
  }

  // ================================================================= //
  // ==================== OPERATOR "*" FOR NUM ======================= //
  // ================================================================= //
  Matrix<N, M, T> operator*(T elem) const {
    Matrix<N, M, T> copy = *this;
    copy *= elem;
    return copy;
  }

  // ================================================================= //
  // ========================= OPERATOR (i, j) ======================= //
  // ================================================================= //
  T& operator()(size_t row, size_t col) { return matrix_[row][col]; }

  T operator()(size_t row, size_t col) const { return matrix_[row][col]; }

  // ================================================================= //
  // ======================= TRANSPOSE MATRIX ======================== //
  // ================================================================= //
  Matrix<M, N, T> Transposed() const {
    Matrix<M, N, T> transposed_matrix;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        transposed_matrix(j, i) = matrix_[i][j];
      }
    }

    return transposed_matrix;
  }

  // ================================================================= //
  // =========================== DESTRUCTOR ========================== //
  // ================================================================= //
  ~Matrix() = default;

 private:
  std::vector<std::vector<T>> matrix_;
};

// ----------------------------------------------------------------- //
// --------------------- MATRIX SPECIALIZATION --------------------- //
// ----------------------------------------------------------------- //

template <size_t N, typename T>
class Matrix<N, N, T> {
 public:
  // ================================================================= //
  // ====================== DEFAULT CONSTRUCTOR ====================== //
  // ================================================================= //
  Matrix() {
    matrix_.resize(N);
    for (size_t i = 0; i < N; ++i) {
      matrix_[i].resize(N, T());
    }
  }

  // ================================================================= //
  // =================== CONSTRUCTOR FROM ELEMENT ==================== //
  // ================================================================= //
  Matrix(const T& elem) {
    matrix_.resize(N);
    for (size_t i = 0; i < N; ++i) {
      matrix_[i].resize(N, elem);
    }
  }

  // ================================================================= //
  // =================== CONSTRUCTOR FROM VECTOR ===================== //
  // ================================================================= //
  Matrix(const std::vector<std::vector<T>>& fields) {
    matrix_.resize(N);
    for (size_t i = 0; i < N; ++i) {
      matrix_[i].resize(N);
    }

    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] = fields[i][j];
      }
    }
  }

  // ================================================================= //
  // ========================= COPY CONSTRUCTOR ====================== //
  // ================================================================= //
  Matrix(const Matrix<N, N, T>& other) {
    matrix_ = other.matrix_;
    //  works only with the same size
  }

  // ================================================================= //
  // ========================== OPERATOR "=" ========================= //
  // ================================================================= //
  Matrix<N, N, T>& operator=(const Matrix<N, N, T>& other) {
    matrix_ = other.matrix_;
    return *this;
    //  works only with the same size
  }

  // ================================================================= //
  // ========================= OPERATOR "==" ========================= //
  // ================================================================= //
  bool operator==(const Matrix<N, N, T>& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        if (matrix_[i][j] != other.matrix_[i][j]) {
          return false;
        }
      }
    }

    return true;
  }

  // ================================================================= //
  // ========================= OPERATOR "+=" ========================= //
  // ================================================================= //
  Matrix<N, N, T>& operator+=(const Matrix<N, N, T>& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] += other.matrix_[i][j];
      }
    }

    return *this;
  }

  // ================================================================= //
  // ========================= OPERATOR "+" ========================== //
  // ================================================================= //
  Matrix<N, N, T> operator+(const Matrix<N, N, T>& other) const {
    Matrix<N, N, T> copy = *this;
    copy += other;

    return copy;
  }

  // ================================================================= //
  // ========================= OPERATOR "-=" ========================= //
  // ================================================================= //
  Matrix<N, N, T>& operator-=(const Matrix<N, N, T>& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] -= other.matrix_[i][j];
      }
    }

    return *this;
  }

  // ================================================================= //
  // ========================= OPERATOR "-" ========================== //
  // ================================================================= //
  Matrix<N, N, T> operator-(const Matrix<N, N, T>& other) const {
    Matrix<N, N, T> copy = *this;
    copy -= other;

    return copy;
  }

  // ================================================================= //
  // =================== OPERATOR "*=" FOR NUM ======================= //
  // ================================================================= //
  Matrix<N, N, T>& operator*=(T elem) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] *= elem;
      }
    }

    return *this;
  }

  // ================================================================= //
  // ==================== OPERATOR "*" FOR NUM ======================= //
  // ================================================================= //
  Matrix<N, N, T> operator*(T elem) const {
    Matrix<N, N, T> copy = *this;
    copy *= elem;
    return copy;
  }

  // ================================================================= //
  // ========================= OPERATOR (i, j) ======================= //
  // ================================================================= //
  T& operator()(size_t row, size_t col) { return matrix_[row][col]; }

  T operator()(size_t row, size_t col) const { return matrix_[row][col]; }

  // ================================================================= //
  // ======================= TRANSPOSE MATRIX ======================== //
  // ================================================================= //
  Matrix<N, N, T> Transposed() const {
    Matrix<N, N, T> transposed_matrix;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        transposed_matrix(j, i) = matrix_[i][j];
      }
    }

    return transposed_matrix;
  }

  // ================================================================= //
  // ============================= TRACE ============================= //
  // ================================================================= //
  T Trace() const {
    T result = T();
    for (size_t i = 0; i < N; ++i) {
      result += matrix_[i][i];
    }

    return result;
  }

  // ================================================================= //
  // =========================== DESTRUCTOR ========================== //
  // ================================================================= //
  ~Matrix() = default;

 private:
  std::vector<std::vector<T>> matrix_;
};

template <size_t N, size_t M, size_t K, typename T = int64_t>
Matrix<N, K, T> operator*(const Matrix<N, M, T>& matrix1,
                          const Matrix<M, K, T>& matrix2) {
  Matrix<N, K, T> result;

  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < K; ++j) {
      T tmp_field = T();
      for (size_t tt = 0; tt < M; ++tt) {
        tmp_field += matrix1(i, tt) * matrix2(tt, j);
      }
      result(i, j) = tmp_field;
    }
  }

  return result;
}

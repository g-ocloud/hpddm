/*
   This file is part of HPDDM.

   Author(s): Pierre Jolivet <jolivet@ann.jussieu.fr>
        Date: 2014-11-06

   Copyright (C) 2011-2014 Université de Grenoble

   HPDDM is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   HPDDM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with HPDDM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SUITESPARSE_
#define _SUITESPARSE_

template<class K>
class SuiteSparse;

#if !defined(COARSEOPERATOR) && !defined(DSUITESPARSE)
#define DSUITESPARSE
#endif
#if !defined(SUBDOMAIN) && !defined(SUITESPARSESUB)
#define SUITESPARSESUB
#endif

#if defined(SUITESPARSESUB) || defined(DSUITESPARSE)
#include <cholmod.h>
#include <umfpack.h>

namespace HPDDM {
template<class>
struct stsprs {
};

template<>
struct stsprs<double> {
    static inline void umfpack_defaults(double control[UMFPACK_CONTROL]) {
        umfpack_di_defaults(control);
    }
    static inline void umfpack_report_info(const double control[UMFPACK_CONTROL], const double info[UMFPACK_INFO]) {
        umfpack_di_report_info(control, info);
    }
    static inline int umfpack_numeric(const int* ia, const int* ja, const double* a, void* symbolic, void** numeric, const double control[UMFPACK_CONTROL], double info[UMFPACK_INFO]) {
        return umfpack_di_numeric(ia, ja, a, symbolic, numeric, control, info);
    }
    static inline int umfpack_symbolic(int n, int m, const int* ia, const int* ja, const double* a, void** symbolic, const double control[UMFPACK_CONTROL], double info[UMFPACK_INFO]) {
        return umfpack_di_symbolic(n, m, ia, ja, a, symbolic, control, info);
    }
    static inline void umfpack_free_symbolic(void** symbolic) {
        umfpack_di_free_symbolic(symbolic);
    }
    static inline int umfpack_wsolve(int sys, const int* ia, const int* ja, const double* a, double* X, const double* B, void* numeric, const double control[UMFPACK_CONTROL], double info[UMFPACK_INFO], int* Wi, double* W) {
        return umfpack_di_wsolve(sys, ia, ja, a, X, B, numeric, control, info, Wi, W);
    }
    static inline void umfpack_free_numeric(void** numeric) {
        umfpack_di_free_numeric(numeric);
    }
};

template<>
struct stsprs<std::complex<double>> {
    static inline void umfpack_defaults(double control[UMFPACK_CONTROL]) {
        umfpack_zi_defaults(control);
    }
    static inline void umfpack_report_info(const double control[UMFPACK_CONTROL], const double info[UMFPACK_INFO]) {
        umfpack_zi_report_info(control, info);
    }
    static inline int umfpack_numeric(const int* ia, const int* ja, const std::complex<double>* a, void* symbolic, void** numeric, const double control[UMFPACK_CONTROL], double info[UMFPACK_INFO]) {
        return umfpack_zi_numeric(ia, ja, reinterpret_cast<const double*>(a), NULL, symbolic, numeric, control, info);
    }
    static inline int umfpack_symbolic(int n, int m, const int* ia, const int* ja, const std::complex<double>* a, void** symbolic, const double control[UMFPACK_CONTROL], double info[UMFPACK_INFO]) {
        return umfpack_zi_symbolic(n, m, ia, ja, reinterpret_cast<const double*>(a), NULL, symbolic, control, info);
    }
    static inline void umfpack_free_symbolic(void** symbolic) {
        umfpack_zi_free_symbolic(symbolic);
    }
    static inline int umfpack_wsolve(int sys, const int* ia, const int* ja, const std::complex<double>* a, std::complex<double>* X, const std::complex<double>* B, void* numeric, const double control[UMFPACK_CONTROL], double info[UMFPACK_INFO], int* Wi, std::complex<double>* W) {
        return umfpack_zi_wsolve(sys, ia, ja, reinterpret_cast<const double*>(a), NULL, reinterpret_cast<double*>(X), NULL, reinterpret_cast<const double*>(B), NULL, numeric, control, info, Wi, reinterpret_cast<double*>(W));
    }
    static inline void umfpack_free_numeric(void** numeric) {
        umfpack_zi_free_numeric(numeric);
    }
};

template<class K>
class SuiteSparse;

#ifdef DSUITESPARSE
#define COARSEOPERATOR HPDDM::SuiteSparse
/* Class: SuiteSparse
 *
 *  A class inheriting from <DMatrix> to use <SuiteSparse>.
 *
 * Template Parameter:
 *    K              - Scalar type. */
template<class K>
class SuiteSparse : public DMatrix {
    private:
        /* Variable: L
         *  Factors returned by CHOLMOD. */
        cholmod_factor*         _L;
        /* Variable: c
         *  Parameters, statistics, and workspace of CHOLMOD. */
        cholmod_common*         _c;
        /* Variable: b
         *  Right-hand side matrix of CHOLMOD. */
        cholmod_dense*          _b;
        /* Variable: x
         *  Solution matrix of CHOLMOD. */
        cholmod_dense*          _x;
        /* Variable: Y
         *  Dense workspace matrix of CHOLMOD. */
        cholmod_dense*          _Y;
        /* Variable: E
         *  Dense workspace matrix of CHOLMOD. */
        cholmod_dense*          _E;
        /* Variable: numeric
         *  Opaque object for the numerical factorization of UMFPACK. */
        void*             _numeric;
        /* Variable: control
         *  Array of double parameters. */
        double*           _control;
        /* Variable: pattern
         *  Workspace integer array of UMFPACK. */
        int*              _pattern;
        /* Variable: W
         *  Workspace double array of UMFPACK. */
        K*                      _W;
        /* Variable: tmp
         *  Workspace array. */
        K*                    _tmp;
    protected:
        /* Variable: numbering
         *  0-based indexing. */
        static constexpr char _numbering = 'C';
    public:
        SuiteSparse() : _L(), _c(), _b(), _x(), _Y(), _E(), _numeric(), _control(), _pattern(), _W(), _tmp() { }
        ~SuiteSparse() {
            delete [] _tmp;
            _W = nullptr;
            if(_c) {
                cholmod_free_factor(&_L, _c);
                cholmod_free(1, sizeof(cholmod_dense), _b, _c);
                cholmod_free(1, sizeof(cholmod_dense), _x, _c);
                cholmod_free_dense(&_Y, _c);
                cholmod_free_dense(&_E, _c);
                cholmod_finish(_c);
                delete _c;
            }
            else {
                delete [] _pattern;
                delete [] _control;
                stsprs<K>::umfpack_free_numeric(&_numeric);
            }
        }
        template<char S>
        inline void numfact(unsigned int ncol, int* I, int* J, K* C) {
            if(S == 'S') {
                _c = new cholmod_common;
                cholmod_start(_c);
                _c->print = 3;
                cholmod_sparse* M = static_cast<cholmod_sparse*>(cholmod_malloc(1, sizeof(cholmod_sparse), _c));
                M->nrow = ncol;
                M->ncol = ncol;
                M->nzmax = I[ncol];
                M->sorted = 1;
                M->packed = 1;
                M->stype = -1;
                M->xtype = std::is_same<K, std::complex<double>>::value ? CHOLMOD_COMPLEX : CHOLMOD_REAL;
                M->p = I;
                M->i = J;
                M->x = C;
                M->dtype = CHOLMOD_DOUBLE;
                M->itype = CHOLMOD_INT;
                _L = cholmod_analyze(M, _c);
                cholmod_print_common(NULL, _c);
                cholmod_factorize(M, _L, _c);
                _b = static_cast<cholmod_dense*>(cholmod_malloc(1, sizeof(cholmod_dense), _c));
                _b->nrow = M->nrow;
                _b->xtype = M->xtype;
                _b->dtype = CHOLMOD_DOUBLE;
                _b->d = _b->nrow;
                _tmp = new K[_b->nrow];
                _x = static_cast<cholmod_dense*>(cholmod_malloc(1, sizeof(cholmod_dense), _c));
                _x->nrow = M->nrow;
                _x->x = NULL;
                _x->xtype = M->xtype;
                _x->dtype = CHOLMOD_DOUBLE;
                _x->d = _x->nrow;
                cholmod_free(1, sizeof(cholmod_sparse), M, _c);
            }
            else {
                _control = new double[UMFPACK_CONTROL];
                stsprs<K>::umfpack_defaults(_control);
                _control[UMFPACK_PRL] = 2;
                _control[UMFPACK_IRSTEP] = 0;
                double* info = new double[UMFPACK_INFO];
                _pattern = new int[ncol];
                _tmp = new K[6 * ncol];
                _W = _tmp + ncol;
                _numeric = NULL;

                void* symbolic;
                stsprs<K>::umfpack_symbolic(ncol, ncol, I, J, C, &symbolic, _control, info);
                stsprs<K>::umfpack_numeric(I, J, C, symbolic, &_numeric, _control, info);
                stsprs<K>::umfpack_report_info(_control, info);
                stsprs<K>::umfpack_free_symbolic(&symbolic);
                delete [] info;
            }
            delete [] I;
            delete [] J;
        }
        template<DMatrix::Distribution D>
        inline void solve(K* rhs) {
            if(_c) {
                _b->ncol = 1;
                _b->nzmax = _x->nrow;
                _b->x = rhs;
                _x->ncol = 1;
                _x->nzmax = _x->nrow;
                _x->x = _tmp;
                cholmod_solve2(CHOLMOD_A, _L, _b, NULL, &_x, NULL, &_Y, &_E, _c);
            }
            else
                stsprs<K>::umfpack_wsolve(UMFPACK_Aat, NULL, NULL, NULL, _tmp, rhs, _numeric, _control, NULL, _pattern, _W);
            std::copy(_tmp, _tmp + DMatrix::_n, rhs);
        }
        template<class Container>
        inline void initialize(Container& parm) {
            if(DMatrix::_communicator != MPI_COMM_NULL)
                MPI_Comm_rank(DMatrix::_communicator, &(DMatrix::_rank));
            if(parm[DISTRIBUTION] != DMatrix::NON_DISTRIBUTED) {
                if(DMatrix::_communicator != MPI_COMM_NULL && DMatrix::_rank == 0)
                    std::cout << "WARNING -- only non distributed solution and RHS supported by the SuiteSparse interface, forcing the distribution to NON_DISTRIBUTED" << std::endl;
                parm[DISTRIBUTION] = DMatrix::NON_DISTRIBUTED;
            }
            DMatrix::_distribution = DMatrix::NON_DISTRIBUTED;
        }
};
#endif // DSUITESPARSE

#ifdef SUITESPARSESUB
#define SUBDOMAIN HPDDM::SuiteSparseSub
template<class K>
class SuiteSparseSub {
    private:
        cholmod_factor*         _L;
        cholmod_common*         _c;
        cholmod_dense*          _b;
        mutable cholmod_dense*  _x;
        mutable cholmod_dense*  _Y;
        mutable cholmod_dense*  _E;
        void*             _numeric;
        double*           _control;
        int*              _pattern;
        K*                      _W;
        K*                    _tmp;
    public:
        SuiteSparseSub() : _L(), _c(), _b(), _x(), _Y(), _E(), _numeric(), _control(), _pattern(), _W(), _tmp() { }
        SuiteSparseSub(const SuiteSparseSub&) = delete;
        ~SuiteSparseSub() {
            delete [] _tmp;
            _W = nullptr;
            if(_c) {
                cholmod_free_factor(&_L, _c);
                cholmod_free(1, sizeof(cholmod_dense), _b, _c);
                cholmod_free(1, sizeof(cholmod_dense), _x, _c);
                cholmod_free_dense(&_Y, _c);
                cholmod_free_dense(&_E, _c);
                cholmod_finish(_c);
                delete _c;
            }
            else {
                delete [] _pattern;
                delete [] _control;
                stsprs<K>::umfpack_free_numeric(&_numeric);
            }
        }
        inline void numfact(MatrixCSR<K>* const& A, bool detection = false) {
            if(!_c && !_W) {
                if(A->_sym && !std::is_same<K, std::complex<double>>::value) {
                    _c = new cholmod_common;
                    cholmod_start(_c);
                    cholmod_sparse* M = static_cast<cholmod_sparse*>(cholmod_malloc(1, sizeof(cholmod_sparse), _c));
                    M->nrow = A->_m;
                    M->ncol = A->_n;
                    M->nzmax = A->_nnz;
                    M->sorted = 1;
                    M->packed = 1;
                    M->stype = 1;
                    M->xtype = std::is_same<K, std::complex<double>>::value ? CHOLMOD_COMPLEX : CHOLMOD_REAL;
                    M->p = A->_ia;
                    M->i = A->_ja;
                    M->x = A->_a;
                    M->dtype = CHOLMOD_DOUBLE;
                    M->itype = CHOLMOD_INT;
                    _L = cholmod_analyze(M, _c);
                    cholmod_factorize(M, _L, _c);
                    _b = static_cast<cholmod_dense*>(cholmod_malloc(1, sizeof(cholmod_dense), _c));
                    _b->nrow = M->nrow;
                    _b->xtype = M->xtype;
                    _b->dtype = CHOLMOD_DOUBLE;
                    _b->d = _b->nrow;
                    _tmp = new K[_b->nrow];
                    _x = static_cast<cholmod_dense*>(cholmod_malloc(1, sizeof(cholmod_dense), _c));
                    _x->nrow = M->nrow;
                    _x->x = NULL;
                    _x->xtype = M->xtype;
                    _x->dtype = CHOLMOD_DOUBLE;
                    _x->d = _x->nrow;
                    cholmod_free(1, sizeof(cholmod_sparse), M, _c);
                }
                else {
                    _control = new double[UMFPACK_CONTROL];
                    stsprs<K>::umfpack_defaults(_control);
                    _control[UMFPACK_PRL] = 0;
                    _control[UMFPACK_IRSTEP] = 0;
                    double* info = new double[UMFPACK_INFO];
                    _pattern = new int[A->_m];
                    _tmp = new K[6 * A->_m];
                    _W = _tmp + A->_m;
                    _numeric = NULL;

                    void* symbolic = NULL;
                    K* a;
                    int* ia;
                    int* ja;
                    if(!A->_sym) {
                        a  = A->_a;
                        ia = A->_ia;
                        ja = A->_ja;
                    }
                    else {
                        std::vector<std::vector<std::pair<unsigned int, K>>> v(A->_n);
                        unsigned int nnz = std::floor((A->_nnz + A->_n - 1) / A->_n) * 2;
                        for(unsigned int i = 0; i < A->_n; ++i)
                            v[i].reserve(nnz);
                        nnz = 0;
                        for(unsigned int i = 0; i < A->_n; ++i) {
                            for(unsigned int j = A->_ia[i]; j < A->_ia[i + 1] - 1; ++j) {
                                if(std::abs(A->_a[j]) > HPDDM_EPS) {
                                    v[i].emplace_back(A->_ja[j], A->_a[j]);
                                    v[A->_ja[j]].emplace_back(i, A->_a[j]);
                                    nnz += 2;
                                }
                            }
                            v[i].emplace_back(i, A->_a[A->_ia[i + 1] - 1]);
                            ++nnz;
                        }
                        ja = new int[A->_n + 1 + nnz];
                        ia = ja + nnz;
                        a  = new K[nnz];
                        nnz = 0;
                        unsigned int i;
#pragma omp parallel for schedule(static, HPDDM_GRANULARITY)
                        for(i = 0; i < A->_n; ++i)
                            std::sort(v[i].begin(), v[i].end(), [](const std::pair<unsigned int, K>& lhs, const std::pair<unsigned int, K>& rhs) { return lhs.first < rhs.first; });
                        ia[0] = 0;
                        for(i = 0; i < A->_n; ++i) {
                            for(const std::pair<unsigned int, K>& p : v[i]) {
                                ja[nnz]  = p.first;
                                a[nnz++] = p.second;
                            }
                            ia[i + 1] = nnz;
                        }
                    }
                    stsprs<K>::umfpack_symbolic(A->_m, A->_n, ia, ja, a, &symbolic, _control, info);
                    stsprs<K>::umfpack_numeric(ia, ja, a, symbolic, &_numeric, _control, info);
                    stsprs<K>::umfpack_report_info(_control, info);
                    stsprs<K>::umfpack_free_symbolic(&symbolic);
                    if(A->_sym) {
                        delete [] ja;
                        delete [] a;
                    }
                    delete [] info;
                }
            }
        }
        inline void solve(K* x) const {
            if(_c) {
                _b->ncol = 1;
                _b->nzmax = _x->nrow;
                _b->x = x;
                _x->ncol = 1;
                _x->nzmax = _x->nrow;
                _x->x = _tmp;
                cholmod_solve2(CHOLMOD_A, _L, _b, NULL, &_x, NULL, &_Y, &_E, _c);
                std::copy(_tmp, _tmp + _x->nrow, x);
            }
            else {
                stsprs<K>::umfpack_wsolve(UMFPACK_Aat, NULL, NULL, NULL, _tmp, x, _numeric, _control, NULL, _pattern, _W);
                std::copy(_tmp, _W, x);
            }
        }
        inline void solve(K* x, const unsigned short& n) const {
            if(_c) {
                _b->ncol = n;
                _b->nzmax = _x->nrow;
                _b->x = x;
                _x->ncol = n;
                _x->nzmax = _x->nrow;
                _x->x = new K[n * _x->nrow];
                cholmod_solve2(CHOLMOD_A, _L, _b, NULL, &_x, NULL, &_Y, &_E, _c);
                std::copy(static_cast<K*>(_x->x), static_cast<K*>(_x->x) + n * _x->nrow, x);
                delete [] static_cast<K*>(_x->x);
                _x->x = NULL;
            }
            else {
                int ld = std::distance(_tmp, _W);
                for(unsigned short i = 0; i < n; ++i) {
                    stsprs<K>::umfpack_wsolve(UMFPACK_Aat, NULL, NULL, NULL, _tmp, x + i * ld, _numeric, _control, NULL, _pattern, _W);
                    std::copy(_tmp, _W, x + i * ld);
                }
            }
        }
        inline void solve(const K* b, K* x) const {
            if(_c) {
                _b->ncol = 1;
                _b->nzmax = _x->nrow;
                _b->x = const_cast<K*>(b);
                _x->ncol = 1;
                _x->nzmax = _x->nrow;
                _x->x = x;
                cholmod_solve2(CHOLMOD_A, _L, _b, NULL, &_x, NULL, &_Y, &_E, _c);
            }
            else
                stsprs<K>::umfpack_wsolve(UMFPACK_Aat, NULL, NULL, NULL, x, b, _numeric, _control, NULL, _pattern, _W);
        }
};
#endif // SUITESPARSESUB
} // HPDDM
#endif // DSUISTESPARSE || SUITESPARSESUB
#endif // _SUITESPARSE_
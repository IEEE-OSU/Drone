// Philip Wallstedt 2007-2008
// See explanation at http://www.sci.utah.edu/~wallstedt

#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

// These functions print matrices and vectors in a nice format
void coutMatrix(int d,double*m){
   cout<<'\n';
   for(int i=0;i<d;++i){
      for(int j=0;j<d;++j)cout<<setw(14)<<m[i*d+j];
      cout<<'\n';
   }
}

void coutVector(int d,double*v){
   cout<<'\n';
   for(int j=0;j<d;++j)cout<<setw(14)<<v[j];
   cout<<'\n';
}

// The following compact LU factorization schemes are described
// in Dahlquist, Bjorck, Anderson 1974 "Numerical Methods".
//
// S and D are d by d matrices.  However, they are stored in
// memory as 1D arrays of length d*d.  Array indices are in
// the C style such that the first element of array A is A[0]
// and the last element is A[d*d-1].
//
// These routines are written with separate source S and
// destination D matrices so the source matrix can be retained
// if desired.  However, the compact schemes were designed to
// perform in-place computations to save memory.  In
// other words, S and D can be the SAME matrix.  Just
// modify the code like this example:
//
//    void Crout(int d,double*S){
//       for(int k=0;k<d;++k){
//          for(int i=k;i<d;++i){
//             double sum=0.;
//             for(int p=0;p<k;++p)sum+=S[i*d+p]*S[p*d+k];
//             S[i*d+k]=S[i*d+k]-sum; // not dividing by diagonals
//          }
//          for(int j=k+1;j<d;++j){
//             double sum=0.;
//             for(int p=0;p<k;++p)sum+=S[k*d+p]*S[p*d+j];
//             S[k*d+j]=(S[k*d+j]-sum)/S[k*d+k];
//          }
//       }
//    }



// Crout uses unit diagonals for the upper triangle
void Crout(int d,double*S,double*D){
   for(int k=0;k<d;++k){
      for(int i=k;i<d;++i){
         double sum=0.;
         for(int p=0;p<k;++p)sum+=D[i*d+p]*D[p*d+k];
         D[i*d+k]=S[i*d+k]-sum; // not dividing by diagonals
      }
      for(int j=k+1;j<d;++j){
         double sum=0.;
         for(int p=0;p<k;++p)sum+=D[k*d+p]*D[p*d+j];
         D[k*d+j]=(S[k*d+j]-sum)/D[k*d+k];
      }
   }
}
void solveCrout(int d,double*LU,double*b,double*x){
   double y[d];
   for(int i=0;i<d;++i){
      double sum=0.;
      for(int k=0;k<i;++k)sum+=LU[i*d+k]*y[k];
      y[i]=(b[i]-sum)/LU[i*d+i];
   }
   for(int i=d-1;i>=0;--i){
      double sum=0.;
      for(int k=i+1;k<d;++k)sum+=LU[i*d+k]*x[k];
      x[i]=(y[i]-sum); // not dividing by diagonals
   }
}



// Doolittle uses unit diagonals for the lower triangle
void Doolittle(int d,double*S,double*D){
   for(int k=0;k<d;++k){
      for(int j=k;j<d;++j){
         double sum=0.;
         for(int p=0;p<k;++p)sum+=D[k*d+p]*D[p*d+j];
         D[k*d+j]=(S[k*d+j]-sum); // not dividing by diagonals
      }
      for(int i=k+1;i<d;++i){
         double sum=0.;
         for(int p=0;p<k;++p)sum+=D[i*d+p]*D[p*d+k];
         D[i*d+k]=(S[i*d+k]-sum)/D[k*d+k];
      }
   }
}
void solveDoolittle(int d,double*LU,double*b,double*x){
   double y[d];
   for(int i=0;i<d;++i){
      double sum=0.;
      for(int k=0;k<i;++k)sum+=LU[i*d+k]*y[k];
      y[i]=(b[i]-sum); // not dividing by diagonals
   }
   for(int i=d-1;i>=0;--i){
      double sum=0.;
      for(int k=i+1;k<d;++k)sum+=LU[i*d+k]*x[k];
      x[i]=(y[i]-sum)/LU[i*d+i];
   }
}



// Cholesky requires the matrix to be symmetric positive-definite
void Cholesky(int d,double*S,double*D){
   for(int k=0;k<d;++k){
      double sum=0.;
      for(int p=0;p<k;++p)sum+=D[k*d+p]*D[k*d+p];
      D[k*d+k]=sqrt(S[k*d+k]-sum);
      for(int i=k+1;i<d;++i){
         double sum=0.;
         for(int p=0;p<k;++p)sum+=D[i*d+p]*D[k*d+p];
         D[i*d+k]=(S[i*d+k]-sum)/D[k*d+k];
      }
   }
}
// This version could be more efficient on some architectures
// Use solveCholesky for both Cholesky decompositions
void CholeskyRow(int d,double*S,double*D){
   for(int k=0;k<d;++k){
      for(int j=0;j<d;++j){
         double sum=0.;
         for(int p=0;p<j;++p)sum+=D[k*d+p]*D[j*d+p];
         D[k*d+j]=(S[k*d+j]-sum)/D[j*d+j];
      }
      double sum=0.;
      for(int p=0;p<k;++p)sum+=D[k*d+p]*D[k*d+p];
      D[k*d+k]=sqrt(S[k*d+k]-sum);
   }
}
void solveCholesky(int d,double*LU,double*b,double*x){
   double y[d];
   for(int i=0;i<d;++i){
      double sum=0.;
      for(int k=0;k<i;++k)sum+=LU[i*d+k]*y[k];
      y[i]=(b[i]-sum)/LU[i*d+i];
   }
   for(int i=d-1;i>=0;--i){
      double sum=0.;
      for(int k=i+1;k<d;++k)sum+=LU[k*d+i]*x[k];
      x[i]=(y[i]-sum)/LU[i*d+i];
   }
}



int main(){
   // the following checks are published at:
   // http://math.fullerton.edu/mathews/n2003/CholeskyMod.html

   // We want to solve the system Ax=b
   
   double A[25]={2.,1.,1.,3.,2.,
                 1.,2.,2.,1.,1.,
                 1.,2.,9.,1.,5.,
                 3.,1.,1.,7.,1.,
                 2.,1.,5.,1.,8.};
   
   double LU[25];
   
   double b[5]={-2.,4.,3.,-5.,1.};

   double x[5];

   Crout(5,A,LU);
   cout<<"Crout";
   coutMatrix(5,LU);
   solveCrout(5,LU,b,x);
   cout<<"solveCrout";
   coutVector(5,x);
   
   Doolittle(5,A,LU);
   cout<<"Doolittle";
   coutMatrix(5,LU);
   solveDoolittle(5,LU,b,x);
   cout<<"solveDoolittle";
   coutVector(5,x);

   Cholesky(5,A,LU);
   cout<<"Cholesky";
   coutMatrix(5,LU);
   solveCholesky(5,LU,b,x);
   cout<<"solveCholesky";
   coutVector(5,x);

}




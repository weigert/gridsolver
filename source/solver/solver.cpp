#include <valarray>
#include <vector>
#include <complex>
#include <glm/glm.hpp>
#include <fftw.h>
#include <iostream>

using namespace std::complex_literals;
const double PI = 3.141592653589793238460;

typedef std::complex<double> complex;
typedef std::valarray<complex> CArray;
typedef std::valarray<bool> BArray;

/*
================================================================================
                                Operator Stuff
================================================================================
*/

//Complex Comparison
bool operator < (const complex& lhs, const complex& rhs){ return lhs.real() < rhs.real(); }
bool operator > (const complex& lhs, const complex& rhs){ return rhs < lhs; }
bool operator <= (const complex& lhs, const complex& rhs){ return !(lhs > rhs); }
bool operator >= (const complex& lhs, const complex& rhs){ return !(lhs < rhs); }

//Boolean Arrays for Masking
BArray operator < (const CArray& lhs, const CArray &rhs){
  BArray b (lhs.size());
  for(unsigned int i = 0; i < lhs.size(); i++){
    b[i] = lhs[i] < rhs[i];
  }
  return b;
}

BArray operator > (const CArray& lhs, const CArray &rhs){
  BArray b (lhs.size());
  for(unsigned int i = 0; i < lhs.size(); i++){
    b[i] = lhs[i] > rhs[i];
  }
  return b;
}

std::valarray<bool> operator > (const CArray& lhs, const complex& rhs){
  std::valarray<bool> b (lhs.size());
  for(unsigned int i = 0; i < lhs.size(); i++){
    b[i] = lhs[i] > rhs;
  }
  return b;
}

std::valarray<bool> operator < (const CArray& lhs, const complex& rhs){
  std::valarray<bool> b (lhs.size());
  for(unsigned int i = 0; i < lhs.size(); i++){
    b[i] = lhs[i] < rhs;
  }
  return b;
}

std::valarray<bool> operator == (const CArray& lhs, const complex& rhs){
  std::valarray<bool> b (lhs.size());
  for(unsigned int i = 0; i < lhs.size(); i++){
    b[i] = lhs[i].real() == rhs.real();
  }
  return b;
}

/*
================================================================================
                                Solver Functions
================================================================================
*/

//Here are all helper functions for doing math with
namespace solve{

//Holds the Gridsize currently being worked on
glm::vec2 modes;

//Index and Vector Conversions on Grid
int ind(glm::vec2 _p);                  //Use the Modes Size
int ind(glm::vec2 _p, glm::vec2 _s);    //Use a custom Size
glm::vec2 pos(int ind);

//Construction Helpers
std::vector<CArray> emptyArray(unsigned int size);    //Fill vector of CArrays with zeros
CArray fromArray(float a[]);                          //Fill CArray from Array
CArray random(double high, double low);

//Solver Term Helpers
CArray diff(CArray field, int x, int y);              //Differential Real space, degree x and y
CArray fdiff(CArray field, int x, int y);             //Differential Fourier space, degree x and y
CArray abs(CArray field);                             //Negative values become positive
CArray scale(CArray field, double a, double b);       //Linear Scale between a, b
CArray roll(CArray field, glm::vec2 offset);          //Offset everything by a fixed amount
CArray shift(CArray field, CArray x, CArray y);       //Offset everything by a specific amount
CArray diffuse(CArray field, double mu, int cycles);  //Returns a diffusion passed CArray
CArray clamp(CArray field, double low, double high);  //Clamps the values between two values

//Complex Operations
CArray fft(CArray coef);                                            //Fourier Transform
CArray ifft(CArray coef);                                           //Inverse Fourier Transform
CArray label(CArray field, int &nareas);                            //Returns label array by value
CArray convolve(CArray field, CArray kernel, glm::vec2 ksize);      //Returns the kernel convolved
float autothresh(CArray height, float start, float fraction);       //Auto Thresholder
CArray sample(CArray field, glm::vec2 oldSize, glm::vec2 newSize);  //Return a higher resolution grid

/*
================================================================================
                    Index and Vector Conversions on Grid
================================================================================
*/

int ind(glm::vec2 _p){
  //Return the Correct Index
  _p = glm::mod(_p+modes, modes);
  if(glm::all(glm::lessThan(_p, modes)) && glm::all(glm::greaterThanEqual(_p, glm::vec2(0)))){
    return _p.x*modes.y+_p.y;
  }
  return 0;
}

int ind(glm::vec2 _p, glm::vec2 _s){
  //Return the Correct Index
  _p = glm::mod(_p+_s, _s);
  if(glm::all(glm::lessThan(_p, _s)) && glm::all(glm::greaterThanEqual(_p, glm::vec2(0)))){
    return _p.x*_s.y+_p.y;
  }
  return 0;
}


glm::vec2 pos(int ind){
  int _x = ind/modes.y;
  int _y = ind-modes.y*_x;
  //Return Binary Vectory
  return glm::vec2(_x, _y);
}

/*
================================================================================
                            Construction Helpers
================================================================================
*/

std::vector<CArray> emptyArray(unsigned int size){
  //Add blank fields to delta
  std::vector<CArray> delta;
  CArray zero(0.0, modes.x*modes.y);
  delta.assign(size, zero);
  return delta;
}

CArray fromArray(float a[]){  //Constrct field from flat array
  //Set the Data
  CArray coef(0.0, modes.x*modes.y);
  for(int i = 0; i < modes.x*modes.y; i++){
    //Generate Blank Vector
    complex _coef = a[i];
    coef[i] = _coef;
  }
  return coef;
}

CArray random(double high, double low){
  CArray coef(0.0, modes.x*modes.y);
  for(int i = 0; i < modes.x*modes.y; i++){
    //Generate Blank Vector
    coef[i] = ((double)(rand()%1000000)/1000000)*(high-low)+low;
  }
  return coef;
}

/*
================================================================================
                              Solver Term Helpers
================================================================================
*/

CArray roll(CArray field, glm::vec2 offset){
  //New Array
  CArray coef(0.0, modes.x*modes.y);
  //Loop over the Field
  for(unsigned int i = 0; i < field.size(); i++){
    int projected = (int)(ind(pos(i)+offset)+modes.x*modes.y)%(int)(modes.x*modes.y);
    coef[i] = field[projected];
  }
  return coef;
}

CArray shift(CArray field, CArray x, CArray y){
  //New Array
  CArray coef(0.0, modes.x*modes.y);
  //Loop over the Field
  for(unsigned int i = 0; i < field.size(); i++){
    glm::vec2 offset = glm::floor(glm::vec2(x[i].real(), y[i].real()));
    int projected = (int)(ind(pos(i)+offset)+modes.x*modes.y)%(int)(modes.x*modes.y);
    coef[i] = field[projected];
  }
  return coef;
}

CArray clamp(CArray field, double low, double high){
  field[field > high] = (complex)high; //Clamp
  field[field < low] = (complex)low;
  return field;
}

CArray scale(CArray field, double min, double max){
  //Return a threshold
  double fmax = field[0].real();
  double fmin = field[0].real();
  //Get Maximum
  for(unsigned int i = 1; i < field.size(); i++){
    if(field[i].real() > fmax){
      fmax = field[i].real();
    }
    if(field[i].real() < fmin){
      fmin = field[i].real();
    }
  }
  //Rescale
  for(unsigned int i = 0; i < field.size(); i++){
    field[i] = (field[i]-fmin)/(fmax-fmin)*(max-min)+min;
  }
  return field;
}

CArray abs(CArray field){
  //Absolute Value
  for(unsigned int i = 0; i < field.size(); i++){
    field[i] = (field[i] < 0.0)?-1.0*field[i]:field[i];
  }
  return field;
}


//2D N-th order Differential in Fourier Space
CArray fdiff(CArray field, int x, int y){
  //Fill the Blank Field
  for(unsigned int i = 0; i < field.size(); i++){
    //Get the coefficient index
    glm::vec2 z = solve::pos(i);
    field[i] *= pow(1.0i*(2*PI*z.x), x)*pow(1.0i*(2*PI*z.y), y);
  }
  return field;
}

//2D N-th order Differential in Fourier Space
CArray diff(CArray field, int x, int y){
  //Convert the Field
  return ifft(fdiff(fft(field), x, y));
}

CArray diffuse(CArray field, double mu, int cycles){
  //Convert to fourier space
  CArray _d = fft(field);

  for(int i = 0; i < cycles; i++){
    //Somehow Compute Diffuse
    for(unsigned int i = 0; i < field.size(); i++){
      //Get the coefficient index
      glm::vec2 z = solve::pos(i);
      _d[i] += (complex)mu*(_d[i]*pow(2.0i*(PI*z.x), 2) + _d[i]*pow(2.0i*(PI*z.y), 2));
    }
  }

  return ifft(_d);
}

/*
================================================================================
                            Complex Operations
================================================================================
*/

CArray fft(CArray coef){
  int N = modes.x*modes.y;
  fftw_complex x[N];

  //Copy from CArray
  for(int i = 0; i < N; i++){
    x[i].re = coef[i].real();
    x[i].im = coef[i].imag();
  }
  //Do the Transform
  fftwnd_plan p = fftw2d_create_plan(modes.x, modes.y, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_IN_PLACE);
  fftwnd_one(p, x, NULL);
  fftwnd_destroy_plan(p);

  //Convert Back
  for(int i = 0; i < N; i++){
    coef[i] = complex(x[i].re, x[i].im);
  }
  return coef;
}

CArray ifft(CArray coef){
  int N = modes.x*modes.y;
  fftw_complex x[N];

  //Copy from CArray
  for(int i = 0; i < N; i++){
    x[i].re = coef[i].real();
    x[i].im = coef[i].imag();
  }
  //Do the Transform
  fftwnd_plan p = fftw2d_create_plan(modes.x, modes.y, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_IN_PLACE);
  fftwnd_one(p, x, NULL);
  fftwnd_destroy_plan(p);

  //Convert Back
  for(int i = 0; i < N; i++){
    coef[i] = complex(x[i].re/N, x[i].im/N);
  }
  return coef;
}

CArray label(CArray field, int &nareas){
  //Label Array
  CArray _labels(0.0, modes.x*modes.y);

  //Centroid Vector
  std::vector<complex> centroids;

  //Loop over all Field Elements
  for(unsigned int i = 0; i < field.size(); i++){
    //Check if its inside the centroids
    bool contained = false;

    for(unsigned int j = 0; j < centroids.size(); j++){
      //Check if we're inside the centroid
      if(centroids[j] == field[i]){
        //We have found the correct centroid
        _labels[i] = (complex)j;
        contained = true;
      }
    }

    if(!contained){
      //Add the centroid
      centroids.push_back(field[i]);

      //Set our label to the current centroids size
      _labels[i] = (complex)centroids.size();
    }
  }

  nareas = centroids.size();
  return _labels;
}

float autothresh(CArray height, float start, float fraction){
  //Compute the Sealevel
  float test = 0.0;
  while((test - fraction)*(test - fraction) > 0.001){
    //Compute land above and below sealevel
    CArray above(0.0, modes.x*modes.y);
    CArray below(0.0, modes.x*modes.y);
    above[height > (double)start] = 1.0;
    below[height < (double)start] = 1.0;
    //Our Current Ratio is the amount of land above/below the sealevel
    test = above.sum().real() / (above.sum().real() + below.sum().real());

    //Proportionally Increase the sealevel
    float k = 0.0001;
    start += (test-fraction)*k;   //Positive Constant Important

    //Make sure it remains in bounds
    if(start < 0.0){
      //The sealevel is out of bounds
      std::cout<<"Threshold too low."<<std::endl;
      return 0.0;
    }
    if(start > 1.0){
      //The sealevel is out of bounds
      std::cout<<"Threshold too high."<<std::endl;
      return 1.0;
    }
  }
  return start;
}

CArray convolve(CArray field, CArray kernel, glm::vec2 ksize){
  //Construct a padded kernel array
  CArray padded(0.0, modes.x*modes.y);

  //Loop over the kernelsize
  for(int k = 0; k < ksize.x; k++){
    for(int l = 0; l < ksize.y; l++){
      //Enter the Padded Stuff
      padded[ind(glm::vec2(k, l)-glm::floor(ksize/glm::vec2(2)))] = kernel[ind(glm::vec2(k, l), ksize)];
    }
  }

  //Return Convolution
  return ifft(fft(padded)*fft(field));
}

CArray sample(CArray field, glm::vec2 oldSize, glm::vec2 newSize){
  //Construct our output array
  CArray coef(0.0, newSize.x*newSize.y);
  return coef;

  //For some reason, I can't get this to work right now.
}

//End of namespace
}

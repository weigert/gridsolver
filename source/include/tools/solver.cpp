#include <valarray>
#include <complex>

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

//Member Functions
int ind(glm::vec2 _p);
glm::vec2 pos(int ind);

//Constructors
CArray fromArray(float a[]);
void emptyArray(std::vector<CArray> &delta, unsigned int size);

//Term Helpers
CArray diff(CArray field, int x, int y);              //Differential Real space, degree x and y
CArray fdiff(CArray field, int x, int y);             //Differential Fourier space, degree x and y
CArray scale(CArray field, double a, double b);       //Linear Scale between a, b
CArray roll(CArray field, glm::vec2 offset);
CArray diffuse(CArray field, double mu, int cycles);

//Complex Operations
CArray fft(CArray coef);                               //Fourier Transform
CArray ifft(CArray coef);                              //Inverse Fourier Transform
CArray cluster(CArray field, int &nclusters);           //Clustering Operation
float autothresh(CArray height, float start, float fraction); //Auto Thresholder

//Visualization
void image(std::string name, CArray a, CArray b, CArray c);
void grayscale(CArray field, std::string name);
void colorthresh(CArray field, std::string name, float t, glm::vec3 c1, glm::vec3 c2);
void colorgrad(CArray field, std::string name, glm::vec3 c1, glm::vec3 c2);
void colorthreshgrad(CArray field, std::string name, float t, glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4);

/*
================================================================================
                        Index and Vector Conversions
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

glm::vec2 pos(int ind){
  int _x = ind/modes.y;
  int _y = ind-modes.y*_x;
  //Return Binary Vectory
  return glm::vec2(_x, _y);
}

/*
================================================================================
                          Fast Fourier Transform
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


/*
================================================================================
                              Solver Term Helpers
================================================================================
*/
void emptyArray(std::vector<CArray> &delta, unsigned int size){
  //Add blank fields to delta
  CArray zero(0.0, modes.x*modes.y);
  for(unsigned int i = 0; i < size; i++){
    delta.push_back(zero);
  }
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
  //Convert into fourier space,
  //Perform fdiff multiple times
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

CArray cluster(CArray field, int &nclusters){
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
  nclusters = centroids.size();
  return _labels;
}

float autothresh(CArray height, float start, float fraction){
  //Compute the Sealevel
  float test = 0.0;
  while((test - fraction)*(test - fraction) > 0.01){
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


/*
================================================================================
                                  Other
================================================================================
*/

void image(std::string name, CArray a, CArray b, CArray c){
  CImg<unsigned char> img(modes.x, modes.y, 1, 3);
  //Draw the Image
  for(int i = 0; i < img.width(); i++){
    for(int j = 0; j < img.height(); j++){
      //Create the Image
      img(i, j, 0, 0) = a[ind(glm::vec2(i, j))].real();
      img(i, j, 0, 1) = b[ind(glm::vec2(i, j))].real();
      img(i, j, 0, 2) = c[ind(glm::vec2(i, j))].real();
    }
  }
  //Save to File
  img.save_png(name.c_str());
}

void grayscale(CArray field, std::string name){
  //Do the Grayscale Image
  image(name, (complex)255.0*field, (complex)255.0*field, (complex)255.0*field);
}

void colorthresh(CArray field, std::string name, float t, glm::vec3 c1, glm::vec3 c2){
  //Do a colorthreshold
  CArray a(c2.x, modes.x*modes.y);
  a[field > t] = (complex)c1.x;

  CArray b(c2.y, modes.x*modes.y);
  b[field > t] = (complex)c1.y;

  CArray c(c2.z, modes.x*modes.y);
  c[field > t] = (complex)c1.z;

  image(name, a, b, c);
}

void colorgrad(CArray field, std::string name, glm::vec3 c1, glm::vec3 c2){
  //Do a color gradient
  CArray a =  field*(complex)c2.x+((complex)1.0-field)*(complex)c1.x;
  CArray b =  field*(complex)c2.y+((complex)1.0-field)*(complex)c1.y;
  CArray c =  field*(complex)c2.z+((complex)1.0-field)*(complex)c1.z;
  image(name, a, b, c);
}

void colorthreshgrad(CArray field, std::string name, float t, glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4){
  //Do a colorthreshold
  BArray test = field > t;
  CArray a =  field*(complex)c2.x+((complex)1.0-field)*(complex)c1.x;
  a[test] = (field*(complex)c3.x+((complex)1.0-field)*(complex)c4.x)[test];

  CArray b =  field*(complex)c2.y+((complex)1.0-field)*(complex)c1.y;
  b[test] = (field*(complex)c3.y+((complex)1.0-field)*(complex)c4.y)[test];

  CArray c =  field*(complex)c2.z+((complex)1.0-field)*(complex)c1.z;
  c[test] = (field*(complex)c3.z+((complex)1.0-field)*(complex)c4.z)[test];

  image(name, a, b, c);
}

//End of namespace
}

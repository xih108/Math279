#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;

//FUNCTION HEADERS HERE
//   FUNCTIONS FOR READING AND WRITING
double **readpicture(ifstream &infile, int &width, int &height);
char readchar(ifstream& infile, int &num);
int readint(ifstream& infile, int numbytes, int &num);
int char2int(char thechar);
int char2int(char *thechar, int numbytes);
void readrgb(ifstream& infile, double *rgb, int &num);
void writepicture(ofstream& outfile, double **f, int width, int height);
char int2char(unsigned long theint);
void int2char(char *thechar, int theint);
void writechar(ofstream &outfile, char thechar);
void writeint(ofstream &outfile, int theint, int numbytes);
void writergb(ofstream& outfile, double r, double g, double b);
//   FUNCTIONS FOR CREATING AND REMOVING MEMORY
double **matrix(int width, int height);
void free_matrix(double **f, int width, int height);
int **imatrix(int width, int height);
void free_matrix(int **f, int width, int height);
//   YOUR FUNCTIONS HERE

double Lagrange(double* c, double* cg, int m, double fij);

//MAIN BODY HERE
int main()
{
   int width;
   int height;
   double **f;
   double x, y, h;
   int i, j;

// if reading in picture
   ifstream infile("grasshopper.bmp", ios::in | ios::binary);
   f = readpicture(infile,width,height);
   infile.close();
// if creating picture

int m = 32;
double c[m];


int H_f[m];
int CH_f[m];
memset(H_f, 0, sizeof(H_f));
memset(CH_f, 0, sizeof(CH_f));

double fmin = 1.0;
double fmax = 0.0;

//find fmax, fmin
for (i = 0; i < width; i++){
   for (j = 0; j < height; j++){
      
      if (f[i][j] < fmin) {
         fmin = f[i][j];
      }
      if (f[i][j] > fmax) {
         fmax = f[i][j];
      }
   }
}
// printf("%f %f\n",fmax,fmin);

//discrete intesities
for (int k = 0; k < m; k++){
   c[k] = (fmax-fmin)*k/(m-1) + fmin;
   // printf("%d %f\n",k,c[k]);
}

//H_f
for (i = 0; i < width; i++){
   for (j = 0; j < height; j++){
      int k = ceil((f[i][j] - fmin)*(m-1)/(fmax-fmin));
      H_f[k] += 1;  
   }
}

//CH_f
for (int k = 0; k < m; k++){
   if (k >= 1 ){
      CH_f[k] += H_f[k] + CH_f[k-1];
   }
   else{
      CH_f[k] = H_f[0];
   }
   // printf("%d: %f %d\n", k, c[k], CH_f[k]);
}

// g(f)
double cg[m];
memset(cg, 0, sizeof(cg));

int CH_fmax = width*height;
int CH_fmin = CH_f[0];

for (int k = 0; k < m; k++){
   cg[k] = (CH_f[k] - CH_fmin)*1.0/(CH_fmax - CH_fmin);
   // printf("(%f, %f)\n",c[k],cg[k]);
}


//P(fij)
for (i = 0; i < width; i++){
   for (j = 0; j < height; j++){
      //lagrange interpolant
      double fij = f[i][j];
      f[i][j] = Lagrange(c, cg, m, fij);
      
      // printf("%f ", f[i][j]);
   }
}

for (int k = 0; k < m; k++){
   double t = Lagrange(c, cg, m, c[k]);
   // printf("(%f, %f)\n",c[k],t);
}



// int a = 197;
// int b = 127;
// printf("The value at pixel (%d,%d) is %f.\n", a,b,f[a][b]);


// printf("The value at pixel (%d,%d) is %f.\n", a,b,f[a][b]);
//test 

// int H_g[m];
// int CH_g[m]; 
// memset(H_g, 0, sizeof(H_g));
// memset(CH_g, 0, sizeof(CH_g));

// for (i = 0; i < width; i++){
//    for (j = 0; j < height; j++){
//       for (int k =0; k < m; k++){
//          if (f[i][j] <= cg[k]){
//             CH_g[k]+=1;
//          }
//       }
//    }
// }

// for (int k = 0; k < m; k++){
//    printf("%d\n",CH_g[k]);
// }


// writing picture
   ofstream outfile("7(b).bmp", ios::out | ios::binary);
   writepicture(outfile,f,width,height);
   outfile.close();

   free_matrix(f,width,height);
}

//FUNCTION CONTENT HERE

//**************************************************************************
//FUNCTIONS FOR READING AND WRITING
//**************************************************************************
double **readpicture(ifstream &infile, int &width, int &height)
{
   int i, j, k;
   char junk, theformat[2];
   double rgb[3], **f;
   int num = 0;
   int upsidedown = 0;
   cout << "reading picture" << endl;

   for (i = 0; i < 2; i++)
      theformat[i] = readchar(infile,num);
   int filesize = readint(infile,4,num);
   cout << "   numbytes = " << filesize << endl;
   for (i = 0; i < 4; i++)
      junk = readchar(infile,num);
   int offset = readint(infile,4,num);
   int headerstart = num;
   int headersize = readint(infile,4,num);
   if (headersize == 12)
   {
      width = readint(infile,2,num);
      height = readint(infile,2,num);
   }
   else
   {
      width = readint(infile,4,num);
      height = readint(infile,4,num);
   }
   if (height < 0)
   {
      height = abs(height);
      upsidedown = 1;
   }
   cout << "   width = " << width << endl;
   cout << "   height = " << height << endl;
   int numcolorpanes = readint(infile,2,num);
   int bitsperpixel = readint(infile,2,num);
   if (bitsperpixel != 24)
   {
      cout << "ERROR: this program is only built for 1 byte per rgb, not a total of " 
           << bitsperpixel << " bits" << endl;
      exit(1);
   }
   for (i = num-headerstart; i < headersize; i++)
      junk = readchar(infile,num);

   f = matrix(width,height);

   if (upsidedown)
      for (j = height-1; j >= 0; j--)
      {
         for (i = 0; i < width; i++)
         {
            readrgb(infile,rgb,num);
            f[i][j] = 0.0;
            for (k = 0; k < 3; k++)
               f[i][j] += rgb[k]*rgb[k];
            f[i][j] = sqrt(f[i][j])/sqrt(3.0);
         }
         for (i = 0; i < (4-(3*width)%4)%4; i++)
            junk = readchar(infile,num);
      }
   else
      for (j = 0; j < height; j++)
      {
         for (i = 0; i < width; i++)
         {
            readrgb(infile,rgb,num);
            f[i][j] = 0.0;
            for (k = 0; k < 3; k++)
               f[i][j] += rgb[k]*rgb[k];
            f[i][j] = sqrt(f[i][j])/sqrt(3.0);
         }
         for (i = 0; i < (4-(3*width)%4)%4; i++)
            junk = readchar(infile,num);
      }

   return f;
}

int readint(ifstream& infile, int numbytes, int &num)
{
   char *temp;
   int i;
   int theint;

   temp = new char[numbytes];

   for (i = 0; i < numbytes; i++)
      infile.read(reinterpret_cast<char *>(&(temp[i])),sizeof(char));
   theint = char2int(temp,numbytes);

   delete[] temp;
   num += numbytes;

   return theint;
}

int char2int(char thechar)
{
   int i, theint, imask;
   char cmask;

   cmask = 1;
   imask = 1;
   theint = 0;
   for (i = 0; i < 8; i++)
   {
      if (thechar & cmask)
      {
         theint += imask;
      }
      cmask = cmask << 1;
      imask *= 2;
   }

   return theint;
}

int char2int(char *thechar, int numbytes)
{
   int i;
   int theint, power;

   power = 1;
   theint = 0;
   for (i = 0; i < numbytes; i++)
   {
      theint += char2int(thechar[i])*power;
      power = power*256;
   }

   return theint;
}

char readchar(ifstream& infile, int &num)
{
   char thechar;

   infile.read(reinterpret_cast<char *>(&thechar),sizeof(char));
   num++;

   return thechar;
}

void readrgb(ifstream& infile, double *rgb, int &num)
{
   char ctemp;
   int i, itemp, ijunk;

   for (i = 0; i < 3; i++)
   {
//      ctemp = readchar(infile);
//      itemp = char2int(ctemp);
      itemp = readint(infile,1,ijunk);
      rgb[i] = static_cast<double>(itemp)/255.0;
      num++;
   }
}

void writepicture(ofstream& outfile, double **f, int width, int height)
{
   int numbytes;
   int i, j;
   cout << "writing picture" << endl;

   writechar(outfile,66);
   writechar(outfile,77);

// number of bytes
   numbytes = 54+height*(3*width+(4-(3*width)%4)%4);
   cout << "   numbytes = " << numbytes << endl;
   writeint(outfile,numbytes,4);

   writeint(outfile,0,2);
   writeint(outfile,0,2);
   writeint(outfile,54,4);
   writeint(outfile,40,4);

// width
   cout << "   width = " << width << endl;
   writeint(outfile,width,4);
// height
   cout << "   height = " << height << endl;
   writeint(outfile,height,4);

   writeint(outfile,1,2);
   writeint(outfile,24,2);
   writeint(outfile,0,4);
   writeint(outfile,16,4);
   writeint(outfile,2835,4);
   writeint(outfile,2835,4);
   writeint(outfile,0,4);
   writeint(outfile,0,4);

   for (j = 0; j < height; j++)
   {
      for (i = 0; i < width; i++)
         writergb(outfile,f[i][j],f[i][j],f[i][j]);
      for (i = 0; i < (4-(3*width)%4)%4; i++)
         writechar(outfile,0);
   }
}

char int2char(unsigned long theint)
{
   char thechar, mask;
   int i;

   mask = 1;
   thechar = 0;
   for (i = 0; i < 8; i++)
   {
      if (theint%2)
         thechar += mask;
      theint /= 2;
      mask = mask << 1;
   }

   return thechar;
}

void int2char(char *thechar, int theint, int numbytes)
{
   int temp = theint;
   int i;

   for (i = 0; i < numbytes; i++)
   {
      thechar[i] = int2char(temp%256);
      temp = temp/256;
   }
}

void writechar(ofstream& outfile, char thechar)
{
   outfile.write(reinterpret_cast<char *>(&thechar),sizeof(char));
}

void writeint(ofstream &outfile, int theint, int numbytes)
{
   char *temp = new char[numbytes];
   int i;

   int2char(temp,theint,numbytes);
   for (i = 0; i < numbytes; i++)
      outfile.write(reinterpret_cast<char *>(&(temp[i])),sizeof(char));

   delete[] temp;
}

void writergb(ofstream& outfile, double r, double g, double b)
{
   int i, irgb;
   char temp;
   char a;
   double rgb[3];

   rgb[0] = r;
   rgb[1] = g;
   rgb[2] = b;
   for (i = 0; i < 3; i++)
   {
      irgb = static_cast<int>(floor(255.0*rgb[i]+0.5));
      if (irgb < 0)
         irgb = 0;
      else if (irgb > 255)
         irgb = 255;

      temp = int2char(irgb);
      outfile.write(reinterpret_cast<char *>(&temp),sizeof(char));
   }
}

//**************************************************************************
//FUNCTIONS FOR CREATING AND REMOVING MEMORY
//**************************************************************************

double **matrix(int width, int height)
{
   double **f;
   int i;

   f = new double*[width];
   for (i = 0; i < width; i++)
      f[i] = new double[height];

   return f;
}

void free_matrix(double **f, int width, int height)
{
   int i;

   for (i = 0; i < width; i++)
      delete f[i];
   delete f;
}

int **imatrix(int width, int height)
{
   int **f;
   int i;

   f = new int*[width];
   for (i = 0; i < width; i++)
      f[i] = new int[height];

   return f;
}

void free_matrix(int **f, int width, int height)
{
   int i;

   for (i = 0; i < width; i++)
      delete f[i];
   delete f;
}

//**************************************************************************
//YOUR FUNCTIONS HERE
//**************************************************************************
double Lagrange(double* c, double* cg, int m, double fij){
   double Pc = 0;
   for (int k = 0; k < m; k++){
      double Pk = cg[k];
      for (int j = 0; j < m; j++){
         if (j != k) {
            Pk *= (fij - c[j])/ (c[k] - c[j]);
         }
      }
      Pc += Pk;
   }
   // if (Pc < 0){
   //    printf("%f,%f ", Pc, fij);
   // }
   return Pc;
}
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
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
//   ADDITIONAL FUNCTIONS FOR READING AND WRITING
void writecurveinpicture(ofstream& outfile, double **f, double **phi, int width, 
                         int height);
void readcolorpicture(double** &fr, double** &fg, double** &fb, ifstream &infile, 
                      int &width, int &height);
void writecolorpicture(ofstream& outfile, double **fr, double **fg, double **fb, 
                       int width, int height);
//   YOUR FUNCTIONS HERE

//MAIN BODY HERE
int main()
{
   int width;
   int height;
   double **f;
   double x, y, h;
   int i, j;
   // int M, m;
   time_t starttime, endtime;
   double **fr, **fg, **fb;

// if reading in colorpicture
   ifstream infile("oak.bmp", ios::in | ios::binary);
   // readcolorpicture(fr,fg,fb,infile,width,height);
   f = readpicture(infile, width, height);
   cout<<width<<endl;
   infile.close();

// if creating picture
//   width = 300;
//   height = 300;
//   f = matrix(width,height);

   double thetime;
   starttime = clock();
// do all your work you want timed between these two

   endtime = clock();
   cout << "Time taken by program: " 
        << static_cast<double>(endtime-starttime)/CLOCKS_PER_SEC << " sec" << endl;

   double omega = 1.5;
   int *I  = new int[width*height];
   int *J  = new int[width*height];
   int r = 0;
   int R[width][height];
   // corrupt
   for (i = 0; i < width; i++){
      for (j = 0; j < height; j++){

         if ((i % 2 == 1) || (j % 6 == 0) || ((j+1) % 6 == 0) || (j+2) % 6 == 0){
            f[i][j] = 0.5;
            I[r] = i;
            J[r] = j;   
            R[i][j] = r;
            r ++;
         }
         else{
            R[i][j] = -1;
         }
         
      }
   }

   //  vector b   
   double *b = new double[r];
  
   // matrix A
   int *val = new int[5*r];
   int *row = new int[5*r];
   int *col = new int[5*r];
   // memset(val, 0, sizeof(val));
   // memset(row, 0, sizeof(row));
   // memset(col, 0, sizeof(col));
   
   int m = 0;
   for (i = 0; i < r; i++){
      val[m] = -4;
      row[m] = i;
      col[m] = i;
      b[i] = 0;
      int c = m;
      m ++;

      if (I[i] - 1 >= 0){
         if ( R[I[i]-1][J[i]] > -1 ){
            val[m] = 1;
            row[m] = i;
            col[m] = R[I[i]-1][J[i]];
         }    
         else{ 
            b[i] += -f[I[i]-1][J[i]];
            val[m] = 0;
         }     
      }
      else {
         val[c] += 1 ;
         val[m] = 0;
      }
      m++;

      if (J[i] - 1 >= 0){
         if ( R[I[i]][J[i]-1] > -1){
            val[m] = 1;
            row[m] = i;
            col[m] = R[I[i]][J[i]-1];
         }
         else{
            b[i] += -f[I[i]][J[i]-1];
            val[m] = 0;
         }
      }
      else {
         val[c] += 1 ;
         val[m] = 0;
      }
      m ++; 

       if (I[i] + 1 < width){
         if ( R[I[i]+1][J[i]] > -1 )
         {
            val[m] = 1;
            row[m] = i;
            col[m] = R[I[i]+1][J[i]];
         }
         else{
            b[i] += -f[I[i]+1][J[i]];
            val[m] = 0;
         }
      } 
      else {
         val[c] += 1 ;
         val[m] = 0;
      }
      m ++;

      if (J[i] + 1 < height){
         if ( R[I[i]][J[i]+1] > -1){
            val[m] = 1;
            row[m] = i;
            col[m] = R[I[i]][J[i]+1];
         }
         else{
            b[i] += -f[I[i]][J[i]+1];
            val[m] = 0;
         }
      }
      else {
         val[c] += 1 ;
         val[m] = 0;
      }
      m ++;
   }


   cout<<m<<endl;
   int k = 0;
   double *z = new double[r];

   while (true){
   
      for (i = 0; i < r; i++){
         z[i] = f[I[i]][J[i]];
      }
      double diff = 0.0;
      for (i = 0; i < r; i++){
         double mul = 0.0;
         double diag = 0.0;

         for (int s = 0 ; s < 5; s++){
            if (s == 0){
               diag = val[5*i];
            }
            else{
               mul += val[5*i+s] * f[I[col[5*i+s]]][J[col[5*i+s]]];
            }
         }
         
         f[I[i]][J[i]] = (1 - omega)*f[I[i]][J[i]] + omega / diag * (b[i] - mul);    
         double tmp = abs(f[I[i]][J[i]] - z[i]);
         if ( tmp > diff ){
            diff = tmp;
         }
         
         // cout<<i<<" "<<j<<" "<<diff<<endl;
      }

      k += 1;
      cout<<"k: "<<k<<" "<<diff<<endl;
     
      if (diff <= pow(10, -11)){
         break;
      }
   }

   cout<<"time step is "<<k<<endl;

  
   // cout<< "The value at pixel (64,64) is (R: " <<f[64][64]<<", G: "<<gg[64][64]<<", B: "<<gb[64][64]<<")" << endl;
// writing picture
   ofstream outfile("4cfinal.bmp", ios::out | ios::binary);
   // writecolorpicture(outfile,gr,gg,gb,twidth,theight);
   writepicture(outfile,f,width,height);
   outfile.close();

   // free_matrix(fr,width,height);
   // free_matrix(fg,width,height);
   // free_matrix(fb,width,height);
   free_matrix(f, width, height);
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
//ADDITIONAL FUNCTIONS FOR READING AND WRITING
//**************************************************************************

void writecurveinpicture(ofstream& outfile, double **f, double **phi, int width, 
                         int height)
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
      {
         if (phi[i][j]*phi[min(i+1,width-1)][j] < 0.0 ||
             phi[i][j]*phi[max(i-1,0)][j] < 0.0 ||
             phi[i][j]*phi[i][min(j+1,height-1)] < 0.0 ||
             phi[i][j]*phi[i][max(j-1,0)] < 0.0)
            writergb(outfile,0.0,0.0,1.0);
         else
            writergb(outfile,f[i][j],f[i][j],f[i][j]);
      }
      for (i = 0; i < (4-(3*width)%4)%4; i++)
         writechar(outfile,0);
   }
}

void readcolorpicture(double** &fr, double** &fg, double** &fb, ifstream &infile, 
                      int &width, int &height)
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

   fr = matrix(width,height);
   fg = matrix(width,height);
   fb = matrix(width,height);

   if (upsidedown)
      for (j = height-1; j >= 0; j--)
      {
         for (i = 0; i < width; i++)
         {
            readrgb(infile,rgb,num);
            fr[i][j] = rgb[2];
            fg[i][j] = rgb[1];
            fb[i][j] = rgb[0];
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
            fr[i][j] = rgb[2];
            fg[i][j] = rgb[1];
            fb[i][j] = rgb[0];
         }
         for (i = 0; i < (4-(3*width)%4)%4; i++)
            junk = readchar(infile,num);
      }
}
 
void writecolorpicture(ofstream& outfile, double **fr, double **fg, double **fb, 
                       int width, int height)
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
         writergb(outfile,fb[i][j],fg[i][j],fr[i][j]);
      for (i = 0; i < (4-(3*width)%4)%4; i++)
         writechar(outfile,0);
   }
}

//**************************************************************************
//YOUR FUNCTIONS HERE
//**************************************************************************
